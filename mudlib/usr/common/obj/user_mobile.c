#include <config.h>
#include <kernel/kernel.h>
#include <map.h>

inherit MOBILE;

/* user associated with this mobile */
object user;

static void create(varargs int clone) {
  ::create(clone);
}

/* overridden functions from mobile.c */
string get_type(void) {
  return "user";
}

object get_user(void) {
  return user;
}

void set_user(object new_user) {
  if (!SYSTEM()) {
    error("Only SYSTEM objects can change the user!");
  }

  user = new_user;
}

void notify_moved(object obj) {
  if (user) {
    user->notify_moved(obj);
  }

  ::notify_moved(obj);
}

/* 
 * Hook function overrides
 */

void hook_say(mixed *args) {
  if (user) {
    user->send_phrase(args[0]->get_brief());
    user->message(" says: " + args[1] + "\r\n");
  }
}

void hook_emote(mixed *args) {
  if(user) {
    user->send_phrase(args[0]->get_brief());
    user->message(" " + args[1] + "\r\n");
  }
}

/* Args -- body, target, verb */
void hook_social(mixed *args) {
  string ret;

  if(user) {
    ret = SOULD->get_social_string(user, args[0], args[1], args[2]);
    if(!ret) {
      user->message("Error getting social string!\r\n");
    } else {
      user->message(ret + "\r\n");
    }
  }
}

void hook_whisper(mixed *args) {
  if (user) {
    user->send_phrase(args[0]->get_brief());
    user->message(" whispers to you: " + args[1] + "\r\n");
  }
}

void hook_whisper_other(mixed *args) {
  if (user) {
    user->send_phrase(args[0]->get_brief());
    user->message(" whispers something to ");
    user->send_phrase(args[1]->get_brief());
    user->message("\r\n");
  }
}

void hook_ask(mixed *args) {
  if (user) {
    user->send_phrase(args[0]->get_brief());
    user->message(" asks you: " + args[1] + "\r\n");
  }
}

void hook_ask_other(mixed *args) {
  if (user) {
    if (args[1]) {
      user->send_phrase(args[0]->get_brief());
      user->message(" asks ");
      user->send_phrase(args[1]->get_brief());
      user->message(": " + args[2] + "\r\n");
    } else {
      user->send_phrase(args[0]->get_brief());
      user->message(" asks: " + args[2] + "\r\n");
    }
  }
}

void hook_leave(mixed *args) {
  if (user) {
    if (args[1] == DIR_TELEPORT) {
      user->send_phrase(args[0]->get_brief());
      user->message(" disappears into thin air.\r\n");
    } else {
      user->send_phrase(args[0]->get_brief()) ;
      user->message(" leaves going ");
      user->send_phrase(EXITD->get_name_for_dir(args[1]));
      user->message("\r\n");
    }
  }
}

void hook_enter(mixed *args) {
  if (user) {
    if (args[1] == DIR_TELEPORT) {
      user->send_phrase(args[0]->get_brief());
      user->message(" appears from thin air.\r\n");
    } else {
      user->send_phrase(args[0]->get_brief()) ;
      user->message(" enters from the ");
      user->send_phrase(EXITD->get_name_for_dir(args[1]));
      user->message("\r\n");
    }
  }
}

void from_dtd_unq(mixed* unq) {
  /* Set the body, location and number fields */
  unq = mobile_from_dtd_unq(unq);

  /* User mobiles don't actually (yet) have any additional data.
     So we can just return. */
}
