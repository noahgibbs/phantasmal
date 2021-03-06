#include <kernel/kernel.h>
#include <kernel/access.h>
#include <kernel/rsrc.h>
#include <kernel/user.h>

#include <phantasmal/log.h>
#include <phantasmal/exit.h>
#include <phantasmal/lpc_names.h>

#include <type.h>
#include <status.h>
#include <limits.h>

inherit access API_ACCESS;
/* prototypes */
static void cmd_list_exit(object user, string cmd, string str);

#define SPACE10 "          "
#define SPACE40 (SPACE10 + SPACE10 + SPACE10 + SPACE10)

private object room_dtd;        /* DTD for room def'n */

static void upgraded(varargs int clone);

private string ralign10(mixed num, int width)
{
    string str;

    str = SPACE10 + (string) num;
    return str[strlen(str) - width ..];
}

private string ralign40(mixed num, int width)
{
    string str;

    str = SPACE40 + (string) num;
    return str[strlen(str) - width ..];
}


static string read_entire_file(string file) {
  string ret;

  ret = read_file(file);
  if (ret == nil) { return nil; }
  if(strlen(ret) > status(ST_STRSIZE) - 3) {
    error("File '" + file + "' is too large!");
  }

  return ret;
}


/*
 * NAME:	create()
 * DESCRIPTION:	initialize variables
 */
static void create(varargs int clone)
{
  if(!find_object(US_MAKE_ROOM)) compile_object(US_MAKE_ROOM);
}

static void upgraded(varargs int clone) {
  string dtd_file;

  /* Set up room DTD */
  if(room_dtd)
    room_dtd->clear();
  else
    room_dtd = ::clone_object(UNQ_DTD);

  dtd_file = read_entire_file(MAPD_ROOM_DTD);
  room_dtd->load(dtd_file);
}

static void destructed(varargs int clone) {
    if(!clone && room_dtd) {
      destruct_object(room_dtd);
    }
}


/********** Room Functions *****************************************/

/* List MUD rooms */
static void cmd_list_room(object user, string cmd, string str) {
  int*    rooms;
  int     ctr, zone;
  string  tmp;
  object  room, phr;

  if(str) {
    str = STRINGD->trim_whitespace(str);
    str = STRINGD->to_lower(str);
  } else {
    str = "";
  }

  if(str == "all" || str == "world" || str == "mud") {
    user->message("Rooms in MUD (" + ZONED->num_zones() + " zones):\n");

    rooms = ({ });
    for(zone = 0; zone < ZONED->num_zones(); zone++) {
      rooms += MAPD->rooms_in_zone(zone);
    }
  } else if(str == "" || str == "zone") {
    user->message("Rooms in zone:\n");

    room = user->get_location();
    zone = ZONED->get_zone_for_room(room);
    if(zone == -1)
      zone = 0;  /* Unzoned rooms */

    rooms = MAPD->rooms_in_zone(zone);
  } else {
    user->message("Usage: " + cmd + "\n"
		  + "       " + cmd + " world\n");
    return;
  }

  tmp = "";
  for(ctr = 0; ctr < sizeof(rooms); ctr++) {
    room = MAPD->get_room_by_num(rooms[ctr]);
    phr = room->get_brief();
    tmp += ralign10("" + rooms[ctr], 6) + "  ";
    if (room->get_mobile()) {
      tmp += "(body) ";
    } else if (room->get_detail_of()) {
      tmp += "(det)  ";
    } else if (room->get_location()
	       && room->get_location()!=MAPD->get_room_by_num(0)) {
      tmp += "(port) ";
    } else {
      tmp += "(room) ";
    }
    tmp += phr->to_string(user);
    tmp += "\n";
  }

  tmp += "-----\n";
  user->message_scroll(tmp);
}


static void cmd_new_room(object user, string cmd, string str) {
  object room;
  int    roomnum, zonenum;
  string segown;

  if(!str || STRINGD->is_whitespace(str)) {
    roomnum = -1;
  } else if(sscanf(str, "%*s %*s") == 2
	    || sscanf(str, "#%d", roomnum) != 1) {
    user->message("Usage: " + cmd + " [#room num]\n");
    return;
  }

  if(MAPD->get_room_by_num(roomnum)) {
    user->message("There is already a room with that number!\n");
    return;
  }

  segown = OBJNUMD->get_segment_owner(roomnum / 100);
  if(roomnum >= 0 && segown && segown != MAPD) {
    user->message("Room number " + roomnum
		  + " is in a segment reserved for non-rooms!\n");
    return;
  }

  room = clone_object(SIMPLE_ROOM);
  zonenum = -1;
  if(roomnum < 0) {
    zonenum = ZONED->get_zone_for_room(user->get_location());
    if(zonenum < 0) {
      LOGD->write_syslog("Odd, zone is less than 0 when making new room...",
			 LOG_WARN);
      zonenum = 0;
    }
  }
  MAPD->add_room_to_zone(room, roomnum, zonenum);

  zonenum = ZONED->get_zone_for_room(room);
  user->message("Added room #" + room->get_number()
		+ " to zone #" + zonenum
		+ " (" + ZONED->get_name_for_zone(zonenum) + ")" + ".\n");
}


static void cmd_delete_room(object user, string cmd, string str) {
  int    roomnum;
  object room;

  if(!str || STRINGD->is_whitespace(str)
     || sscanf(str, "%*s %*s") == 2 || !sscanf(str, "#%d", roomnum)) {
    user->message("Usage: " + cmd + " #<room num>\n");
    return;
  }

  room = MAPD->get_room_by_num(roomnum);
  if(!room) {
    user->message("No such room as #" + roomnum + ".\n");
    return;
  }

  EXITD->clear_all_exits(room);

  if(room->get_detail_of()) {
    room->get_detail_of()->remove_detail(room);
  }
  if(room->get_location()) {
    room->get_location()->remove_from_container(room);
  }

  destruct_object(room);

  user->message("Room/Portable #" + roomnum + " destroyed.\n");
}


static void cmd_save_rooms(object user, string cmd, string str) {
  string unq_str, argstr;
  mixed* rooms, *args;
  object room;
  int    ctr;

  if(!str || STRINGD->is_whitespace(str)) {
    user->message("Usage: " + cmd + " <file to write>\n");
    user->message("   or  " + cmd
		  + " <file to write> #<num> #<num> #<num>...\n");
    return;
  }

  str = STRINGD->trim_whitespace(str);
  remove_file(str + ".old");
  rename_file(str, str + ".old");  /* Try to remove & rename, just in case */

  if(sizeof(get_dir(str)[0])) {
    user->message("Couldn't make space for file -- can't overwrite!\n");
    return;
  }

  if(sscanf(str, "%*s %*s") != 2) {
    rooms = MAPD->rooms_in_zone(0) - ({ 0 });
  } else {
    int roomnum;

    rooms = ({ });
    sscanf(str, "%s %s", str, argstr);
    args = explode(argstr, " ");
    for(ctr = 0; ctr < sizeof(args); ctr++) {
      if(sscanf(args[ctr], "#%d", roomnum)) {
	rooms += ({ roomnum });
      } else {
	user->message("'" + args[ctr] + "' is not a valid room number.\n");
	return;
      }
    }
  }

  if(!rooms || !sizeof(rooms)) {
    user->message("No rooms to save!\n");
    return;
  }

  user->message("Saving rooms: ");
  for(ctr = 0; ctr < sizeof(rooms); ctr++) {
    room = MAPD->get_room_by_num(rooms[ctr]);

    unq_str = room->to_unq_text();

    if(!write_file(str, unq_str))
      error("Couldn't write rooms to file " + str + "!");
    user->message(".");
  }

  user->message("\nDone!\n");
}

mixed* parse_to_room(string room_file) {
  if(!SYSTEM())
    return nil;

  return UNQ_PARSER->unq_parse_with_dtd(room_file, room_dtd);
}

static void cmd_load_rooms(object user, string cmd, string str) {
  string room_file, argstr;
  mixed* unq_data, *tmp, *args, *rooms;
  int    iter, roomnum;

  if(!access(user->query_name(), "/", FULL_ACCESS)) {
    user->message("Currently only those with full administrative access "
		  + "may load rooms.\n");
    return;
  }

  if(!str || STRINGD->is_whitespace(str)) {
    user->message("Usage: " + cmd + " <file to load>\n");
    return;
  }

  /* If it looks like the admin specified rooms, parse them */
  if(sscanf(str, "%s %s", str, argstr) == 2) {
    rooms = ({ });
    args = explode(argstr, " ");
    for(iter = 0; iter < sizeof(args); iter++) {
      if(sscanf(args[iter], "#%d", roomnum)) {
	rooms += ({ roomnum });
      } else {
	user->message("'" + args[iter]
		      + "' doesn't look like a room number!\n");
	return;
      }
    }
  }

  /* Check validity of file */
  str = STRINGD->trim_whitespace(str);
  if(!sizeof(get_dir(str)[0])) {
    user->message("Can't find file: " + str + "\n");
    return;
  }
  room_file = read_file(str);
  if(!room_file || !strlen(room_file)) {
    user->message("Error reading room file, or file is empty.\n");
    return;
  }
  if(strlen(room_file) > status(ST_STRSIZE) - 3) {
    user->message("Room file is too big!  "
		  + "See Angelbob to increase current limit.");
    return;
  }

  tmp = UNQ_PARSER->basic_unq_parse(room_file);
  if(!tmp) {
    user->message("Cannot parse text as UNQ adding UNQ rooms!\n");
    return;
  }
  tmp = SYSTEM_WIZTOOL->parse_to_room(room_file);
  if(!tmp) {
    user->message("Cannot parse UNQ as rooms adding UNQ rooms!\n");
    return;
  }

  if(rooms) {
    unq_data = ({ });
    for(iter = 0; iter < sizeof(tmp); iter += 2) {
      if(tmp[iter + 1][1][0] == "number") {
	roomnum = tmp[iter + 1][1][1];
	if( sizeof(({ roomnum }) & rooms) ) {
	  unq_data += tmp[iter..iter + 1];
	  rooms -= ({ roomnum });
	}
      }
    }
  } else {
    unq_data = tmp[..];
  }

  if(rooms && sizeof(rooms)) {
    string tmp;

    tmp = "No match loading room numbers: ";
    for(iter = 0; iter < sizeof(rooms); iter++) {
      tmp += "#" + rooms[iter] + " ";
    }
    tmp += "\n";
    user->message(tmp);

    if(sizeof(unq_data)) {
      user->message("Attempting remaining rooms:\n\n");
    } else {
      user->message("No specified rooms found, ignoring.\n");
      return;
    }
  }

  user->message("Registering rooms...\n");
  MAPD->add_dtd_unq_rooms(unq_data, str);
  user->message("Resolving exits...\n");
  EXITD->add_deferred_exits();
  user->message("Done.\n");
}

static void cmd_goto_room(object user, string cmd, string str) {
  int    roomnum;
  object exit, room, mob;

  if(str && sscanf(str, "#%d", roomnum)==1) {
    room = MAPD->get_room_by_num(roomnum);
    if(!room) { /* Not a room, maybe an exit? */
      exit = EXITD->get_exit_by_num(roomnum);
      room = exit->get_from_location();
    }
    if(!room) {
      user->message("Can't locate room #" + roomnum + "\n");
      return;
    }
  } else {
    user->message("Usage: " + cmd + " #<location num>\n");
    return;
  }

  /* Do we really want people teleporting into a details? */
  if(room->get_detail_of()) {
    user->message("Can't teleport to a detail.\n");
    return;
  }

  mob = user->get_body()->get_mobile();

  user->message("You teleport to " + room->get_brief()->to_string(user)
		+ ".\n");
  mob->teleport(room, 1);
  user->show_room_to_player(user->get_location());
}

/******  Exit Functions ****************************************/

static void cmd_new_exit(object user, string cmd, string str) {
  int    roomnum, dir, retcode;
  object room;
  string dirname;
  string type;

  if(str)
    retcode = sscanf(str, "%s #%d %s", dirname, roomnum, type);

  if (str && retcode == 2) type = "twoway";

  if(str && (retcode == 3 || retcode == 2)) {
    room = MAPD->get_room_by_num(roomnum);
    if(!room) {
      user->message("Can't locate room #" + roomnum + "\n");
      return;
    }
    dir = EXITD->direction_by_string(dirname);
    if(dir == -1) {
      user->message("Don't recognize " + dirname + " as direction.\n");
      return;
    }
  } else {
    user->message("Usage: " + cmd + " <direction> #<room number> <type>\n");
    return;
  }

  if(!user->get_location()) {
    user->message("You aren't standing anywhere so you can't make"
		  + " an exit!\n");
    return;
  }
  if(user->get_location()->get_exit(dir)) {
    user->message("There already appears to be an exit in that"
		  + " direction.\n");
    return;
  }

  if (roomnum == 0) {
    user->message("Cannot link to void.\n");
    return;
  }

  if (type=="oneway" || type=="one-way" || type=="1") {
    user->message("You begin creating a one-way exit to '"
		+ room->get_brief()->to_string(user) + "'.\n");
    EXITD->add_oneway_exit_between(user->get_location(), room, dir, -1);

  } else if (type=="twoway" || type=="two-way" || type=="2") {
    if (room->get_exit(EXITD->opposite_direction(dir))) {
      user->message("There appears to be an exit in the other room.\n");
      return;
    }
    user->message("You begin creating a two-way exit to '"
		+ room->get_brief()->to_string(user) + "'.\n");
    EXITD->add_twoway_exit_between(user->get_location(), room, dir, -1, -1);

  }
  user->message("You create the exit successfully.\n");
}

static void cmd_clear_exits(object user, string cmd, string str) {
  if(str && !STRINGD->is_whitespace(str)) {
    user->message("Usage: " + cmd + "\n");
    return;
  }

  if(!user->get_location()) {
    user->message("You aren't anywhere, so you can't " + cmd + "!\n");
    return;
  }
  EXITD->clear_all_exits(user->get_location());
}

static void cmd_remove_exit(object user, string cmd, string str) {
  int    dir;
  object exit, exit2;

  if(!str || STRINGD->is_whitespace(str) || sscanf(str, "%*s %*s") == 2) {
    user->message("Usage: " + cmd + " <direction>\n");
    return;
  }

  str = STRINGD->trim_whitespace(str);
  dir = EXITD->direction_by_string(str);
  if(dir == -1) {
    user->message("'" + str + "' is not a direction.\n");
    return;
  }

  if(!user->get_location()) {
    user->message("You aren't anywhere, so you can't " + cmd + "!\n");
    return;
  }
  exit = user->get_location()->get_exit(dir);
  if(!exit) {
    user->message("No exit found going " + str + ".\n");
    return;
  }

  EXITD->clear_exit(exit);

  user->message("Exit " + str + " destroyed.\n");
}

static void cmd_list_exits(object user, string cmd, string str) {
  int   *tmp, *segs, *exits;
  int    ctr;
  string tmpstr;

  if(str && !STRINGD->is_whitespace(str)) {
    user->message("Usage: " + cmd + "\n");
    return;
  }

  /* Ignore cmd, str for the moment */
  segs = EXITD->get_exit_segments();
  if(!segs) {
    user->message("Can't get segments for exits!\n");
    return;
  }

  tmp = ({ });
  exits = ({ });
  for(ctr = 0; ctr < sizeof(segs); ctr++) {
    tmp = EXITD->exits_in_segment(segs[ctr]);
    if(tmp && sizeof(tmp))
      exits += tmp;
  }

  user->message("Exits:\n");
  tmpstr = "";
  for(ctr = 0; ctr < sizeof(exits); ctr++) {
    object exit;

    exit = EXITD->get_exit_by_num(exits[ctr]);

    tmpstr = "#";
    tmpstr += exit->get_number();
    cmd_list_exit(user, "cmd_list_exit", tmpstr);
  }

  ctr = EXITD->num_deferred_exits();
  if(ctr)
    user->message(ctr + " deferred exits pending.\n");

}

static void cmd_list_exit(object user, string cmd, string str) {
  int    objnum, dir, type;
  string tmpstr;
  object exit, dest, phr, from;

  if(!str || sscanf(str, "#%d", objnum) != 1) {
    user->message("Usage: " + cmd + " #<exit num>\n");
    return;
  }

  /* Ignore cmd, str for the moment */
  tmpstr = "";

  exit = EXITD->get_exit_by_num(objnum);
  if(!exit) {
    user->message("Can't find exit #" + objnum + " \n");
    return;
  }

  dest = exit->get_destination();
  from = exit->get_from_location();

  tmpstr += "Exit #" + exit->get_number();

  if(exit->get_brief())
    tmpstr += " (" + exit->get_brief()->to_string(user) + ") ";
  else
    tmpstr += " (no desc) ";

  dir = exit->get_direction();
  if(dir != -1) {
    phr = EXITD->get_name_for_dir(dir);
    if(!phr)
      tmpstr += "undef direction " + dir;
    else
      tmpstr += phr->to_string(user);
  } else
    tmpstr += "no direction";
  /* Show "from" location as number or commentary */
  tmpstr += " from #";
  if (!from)
    tmpstr += "<none>";
  else if (from->get_number() == -1)
    tmpstr += "<unregistered room>";
  else
    tmpstr += from->get_number();

  /* Show "to" location as number or commentary */
  tmpstr += " to #";
  if(!dest)
    tmpstr += "<none>";
  else if (dest->get_number() == -1)
    tmpstr += "<unregistered room>";
  else
    tmpstr += dest->get_number();
   /* Show type: one-way, two-way, other */
  tmpstr += " type: ";
  type = exit->get_exit_type();
  switch (type) {
    case 1: /* one-way */
           tmpstr += "one-way";
           break;
   case 2: /* two-way */
           tmpstr += "two-way";
           break;
   default: /* unknown */
           tmpstr += type;
           break;
  }

  /* Show "link" location as number or commentary */
  tmpstr += " link #";
  tmpstr += exit->get_link();

  if (exit->is_open())
    tmpstr += " Open";
  else
    tmpstr += " Closed";

  if (exit->is_openable())
    tmpstr += " Openable";

  if (exit->is_container())
    tmpstr += " Container";

  if (exit->is_locked())
    tmpstr += " Locked";

  if (exit->is_lockable())
    tmpstr += " Lockable";

  tmpstr += "\n";

  user->message(tmpstr);
}


static void cmd_add_deferred_exits(object user, string cmd, string str) {
  int num;

  num = EXITD->num_deferred_exits();
  user->message("Deferred exits: " + num + ".\n");

  if(!access(user->query_name(), "/", FULL_ACCESS)) {
    user->message("Currently only those with full administrative access "
		  + "may resolve deferred exits and rooms.\n");
    return;
  }

  EXITD->add_deferred_exits();
  MAPD->do_room_resolution(0);  /* Don't fully resolve, so no errors */

  num = EXITD->num_deferred_exits();
  user->message(num + " deferred exits still pending.\n");
  user->message(sizeof(MAPD->get_deferred_rooms())
		+ " deferred rooms still pending.\n");
}


static void cmd_check_deferred_exits(object user, string cmd, string str) {
  int num;

  if(str)
    str = STRINGD->trim_whitespace(str);

  if(str && str != "") {
    user->message("Usage: " + cmd + "\n");
    return;
  }

  num = EXITD->num_deferred_exits();

  user->message(num + " deferred exits still pending.\n");
  user->message(sizeof(MAPD->get_deferred_rooms())
		+ " deferred rooms still pending.\n");
}
