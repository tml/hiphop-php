/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010 Facebook, Inc. (http://www.facebook.com)          |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#include "capability.h"
#include <util/logger.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <pwd.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

static bool setInitialCapabilities() {
  cap_t cap_d = cap_init();
  if (cap_d != NULL) {
    cap_value_t cap_list[] = {CAP_NET_BIND_SERVICE, CAP_SYS_RESOURCE,
                              CAP_SETUID};
    cap_clear(cap_d);

    if (cap_set_flag(cap_d, CAP_PERMITTED, 3, cap_list, CAP_SET) < 0 ||
        cap_set_flag(cap_d, CAP_EFFECTIVE, 3, cap_list, CAP_SET) < 0) {
      Logger::Error("cap_set_flag failed");
      return false;
    }

    if (cap_set_proc(cap_d) == -1) {
      Logger::Error("cap_set_proc failed");
      return false;
    }

    if (cap_free(cap_d) == -1) {
      Logger::Error("cap_free failed");
      return false;
    }

    if (prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0) < 0) {
      Logger::Error("prctl(PR_SET_KEEPCAPS) failed");
      return false;
    }
    prctl(PR_SET_DUMPABLE, 1, 0, 0, 0);
    return true;
  }
  return false;
}

static bool setMinimalCapabilities() {
  cap_t cap_d = cap_init();

  if (cap_d != NULL) {
    cap_value_t cap_list[] = {CAP_NET_BIND_SERVICE, CAP_SYS_RESOURCE};

    cap_clear(cap_d);

    if (cap_set_flag(cap_d, CAP_PERMITTED, 2, cap_list, CAP_SET) < 0 ||
        cap_set_flag(cap_d, CAP_EFFECTIVE, 2, cap_list, CAP_SET) < 0) {
      Logger::Error("cap_set_flag failed");
      return false;
    }

    if (cap_set_proc(cap_d) == -1) {
      Logger::Error("cap_set_proc failed");
      return false;
    }

    if (cap_free(cap_d) == -1) {
      Logger::Error("cap_free failed");
      return false;
    }

    prctl(PR_SET_DUMPABLE, 1, 0, 0, 0);
    return true;
  }
  return false;
}

bool Capability::ChangeUnixUser(uid_t uid) {
  if (setInitialCapabilities()) {
    if (setuid(uid) < 0) {
      Logger::Error("unable to drop privs: %s", strerror(errno));
      return false;
    }

    if (!setMinimalCapabilities()) {
      Logger::Error("unable to set minimal server capabiltiies");
      return false;
    }
    return true;
  }
  return false;
}

bool Capability::ChangeUnixUser(const std::string &username) {
  if (!username.empty()) {
    struct passwd *pw = getpwnam(username.c_str());
    if (pw && pw->pw_uid) {
      return ChangeUnixUser(pw->pw_uid);
    }
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
}
