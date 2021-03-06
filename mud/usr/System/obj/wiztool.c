#include <kernel/kernel.h>
#include <kernel/user.h>

#include <phantasmal/lpc_names.h>

#include <type.h>

inherit SYSTEM_WIZTOOLLIB;

private object user;		/* associated user object */
private string owner;
private string directory;
private object driver;

/* Prototypes */
void upgraded(varargs int clone);

/*
 * NAME:	create()
 * DESCRIPTION:	initialize object
 */
static void create(int clone)
{
  ::create(clone);
  if (clone) {
    user = this_user();
    command_sets = nil;
  } else {
    upgraded(clone);
  }
}

void destructed(int clone) {
  if(SYSTEM()) {
    ::destructed(clone);
  }
}

/* Called by objectd when recompiling */
void upgraded(varargs int clone) {
  if(!SYSTEM())
    return;

  command_sets
    = ({
      ([
	"@set_brief"           : ({ "cmd_set_obj_desc" }),
	"@set_look"            : ({ "cmd_set_obj_desc" }),
	"@set_examine"         : ({ "cmd_set_obj_desc" }),
	"@stat"                : ({ "cmd_stat" }),

	"@set_obj_fl"          : ({ "cmd_set_obj_flag" }),
	"@set_obj_flag"        : ({ "cmd_set_obj_flag" }),
	"@set_obj_flags"       : ({ "cmd_set_obj_flag" }),
	"@set_object_fl"       : ({ "cmd_set_obj_flag" }),
	"@set_object_flag"     : ({ "cmd_set_obj_flag" }),
	"@set_object_flags"    : ({ "cmd_set_obj_flag" }),

	"@set_obj_det"         : ({ "cmd_set_obj_detail" }),
	"@set_obj_detail"      : ({ "cmd_set_obj_detail" }),
	"@set_object_det"      : ({ "cmd_set_obj_detail" }),
	"@set_object_detail"   : ({ "cmd_set_obj_detail" }),
	"@set_obj_par"         : ({ "cmd_set_obj_parent" }),
	"@set_obj_parent"      : ({ "cmd_set_obj_parent" }),
	"@set_object_par"      : ({ "cmd_set_obj_parent" }),
	"@set_object_parent"   : ({ "cmd_set_obj_parent" }),
	"@set_parent"          : ({ "cmd_set_obj_parent" }),

	"@set_object_weight_capacity" : ({ "cmd_set_obj_value" }),
	"@set_object_volume_capacity" : ({ "cmd_set_obj_value" }),
	"@set_object_vol_capacity"    : ({ "cmd_set_obj_value" }),
	"@set_object_length_capacity" : ({ "cmd_set_obj_value" }),
	"@set_object_len_capacity"    : ({ "cmd_set_obj_value" }),
	"@set_object_height_capacity" : ({ "cmd_set_obj_value" }),
	"@set_object_weight_max"      : ({ "cmd_set_obj_value" }),
	"@set_object_volume_max"      : ({ "cmd_set_obj_value" }),
	"@set_object_vol_max"         : ({ "cmd_set_obj_value" }),
	"@set_object_length_max"      : ({ "cmd_set_obj_value" }),
	"@set_object_len_max"         : ({ "cmd_set_obj_value" }),
	"@set_object_height_max"      : ({ "cmd_set_obj_value" }),
	"@set_object_weight_maximum"  : ({ "cmd_set_obj_value" }),
	"@set_object_volume_maximum"  : ({ "cmd_set_obj_value" }),
	"@set_object_vol_maximum"     : ({ "cmd_set_obj_value" }),
	"@set_object_length_maximum"  : ({ "cmd_set_obj_value" }),
	"@set_object_len_maximum"     : ({ "cmd_set_obj_value" }),
	"@set_object_height_maximum"  : ({ "cmd_set_obj_value" }),
	"@set_object_weight_cap"      : ({ "cmd_set_obj_value" }),
	"@set_object_volume_cap"      : ({ "cmd_set_obj_value" }),
	"@set_object_vol_cap"         : ({ "cmd_set_obj_value" }),
	"@set_object_length_cap"      : ({ "cmd_set_obj_value" }),
	"@set_object_len_cap"         : ({ "cmd_set_obj_value" }),
	"@set_object_height_cap"      : ({ "cmd_set_obj_value" }),
	"@set_obj_weight_capacity"    : ({ "cmd_set_obj_value" }),
	"@set_obj_volume_capacity"    : ({ "cmd_set_obj_value" }),
	"@set_obj_vol_capacity"       : ({ "cmd_set_obj_value" }),
	"@set_obj_length_capacity"    : ({ "cmd_set_obj_value" }),
	"@set_obj_len_capacity"       : ({ "cmd_set_obj_value" }),
	"@set_obj_height_capacity"    : ({ "cmd_set_obj_value" }),
	"@set_obj_weight_maximum"     : ({ "cmd_set_obj_value" }),
	"@set_obj_volume_maximum"     : ({ "cmd_set_obj_value" }),
	"@set_obj_vol_maximum"        : ({ "cmd_set_obj_value" }),
	"@set_obj_length_maximum"     : ({ "cmd_set_obj_value" }),
	"@set_obj_len_maximum"        : ({ "cmd_set_obj_value" }),
	"@set_obj_height_maximum"     : ({ "cmd_set_obj_value" }),
	"@set_obj_weight_max"         : ({ "cmd_set_obj_value" }),
	"@set_obj_volume_max"         : ({ "cmd_set_obj_value" }),
	"@set_obj_vol_max"            : ({ "cmd_set_obj_value" }),
	"@set_obj_length_max"         : ({ "cmd_set_obj_value" }),
	"@set_obj_len_max"            : ({ "cmd_set_obj_value" }),
	"@set_obj_height_max"         : ({ "cmd_set_obj_value" }),
	"@set_obj_weight_cap"         : ({ "cmd_set_obj_value" }),
	"@set_obj_volume_cap"         : ({ "cmd_set_obj_value" }),
	"@set_obj_vol_cap"            : ({ "cmd_set_obj_value" }),
	"@set_obj_length_cap"         : ({ "cmd_set_obj_value" }),
	"@set_obj_len_cap"            : ({ "cmd_set_obj_value" }),
	"@set_obj_height_cap"         : ({ "cmd_set_obj_value" }),

	"@set_object_weight"          : ({ "cmd_set_obj_value" }),
	"@set_object_volume"          : ({ "cmd_set_obj_value" }),
	"@set_object_vol"             : ({ "cmd_set_obj_value" }),
	"@set_object_length"          : ({ "cmd_set_obj_value" }),
	"@set_object_len"             : ({ "cmd_set_obj_value" }),
	"@set_object_height"          : ({ "cmd_set_obj_value" }),
	"@set_obj_weight"             : ({ "cmd_set_obj_value" }),
	"@set_obj_volume"             : ({ "cmd_set_obj_value" }),
	"@set_obj_vol"                : ({ "cmd_set_obj_value" }),
	"@set_obj_length"             : ({ "cmd_set_obj_value" }),
	"@set_obj_len"                : ({ "cmd_set_obj_value" }),
	"@set_obj_height"             : ({ "cmd_set_obj_value" }),

	"@new_tag_type"               : ({ "cmd_new_tag_type" }),
	"@list_tags"                  : ({ "cmd_list_tags" }),
        "@set_tag"                    : ({ "cmd_set_tag" }),
        "@set_obj_tag"                : ({ "cmd_set_tag" }),
        "@set_object_tag"             : ({ "cmd_set_tag" }),

	"@add_noun"            : ({ "cmd_add_nouns" }),
	"@add_nouns"           : ({ "cmd_add_nouns" }),
	"@clear_nouns"         : ({ "cmd_clear_nouns" }),
	"@add_adjective"       : ({ "cmd_add_adjectives" }),
	"@add_adjectives"      : ({ "cmd_add_adjectives" }),
	"@add_adj"             : ({ "cmd_add_adjectives" }),
	"@clear_adjectives"    : ({ "cmd_clear_adjectives" }),
	"@clear_adj"           : ({ "cmd_clear_adjectives" }),

	"@move"                : ({ "cmd_move_obj" }),
	"@move_obj"            : ({ "cmd_move_obj" }),

	"@list_room"           : ({ "cmd_list_room" }),
	"@list_rooms"          : ({ "cmd_list_room" }),
	"@list_obj"            : ({ "cmd_list_room" }),
	"@list"                : ({ "cmd_list_room" }),
	"@goto"                : ({ "cmd_goto_room" }),
	"@goto_room"           : ({ "cmd_goto_room" }),
	"@save_room"           : ({ "cmd_save_rooms" }),
	"@save_rooms"          : ({ "cmd_save_rooms" }),
	"@load_room"           : ({ "cmd_load_rooms" }),
	"@load_rooms"          : ({ "cmd_load_rooms" }),
	"@new_room"            : ({ "cmd_new_room" }),
	"@add_room"            : ({ "cmd_new_room" }),
	"@delete_room"         : ({ "cmd_delete_room" }),

	"@make_room"           : ({ "cmd_make_obj" }),
	"@make_port"           : ({ "cmd_make_obj" }),
	"@make_portable"       : ({ "cmd_make_obj" }),
	"@make_detail"         : ({ "cmd_make_obj" }),
	"@make_det"            : ({ "cmd_make_obj" }),
	"@delete"              : ({ "cmd_delete_obj" }),
	"@delete_obj"          : ({ "cmd_delete_obj" }),
	"@delete_object"       : ({ "cmd_delete_obj" }),

	"@new_exit"            : ({ "cmd_new_exit" }),
	"@add_exit"            : ({ "cmd_new_exit" }),
	"@clear_exits"         : ({ "cmd_clear_exits" }),
	"@clear_exit"          : ({ "cmd_clear_exits" }),
	"@remove_exit"         : ({ "cmd_remove_exit" }),
	"@list_exit"           : ({ "cmd_list_exits" }),
	"@list_exits"          : ({ "cmd_list_exits" }),
	"@add_deferred_exits"  : ({ "cmd_add_deferred_exits" }),
	"@add_deferred"        : ({ "cmd_add_deferred_exits" }),
	"@check_deferred"      : ({ "cmd_check_deferred_exits" }),

	"@list_mob"            : ({ "cmd_list_mobiles" }),
	"@list_mobile"         : ({ "cmd_list_mobiles" }),
	"@list_mobiles"        : ({ "cmd_list_mobiles" }),
	"@new_mob"             : ({ "cmd_new_mobile" }),
	"@new_mobile"          : ({ "cmd_new_mobile" }),
	"@delete_mob"          : ({ "cmd_delete_mobile" }),
	"@delete_mobile"       : ({ "cmd_delete_mobile" }),

	"@segment_map"         : ({ "cmd_segment_map" }),
	"@seg_map"             : ({ "cmd_segment_map" }),
	"@segmap"              : ({ "cmd_segment_map" }),
	"@set_segment_zone"    : ({ "cmd_set_segment_zone" }),
	"@set_seg_zone"        : ({ "cmd_set_segment_zone" }),
	"@zone_map"            : ({ "cmd_zone_map" }),
	"@zonemap"             : ({ "cmd_zone_map" }),
        "@new_zone"            : ({ "cmd_new_zone" }),

	"%od_report"           : ({ "cmd_od_report" }),
	"%list_dest"           : ({ "cmd_list_dest" }),
	"%full_rebuild"        : ({ "cmd_full_rebuild" }),

	"@log"                 : ({ "cmd_writelog" }),
	"%log"                 : ({ "cmd_writelog" }),
	"@writelog"            : ({ "cmd_writelog" }),
	"%writelog"            : ({ "cmd_writelog" }),
	"@write_log"           : ({ "cmd_writelog" }),
	"%write_log"           : ({ "cmd_writelog" }),
	"%log_subscribe"       : ({ "cmd_log_subscribe" }),

	"%people"              : ({ "cmd_people" }),
	"@people"              : ({ "cmd_people" }),
	"%who"                 : ({ "cmd_people" }),
	"@who"                 : ({ "cmd_people" }),
	"%whoami"              : ({ "cmd_whoami" }),
	"@whoami"              : ({ "cmd_whoami" }),

	"%status"              : ({ "cmd_status" }),

	"%shutdown"            : ({ "cmd_shutdown" }),
	"%reboot"              : ({ "cmd_reboot" }),
	"%swapout"             : ({ "cmd_swapout" }),
	"%statedump"           : ({ "cmd_statedump" }),
	"%datadump"            : ({ "cmd_datadump" }),
	"%save"                : ({ "cmd_datadump" }),
	"%safesave"            : ({ "cmd_safesave" }),

	"%code"                : ({ "cmd_code" }),
	"%history"             : ({ "cmd_history" }),
	"%clear"               : ({ "cmd_clear" }),
	"%compile"             : ({ "cmd_compile" }),
	"%clone"               : ({ "cmd_clone" }),
	"%destruct"            : ({ "cmd_destruct" }),

	"%access"              : ({ "cmd_access" }),
	"%grant"               : ({ "cmd_grant" }),
	"%ungrant"             : ({ "cmd_ungrant" }),
	"%quota"               : ({ "cmd_quota" }),
	"%rsrc"                : ({ "cmd_rsrc" }),

	"%ed"                  : ({ "cmd_ed" }),
	]),
	});

  if(!find_object(UNQ_DTD))
    compile_object(UNQ_DTD);

  ::upgraded(clone);
}


/*
 * NAME:	message()
 * DESCRIPTION:	pass on a message to the user
 */
static void message(string str)
{
  if(user)
    user->message(str);
  else
    DRIVER->message("From wiztool.c: " + str);
}

/*
 * NAME:	command()
 * DESCRIPTION:	deal with input from user
 */
void command(string cmd, string str) {
  if(previous_object() == user) {
    call_limited("process_command", cmd, str);
  }
}

/*
 * NAME:	process_command()
 * DESCRIPTION:	process user input
 */
static void process_command(string cmd, string str)
{
  string arg;
  int    ctr;
  mixed* command_sets;

  if (query_editor(this_object())) {
    if (strlen(cmd) != 0 && cmd[0] == '!') {
      cmd = cmd[1 ..];
    } else {
      str = editor(str);
      if (str) {
	message(str);
      }
      return;
    }
  }

  command_sets = SYSTEM_WIZTOOL->get_command_sets(this_object());
  for(ctr = 0; ctr < sizeof(command_sets); ctr++) {
    if(command_sets[ctr][cmd]) {
      call_other(this_object(), command_sets[ctr][cmd][0], user, cmd,
		 str == "" ? nil : str);
      return;
    }
  }

  switch (cmd) {
  case "cd":
  case "pwd":
  case "ls":
  case "cp":
  case "mv":
  case "rm":
  case "mkdir":
  case "rmdir":

    call_other(this_object(), "cmd_" + cmd, user, cmd, str == "" ? nil : str);
    break;

  default:
    message("No command: " + cmd + "\n");
    break;
  }
}
