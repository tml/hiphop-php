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

#include <php/classes/iterator.h>
#include <php/classes/splfile.h>
#include <cpp/ext/ext.h>
#include <cpp/eval/eval.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

/* preface starts */
/* preface finishes */
/* SRC: classes/splfile.php line 120 */
const int64 q_splfileobject_DROP_NEW_LINE = 1LL;
const int64 q_splfileobject_READ_AHEAD = 2LL;
const int64 q_splfileobject_SKIP_EMPTY = 6LL;
const int64 q_splfileobject_READ_CSV = 8LL;
Variant c_splfileobject::os_get(const char *s, int64 hash) {
  return c_splfileinfo::os_get(s, hash);
}
Variant &c_splfileobject::os_lval(const char *s, int64 hash) {
  return c_splfileinfo::os_lval(s, hash);
}
void c_splfileobject::o_get(ArrayElementVec &props) const {
  c_splfileinfo::o_get(props);
}
bool c_splfileobject::o_exists(CStrRef s, int64 hash) const {
  return c_splfileinfo::o_exists(s, hash);
}
Variant c_splfileobject::o_get(CStrRef s, int64 hash) {
  return c_splfileinfo::o_get(s, hash);
}
Variant c_splfileobject::o_set(CStrRef s, int64 hash, CVarRef v,bool forInit /* = false */) {
  return c_splfileinfo::o_set(s, hash, v, forInit);
}
Variant &c_splfileobject::o_lval(CStrRef s, int64 hash) {
  return c_splfileinfo::o_lval(s, hash);
}
Variant c_splfileobject::os_constant(const char *s) {
  int64 hash = hash_string(s);
  switch (hash & 7) {
    case 2:
      HASH_RETURN(0x54B2A676B563E232LL, q_splfileobject_SKIP_EMPTY, SKIP_EMPTY);
      break;
    case 3:
      HASH_RETURN(0x23519E830A6DEC13LL, q_splfileobject_READ_CSV, READ_CSV);
      break;
    case 4:
      HASH_RETURN(0x05ABC27224BDAC64LL, q_splfileobject_DROP_NEW_LINE, DROP_NEW_LINE);
      break;
    case 6:
      HASH_RETURN(0x6ABD9DCA7D94431ELL, q_splfileobject_READ_AHEAD, READ_AHEAD);
      break;
    default:
      break;
  }
  return c_splfileinfo::os_constant(s);
}
IMPLEMENT_CLASS(splfileobject)
ObjectData *c_splfileobject::create(Variant v_filename, Variant v_open_mode //  = "r"
, Variant v_use_include_path //  = false
, Variant v_context //  = null
) {
  init();
  t___construct(v_filename, v_open_mode, v_use_include_path, v_context);
  return this;
}
ObjectData *c_splfileobject::dynCreate(CArrRef params, bool init /* = true */) {
  if (init) {
    int count = params.size();
    if (count <= 1) return (create(params.rvalAt(0)));
    if (count == 2) return (create(params.rvalAt(0), params.rvalAt(1)));
    if (count == 3) return (create(params.rvalAt(0), params.rvalAt(1), params.rvalAt(2)));
    return (create(params.rvalAt(0), params.rvalAt(1), params.rvalAt(2), params.rvalAt(3)));
  } else return this;
}
void c_splfileobject::dynConstruct(CArrRef params) {
  int count = params.size();
  if (count <= 1) (t___construct(params.rvalAt(0)));
  if (count == 2) (t___construct(params.rvalAt(0), params.rvalAt(1)));
  if (count == 3) (t___construct(params.rvalAt(0), params.rvalAt(1), params.rvalAt(2)));
  (t___construct(params.rvalAt(0), params.rvalAt(1), params.rvalAt(2), params.rvalAt(3)));
}
ObjectData *c_splfileobject::cloneImpl() {
  c_splfileobject *obj = NEW(c_splfileobject)();
  cloneSet(obj);
  return obj;
}
void c_splfileobject::cloneSet(c_splfileobject *clone) {
  c_splfileinfo::cloneSet(clone);
}
Variant c_splfileobject::o_invoke(const char *s, CArrRef params, int64 hash, bool fatal) {
  if (hash < 0) hash = hash_string_i(s);
  switch (hash & 127) {
    case 0:
      HASH_GUARD(0x09637D7CA2E33F00LL, fgetc) {
        return (t_fgetc());
      }
      break;
    case 6:
      HASH_GUARD(0x6B2EAD4A44934786LL, getrealpath) {
        return (t_getrealpath());
      }
      HASH_GUARD(0x44CE4DB1CE7E9F86LL, flock) {
        return (t_flock(params.rvalAt(0), ref(const_cast<Array&>(params).lvalAt(1))));
      }
      HASH_GUARD(0x1D3B08AA0AF50F06LL, gettype) {
        return (t_gettype());
      }
      break;
    case 8:
      HASH_GUARD(0x1ADA46FCC8EFEC08LL, isdir) {
        return (t_isdir());
      }
      break;
    case 10:
      HASH_GUARD(0x2FC3A6941D522E0ALL, setflags) {
        return (t_setflags(params.rvalAt(0)));
      }
      break;
    case 14:
      HASH_GUARD(0x32ABF385AD4BE48ELL, getowner) {
        return (t_getowner());
      }
      break;
    case 15:
      HASH_GUARD(0x732EC1BDA8EC520FLL, getchildren) {
        return (t_getchildren());
      }
      break;
    case 19:
      HASH_GUARD(0x642C2D2994B34A13LL, __tostring) {
        return (t___tostring());
      }
      break;
    case 21:
      HASH_GUARD(0x40044334DA397C15LL, haschildren) {
        return (t_haschildren());
      }
      break;
    case 26:
      HASH_GUARD(0x2B7CAC006AF27F9ALL, fflush) {
        return (t_fflush());
      }
      break;
    case 27:
      HASH_GUARD(0x5B33B55D4B7E339BLL, fpassthru) {
        return (t_fpassthru());
      }
      break;
    case 28:
      HASH_GUARD(0x572E108C6731E29CLL, getbasename) {
        int count = params.size();
        if (count <= 0) return (t_getbasename());
        return (t_getbasename(params.rvalAt(0)));
      }
      break;
    case 29:
      HASH_GUARD(0x4C43532D60465F1DLL, isfile) {
        return (t_isfile());
      }
      break;
    case 30:
      HASH_GUARD(0x3E4E7C561D3A541ELL, fgetss) {
        return (t_fgetss(params.rvalAt(0)));
      }
      break;
    case 31:
      HASH_GUARD(0x4BC19906B553C59FLL, getatime) {
        return (t_getatime());
      }
      break;
    case 35:
      HASH_GUARD(0x044B276686B77923LL, fscanf) {
        return (t_fscanf(params.rvalAt(0)));
      }
      break;
    case 37:
      HASH_GUARD(0x5948407CA9CC4DA5LL, setfileclass) {
        int count = params.size();
        if (count <= 0) return (t_setfileclass());
        return (t_setfileclass(params.rvalAt(0)));
      }
      break;
    case 43:
      HASH_GUARD(0x0D6276BAB75513ABLL, getlinktarget) {
        return (t_getlinktarget());
      }
      break;
    case 45:
      HASH_GUARD(0x1C1216F2B7C16CADLL, ftell) {
        return (t_ftell());
      }
      break;
    case 47:
      HASH_GUARD(0x5640A4755D0078AFLL, getctime) {
        return (t_getctime());
      }
      break;
    case 53:
      HASH_GUARD(0x337DEC2D48BDFE35LL, openfile) {
        int count = params.size();
        if (count <= 0) return (t_openfile());
        if (count == 1) return (t_openfile(params.rvalAt(0)));
        if (count == 2) return (t_openfile(params.rvalAt(0), params.rvalAt(1)));
        return (t_openfile(params.rvalAt(0), params.rvalAt(1), params.rvalAt(2)));
      }
      break;
    case 55:
      HASH_GUARD(0x4CEC6AA30E43D437LL, setmaxlinelen) {
        return (t_setmaxlinelen(params.rvalAt(0)));
      }
      break;
    case 56:
      HASH_GUARD(0x3C6D50F3BB8102B8LL, next) {
        return (t_next());
      }
      HASH_GUARD(0x0890F9052322E838LL, fstat) {
        return (t_fstat());
      }
      break;
    case 63:
      HASH_GUARD(0x04C642C6C162243FLL, getpath) {
        return (t_getpath());
      }
      HASH_GUARD(0x7D50FA42F9D4923FLL, getfileinfo) {
        int count = params.size();
        if (count <= 0) return (t_getfileinfo());
        return (t_getfileinfo(params.rvalAt(0)));
      }
      break;
    case 67:
      HASH_GUARD(0x5ACCF9166CD9D043LL, ftruncate) {
        return (t_ftruncate(params.rvalAt(0)));
      }
      HASH_GUARD(0x794FAFD4412AEFC3LL, eof) {
        return (t_eof());
      }
      break;
    case 68:
      HASH_GUARD(0x6413CB5154808C44LL, valid) {
        return (t_valid());
      }
      break;
    case 69:
      HASH_GUARD(0x5676046725D241C5LL, setinfoclass) {
        int count = params.size();
        if (count <= 0) return (t_setinfoclass());
        return (t_setinfoclass(params.rvalAt(0)));
      }
      break;
    case 73:
      HASH_GUARD(0x4282E0231F600049LL, fseek) {
        return (t_fseek(params.rvalAt(0), params.rvalAt(1)));
      }
      break;
    case 74:
      HASH_GUARD(0x01A800A73CD2604ALL, getinode) {
        return (t_getinode());
      }
      break;
    case 77:
      HASH_GUARD(0x1930CE336D39474DLL, getfilename) {
        return (t_getfilename());
      }
      break;
    case 78:
      HASH_GUARD(0x7E978C38D741664ELL, fgetcsv) {
        int count = params.size();
        if (count <= 0) return (t_fgetcsv());
        if (count == 1) return (t_fgetcsv(params.rvalAt(0)));
        if (count == 2) return (t_fgetcsv(params.rvalAt(0), params.rvalAt(1)));
        return (t_fgetcsv(params.rvalAt(0), params.rvalAt(1), params.rvalAt(2)));
      }
      break;
    case 79:
      HASH_GUARD(0x569FC7D8E9401C4FLL, isreadable) {
        return (t_isreadable());
      }
      break;
    case 80:
      HASH_GUARD(0x3A335010F905ACD0LL, setcsvcontrol) {
        int count = params.size();
        if (count <= 0) return (t_setcsvcontrol());
        if (count == 1) return (t_setcsvcontrol(params.rvalAt(0)));
        if (count == 2) return (t_setcsvcontrol(params.rvalAt(0), params.rvalAt(1)));
        return (t_setcsvcontrol(params.rvalAt(0), params.rvalAt(1), params.rvalAt(2)));
      }
      break;
    case 81:
      HASH_GUARD(0x56EDB60C824E8C51LL, key) {
        return (t_key());
      }
      break;
    case 88:
      HASH_GUARD(0x1D5801BB72C51C58LL, islink) {
        return (t_islink());
      }
      break;
    case 90:
      HASH_GUARD(0x25F68E7910FE9CDALL, getmaxlinelen) {
        return (t_getmaxlinelen());
      }
      break;
    case 92:
      HASH_GUARD(0x5B3A4A72846B21DCLL, current) {
        return (t_current());
      }
      break;
    case 95:
      HASH_GUARD(0x0D31D0AC229C615FLL, __construct) {
        int count = params.size();
        if (count <= 1) return (t___construct(params.rvalAt(0)), null);
        if (count == 2) return (t___construct(params.rvalAt(0), params.rvalAt(1)), null);
        if (count == 3) return (t___construct(params.rvalAt(0), params.rvalAt(1), params.rvalAt(2)), null);
        return (t___construct(params.rvalAt(0), params.rvalAt(1), params.rvalAt(2), params.rvalAt(3)), null);
      }
      break;
    case 97:
      HASH_GUARD(0x27E7DBA875AD17E1LL, getflags) {
        return (t_getflags());
      }
      break;
    case 98:
      HASH_GUARD(0x6FE9F691E4A6D962LL, getcsvcontrol) {
        return (t_getcsvcontrol());
      }
      break;
    case 99:
      HASH_GUARD(0x638F2A56B8463A63LL, iswritable) {
        return (t_iswritable());
      }
      break;
    case 101:
      HASH_GUARD(0x05D72365192CE465LL, fwrite) {
        return (t_fwrite(params.rvalAt(0), params.rvalAt(1)));
      }
      break;
    case 103:
      HASH_GUARD(0x00DCC39EDB16AFE7LL, getpathinfo) {
        int count = params.size();
        if (count <= 0) return (t_getpathinfo());
        return (t_getpathinfo(params.rvalAt(0)));
      }
      HASH_GUARD(0x7EF5445C77054C67LL, seek) {
        return (t_seek(params.rvalAt(0)));
      }
      break;
    case 106:
      HASH_GUARD(0x0F9EDEC32565D86ALL, getgroup) {
        return (t_getgroup());
      }
      HASH_GUARD(0x6615B5496D03A6EALL, getsize) {
        return (t_getsize());
      }
      HASH_GUARD(0x1670096FDE27AF6ALL, rewind) {
        return (t_rewind());
      }
      break;
    case 115:
      HASH_GUARD(0x7AE1BE187F18FDF3LL, fgets) {
        return (t_fgets());
      }
      break;
    case 116:
      HASH_GUARD(0x265BDC54C992EE74LL, getmtime) {
        return (t_getmtime());
      }
      break;
    case 120:
      HASH_GUARD(0x25070641C3D924F8LL, getpathname) {
        return (t_getpathname());
      }
      break;
    case 122:
      HASH_GUARD(0x3786834B2A0CCB7ALL, isexecutable) {
        return (t_isexecutable());
      }
      break;
    case 125:
      HASH_GUARD(0x4351578037A06E7DLL, getperms) {
        return (t_getperms());
      }
      break;
    default:
      break;
  }
  return c_splfileinfo::o_invoke(s, params, hash, fatal);
}
Variant c_splfileobject::o_invoke_few_args(const char *s, int64 hash, int count, CVarRef a0, CVarRef a1, CVarRef a2, CVarRef a3, CVarRef a4, CVarRef a5) {
  if (hash < 0) hash = hash_string_i(s);
  switch (hash & 127) {
    case 0:
      HASH_GUARD(0x09637D7CA2E33F00LL, fgetc) {
        return (t_fgetc());
      }
      break;
    case 6:
      HASH_GUARD(0x6B2EAD4A44934786LL, getrealpath) {
        return (t_getrealpath());
      }
      HASH_GUARD(0x44CE4DB1CE7E9F86LL, flock) {
        return (t_flock(a0, ref(a1)));
      }
      HASH_GUARD(0x1D3B08AA0AF50F06LL, gettype) {
        return (t_gettype());
      }
      break;
    case 8:
      HASH_GUARD(0x1ADA46FCC8EFEC08LL, isdir) {
        return (t_isdir());
      }
      break;
    case 10:
      HASH_GUARD(0x2FC3A6941D522E0ALL, setflags) {
        return (t_setflags(a0));
      }
      break;
    case 14:
      HASH_GUARD(0x32ABF385AD4BE48ELL, getowner) {
        return (t_getowner());
      }
      break;
    case 15:
      HASH_GUARD(0x732EC1BDA8EC520FLL, getchildren) {
        return (t_getchildren());
      }
      break;
    case 19:
      HASH_GUARD(0x642C2D2994B34A13LL, __tostring) {
        return (t___tostring());
      }
      break;
    case 21:
      HASH_GUARD(0x40044334DA397C15LL, haschildren) {
        return (t_haschildren());
      }
      break;
    case 26:
      HASH_GUARD(0x2B7CAC006AF27F9ALL, fflush) {
        return (t_fflush());
      }
      break;
    case 27:
      HASH_GUARD(0x5B33B55D4B7E339BLL, fpassthru) {
        return (t_fpassthru());
      }
      break;
    case 28:
      HASH_GUARD(0x572E108C6731E29CLL, getbasename) {
        if (count <= 0) return (t_getbasename());
        return (t_getbasename(a0));
      }
      break;
    case 29:
      HASH_GUARD(0x4C43532D60465F1DLL, isfile) {
        return (t_isfile());
      }
      break;
    case 30:
      HASH_GUARD(0x3E4E7C561D3A541ELL, fgetss) {
        return (t_fgetss(a0));
      }
      break;
    case 31:
      HASH_GUARD(0x4BC19906B553C59FLL, getatime) {
        return (t_getatime());
      }
      break;
    case 35:
      HASH_GUARD(0x044B276686B77923LL, fscanf) {
        return (t_fscanf(a0));
      }
      break;
    case 37:
      HASH_GUARD(0x5948407CA9CC4DA5LL, setfileclass) {
        if (count <= 0) return (t_setfileclass());
        return (t_setfileclass(a0));
      }
      break;
    case 43:
      HASH_GUARD(0x0D6276BAB75513ABLL, getlinktarget) {
        return (t_getlinktarget());
      }
      break;
    case 45:
      HASH_GUARD(0x1C1216F2B7C16CADLL, ftell) {
        return (t_ftell());
      }
      break;
    case 47:
      HASH_GUARD(0x5640A4755D0078AFLL, getctime) {
        return (t_getctime());
      }
      break;
    case 53:
      HASH_GUARD(0x337DEC2D48BDFE35LL, openfile) {
        if (count <= 0) return (t_openfile());
        if (count == 1) return (t_openfile(a0));
        if (count == 2) return (t_openfile(a0, a1));
        return (t_openfile(a0, a1, a2));
      }
      break;
    case 55:
      HASH_GUARD(0x4CEC6AA30E43D437LL, setmaxlinelen) {
        return (t_setmaxlinelen(a0));
      }
      break;
    case 56:
      HASH_GUARD(0x3C6D50F3BB8102B8LL, next) {
        return (t_next());
      }
      HASH_GUARD(0x0890F9052322E838LL, fstat) {
        return (t_fstat());
      }
      break;
    case 63:
      HASH_GUARD(0x04C642C6C162243FLL, getpath) {
        return (t_getpath());
      }
      HASH_GUARD(0x7D50FA42F9D4923FLL, getfileinfo) {
        if (count <= 0) return (t_getfileinfo());
        return (t_getfileinfo(a0));
      }
      break;
    case 67:
      HASH_GUARD(0x5ACCF9166CD9D043LL, ftruncate) {
        return (t_ftruncate(a0));
      }
      HASH_GUARD(0x794FAFD4412AEFC3LL, eof) {
        return (t_eof());
      }
      break;
    case 68:
      HASH_GUARD(0x6413CB5154808C44LL, valid) {
        return (t_valid());
      }
      break;
    case 69:
      HASH_GUARD(0x5676046725D241C5LL, setinfoclass) {
        if (count <= 0) return (t_setinfoclass());
        return (t_setinfoclass(a0));
      }
      break;
    case 73:
      HASH_GUARD(0x4282E0231F600049LL, fseek) {
        return (t_fseek(a0, a1));
      }
      break;
    case 74:
      HASH_GUARD(0x01A800A73CD2604ALL, getinode) {
        return (t_getinode());
      }
      break;
    case 77:
      HASH_GUARD(0x1930CE336D39474DLL, getfilename) {
        return (t_getfilename());
      }
      break;
    case 78:
      HASH_GUARD(0x7E978C38D741664ELL, fgetcsv) {
        if (count <= 0) return (t_fgetcsv());
        if (count == 1) return (t_fgetcsv(a0));
        if (count == 2) return (t_fgetcsv(a0, a1));
        return (t_fgetcsv(a0, a1, a2));
      }
      break;
    case 79:
      HASH_GUARD(0x569FC7D8E9401C4FLL, isreadable) {
        return (t_isreadable());
      }
      break;
    case 80:
      HASH_GUARD(0x3A335010F905ACD0LL, setcsvcontrol) {
        if (count <= 0) return (t_setcsvcontrol());
        if (count == 1) return (t_setcsvcontrol(a0));
        if (count == 2) return (t_setcsvcontrol(a0, a1));
        return (t_setcsvcontrol(a0, a1, a2));
      }
      break;
    case 81:
      HASH_GUARD(0x56EDB60C824E8C51LL, key) {
        return (t_key());
      }
      break;
    case 88:
      HASH_GUARD(0x1D5801BB72C51C58LL, islink) {
        return (t_islink());
      }
      break;
    case 90:
      HASH_GUARD(0x25F68E7910FE9CDALL, getmaxlinelen) {
        return (t_getmaxlinelen());
      }
      break;
    case 92:
      HASH_GUARD(0x5B3A4A72846B21DCLL, current) {
        return (t_current());
      }
      break;
    case 95:
      HASH_GUARD(0x0D31D0AC229C615FLL, __construct) {
        if (count <= 1) return (t___construct(a0), null);
        if (count == 2) return (t___construct(a0, a1), null);
        if (count == 3) return (t___construct(a0, a1, a2), null);
        return (t___construct(a0, a1, a2, a3), null);
      }
      break;
    case 97:
      HASH_GUARD(0x27E7DBA875AD17E1LL, getflags) {
        return (t_getflags());
      }
      break;
    case 98:
      HASH_GUARD(0x6FE9F691E4A6D962LL, getcsvcontrol) {
        return (t_getcsvcontrol());
      }
      break;
    case 99:
      HASH_GUARD(0x638F2A56B8463A63LL, iswritable) {
        return (t_iswritable());
      }
      break;
    case 101:
      HASH_GUARD(0x05D72365192CE465LL, fwrite) {
        return (t_fwrite(a0, a1));
      }
      break;
    case 103:
      HASH_GUARD(0x00DCC39EDB16AFE7LL, getpathinfo) {
        if (count <= 0) return (t_getpathinfo());
        return (t_getpathinfo(a0));
      }
      HASH_GUARD(0x7EF5445C77054C67LL, seek) {
        return (t_seek(a0));
      }
      break;
    case 106:
      HASH_GUARD(0x0F9EDEC32565D86ALL, getgroup) {
        return (t_getgroup());
      }
      HASH_GUARD(0x6615B5496D03A6EALL, getsize) {
        return (t_getsize());
      }
      HASH_GUARD(0x1670096FDE27AF6ALL, rewind) {
        return (t_rewind());
      }
      break;
    case 115:
      HASH_GUARD(0x7AE1BE187F18FDF3LL, fgets) {
        return (t_fgets());
      }
      break;
    case 116:
      HASH_GUARD(0x265BDC54C992EE74LL, getmtime) {
        return (t_getmtime());
      }
      break;
    case 120:
      HASH_GUARD(0x25070641C3D924F8LL, getpathname) {
        return (t_getpathname());
      }
      break;
    case 122:
      HASH_GUARD(0x3786834B2A0CCB7ALL, isexecutable) {
        return (t_isexecutable());
      }
      break;
    case 125:
      HASH_GUARD(0x4351578037A06E7DLL, getperms) {
        return (t_getperms());
      }
      break;
    default:
      break;
  }
  return c_splfileinfo::o_invoke_few_args(s, hash, count, a0, a1, a2, a3, a4, a5);
}
Variant c_splfileobject::os_invoke(const char *c, const char *s, CArrRef params, int64 hash, bool fatal) {
  return c_splfileinfo::os_invoke(c, s, params, hash, fatal);
}
Variant c_splfileobject::o_invoke_from_eval(const char *s, Eval::VariableEnvironment &env, const Eval::FunctionCallExpression *caller, int64 hash, bool fatal) {
  if (hash < 0) hash = hash_string_i(s);
  switch (hash & 127) {
    case 0:
      HASH_GUARD(0x09637D7CA2E33F00LL, fgetc) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_fgetc());
      }
      break;
    case 6:
      HASH_GUARD(0x6B2EAD4A44934786LL, getrealpath) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getrealpath());
      }
      HASH_GUARD(0x44CE4DB1CE7E9F86LL, flock) {
        Variant a0;
        Variant a1;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a1 = ref((*it)->refval(env));
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_flock(a0, ref(a1)));
      }
      HASH_GUARD(0x1D3B08AA0AF50F06LL, gettype) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_gettype());
      }
      break;
    case 8:
      HASH_GUARD(0x1ADA46FCC8EFEC08LL, isdir) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_isdir());
      }
      break;
    case 10:
      HASH_GUARD(0x2FC3A6941D522E0ALL, setflags) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_setflags(a0));
      }
      break;
    case 14:
      HASH_GUARD(0x32ABF385AD4BE48ELL, getowner) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getowner());
      }
      break;
    case 15:
      HASH_GUARD(0x732EC1BDA8EC520FLL, getchildren) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getchildren());
      }
      break;
    case 19:
      HASH_GUARD(0x642C2D2994B34A13LL, __tostring) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t___tostring());
      }
      break;
    case 21:
      HASH_GUARD(0x40044334DA397C15LL, haschildren) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_haschildren());
      }
      break;
    case 26:
      HASH_GUARD(0x2B7CAC006AF27F9ALL, fflush) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_fflush());
      }
      break;
    case 27:
      HASH_GUARD(0x5B33B55D4B7E339BLL, fpassthru) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_fpassthru());
      }
      break;
    case 28:
      HASH_GUARD(0x572E108C6731E29CLL, getbasename) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_getbasename());
        return (t_getbasename(a0));
      }
      break;
    case 29:
      HASH_GUARD(0x4C43532D60465F1DLL, isfile) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_isfile());
      }
      break;
    case 30:
      HASH_GUARD(0x3E4E7C561D3A541ELL, fgetss) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_fgetss(a0));
      }
      break;
    case 31:
      HASH_GUARD(0x4BC19906B553C59FLL, getatime) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getatime());
      }
      break;
    case 35:
      HASH_GUARD(0x044B276686B77923LL, fscanf) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_fscanf(a0));
      }
      break;
    case 37:
      HASH_GUARD(0x5948407CA9CC4DA5LL, setfileclass) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_setfileclass());
        return (t_setfileclass(a0));
      }
      break;
    case 43:
      HASH_GUARD(0x0D6276BAB75513ABLL, getlinktarget) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getlinktarget());
      }
      break;
    case 45:
      HASH_GUARD(0x1C1216F2B7C16CADLL, ftell) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_ftell());
      }
      break;
    case 47:
      HASH_GUARD(0x5640A4755D0078AFLL, getctime) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getctime());
      }
      break;
    case 53:
      HASH_GUARD(0x337DEC2D48BDFE35LL, openfile) {
        Variant a0;
        Variant a1;
        Variant a2;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a1 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a2 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_openfile());
        if (count == 1) return (t_openfile(a0));
        if (count == 2) return (t_openfile(a0, a1));
        return (t_openfile(a0, a1, a2));
      }
      break;
    case 55:
      HASH_GUARD(0x4CEC6AA30E43D437LL, setmaxlinelen) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_setmaxlinelen(a0));
      }
      break;
    case 56:
      HASH_GUARD(0x3C6D50F3BB8102B8LL, next) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_next());
      }
      HASH_GUARD(0x0890F9052322E838LL, fstat) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_fstat());
      }
      break;
    case 63:
      HASH_GUARD(0x04C642C6C162243FLL, getpath) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getpath());
      }
      HASH_GUARD(0x7D50FA42F9D4923FLL, getfileinfo) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_getfileinfo());
        return (t_getfileinfo(a0));
      }
      break;
    case 67:
      HASH_GUARD(0x5ACCF9166CD9D043LL, ftruncate) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_ftruncate(a0));
      }
      HASH_GUARD(0x794FAFD4412AEFC3LL, eof) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_eof());
      }
      break;
    case 68:
      HASH_GUARD(0x6413CB5154808C44LL, valid) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_valid());
      }
      break;
    case 69:
      HASH_GUARD(0x5676046725D241C5LL, setinfoclass) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_setinfoclass());
        return (t_setinfoclass(a0));
      }
      break;
    case 73:
      HASH_GUARD(0x4282E0231F600049LL, fseek) {
        Variant a0;
        Variant a1;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a1 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_fseek(a0, a1));
      }
      break;
    case 74:
      HASH_GUARD(0x01A800A73CD2604ALL, getinode) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getinode());
      }
      break;
    case 77:
      HASH_GUARD(0x1930CE336D39474DLL, getfilename) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getfilename());
      }
      break;
    case 78:
      HASH_GUARD(0x7E978C38D741664ELL, fgetcsv) {
        Variant a0;
        Variant a1;
        Variant a2;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a1 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a2 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_fgetcsv());
        if (count == 1) return (t_fgetcsv(a0));
        if (count == 2) return (t_fgetcsv(a0, a1));
        return (t_fgetcsv(a0, a1, a2));
      }
      break;
    case 79:
      HASH_GUARD(0x569FC7D8E9401C4FLL, isreadable) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_isreadable());
      }
      break;
    case 80:
      HASH_GUARD(0x3A335010F905ACD0LL, setcsvcontrol) {
        Variant a0;
        Variant a1;
        Variant a2;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a1 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a2 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_setcsvcontrol());
        if (count == 1) return (t_setcsvcontrol(a0));
        if (count == 2) return (t_setcsvcontrol(a0, a1));
        return (t_setcsvcontrol(a0, a1, a2));
      }
      break;
    case 81:
      HASH_GUARD(0x56EDB60C824E8C51LL, key) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_key());
      }
      break;
    case 88:
      HASH_GUARD(0x1D5801BB72C51C58LL, islink) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_islink());
      }
      break;
    case 90:
      HASH_GUARD(0x25F68E7910FE9CDALL, getmaxlinelen) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getmaxlinelen());
      }
      break;
    case 92:
      HASH_GUARD(0x5B3A4A72846B21DCLL, current) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_current());
      }
      break;
    case 95:
      HASH_GUARD(0x0D31D0AC229C615FLL, __construct) {
        Variant a0;
        Variant a1;
        Variant a2;
        Variant a3;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a1 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a2 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a3 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 1) return (t___construct(a0), null);
        if (count == 2) return (t___construct(a0, a1), null);
        if (count == 3) return (t___construct(a0, a1, a2), null);
        return (t___construct(a0, a1, a2, a3), null);
      }
      break;
    case 97:
      HASH_GUARD(0x27E7DBA875AD17E1LL, getflags) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getflags());
      }
      break;
    case 98:
      HASH_GUARD(0x6FE9F691E4A6D962LL, getcsvcontrol) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getcsvcontrol());
      }
      break;
    case 99:
      HASH_GUARD(0x638F2A56B8463A63LL, iswritable) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_iswritable());
      }
      break;
    case 101:
      HASH_GUARD(0x05D72365192CE465LL, fwrite) {
        Variant a0;
        Variant a1;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a1 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_fwrite(a0, a1));
      }
      break;
    case 103:
      HASH_GUARD(0x00DCC39EDB16AFE7LL, getpathinfo) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_getpathinfo());
        return (t_getpathinfo(a0));
      }
      HASH_GUARD(0x7EF5445C77054C67LL, seek) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_seek(a0));
      }
      break;
    case 106:
      HASH_GUARD(0x0F9EDEC32565D86ALL, getgroup) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getgroup());
      }
      HASH_GUARD(0x6615B5496D03A6EALL, getsize) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getsize());
      }
      HASH_GUARD(0x1670096FDE27AF6ALL, rewind) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_rewind());
      }
      break;
    case 115:
      HASH_GUARD(0x7AE1BE187F18FDF3LL, fgets) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_fgets());
      }
      break;
    case 116:
      HASH_GUARD(0x265BDC54C992EE74LL, getmtime) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getmtime());
      }
      break;
    case 120:
      HASH_GUARD(0x25070641C3D924F8LL, getpathname) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getpathname());
      }
      break;
    case 122:
      HASH_GUARD(0x3786834B2A0CCB7ALL, isexecutable) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_isexecutable());
      }
      break;
    case 125:
      HASH_GUARD(0x4351578037A06E7DLL, getperms) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getperms());
      }
      break;
    default:
      break;
  }
  return c_splfileinfo::o_invoke_from_eval(s, env, caller, hash, fatal);
}
Variant c_splfileobject::os_invoke_from_eval(const char *c, const char *s, Eval::VariableEnvironment &env, const Eval::FunctionCallExpression *caller, int64 hash, bool fatal) {
  return c_splfileinfo::os_invoke_from_eval(c, s, env, caller, hash, fatal);
}
Variant cw_splfileobject$os_get(const char *s) {
  return c_splfileobject::os_get(s, -1);
}
Variant &cw_splfileobject$os_lval(const char *s) {
  return c_splfileobject::os_lval(s, -1);
}
Variant cw_splfileobject$os_constant(const char *s) {
  return c_splfileobject::os_constant(s);
}
Variant cw_splfileobject$os_invoke(const char *c, const char *s, CArrRef params, bool fatal /* = true */) {
  return c_splfileobject::os_invoke(c, s, params, -1, fatal);
}
void c_splfileobject::init() {
  c_splfileinfo::init();
}
/* SRC: classes/splfile.php line 128 */
void c_splfileobject::t___construct(Variant v_filename, Variant v_open_mode //  = "r"
, Variant v_use_include_path //  = false
, Variant v_context //  = null
) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::__construct);
  bool oldInCtor = gasInCtor(true);
  Variant v_file_name;

  x_hphp_splfileobject___construct(((Object)(this)), toString(v_file_name), toString(v_open_mode), toBoolean(v_use_include_path), v_context);
  gasInCtor(oldInCtor);
} /* function */
/* SRC: classes/splfile.php line 135 */
Variant c_splfileobject::t_current() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::current);
  return x_hphp_splfileobject_current(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 139 */
bool c_splfileobject::t_eof() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::eof);
  return x_hphp_splfileobject_eof(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 143 */
bool c_splfileobject::t_fflush() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::fflush);
  return x_hphp_splfileobject_fflush(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 147 */
String c_splfileobject::t_fgetc() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::fgetc);
  return x_hphp_splfileobject_fgetc(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 151 */
Variant c_splfileobject::t_fgetcsv(CStrRef v_delimiter //  = ","
, CStrRef v_enclosure //  = "\""
, CStrRef v_escape //  = "\\"
) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::fgetcsv);
  return x_hphp_splfileobject_fgetcsv(((Object)(this)), v_delimiter, v_enclosure, v_escape);
} /* function */
/* SRC: classes/splfile.php line 157 */
String c_splfileobject::t_fgets() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::fgets);
  return x_hphp_splfileobject_fgets(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 161 */
String c_splfileobject::t_fgetss(CVarRef v_allowable_tags) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::fgetss);
  return x_hphp_splfileobject_fgetss(((Object)(this)), toString(v_allowable_tags));
} /* function */
/* SRC: classes/splfile.php line 165 */
bool c_splfileobject::t_flock(CVarRef v_operation, Variant v_wouldblock) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::flock);
  return x_hphp_splfileobject_flock(((Object)(this)), ref(v_wouldblock));
} /* function */
/* SRC: classes/splfile.php line 169 */
int64 c_splfileobject::t_fpassthru() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::fpassthru);
  return x_hphp_splfileobject_fpassthru(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 173 */
Variant c_splfileobject::t_fscanf(CVarRef v_format) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::fscanf);
  return invoke_failed("hphp_splfileobject_fscanf", Array(ArrayInit(1).set(0, toInt64(this)).create()), 0x6DB71D850799D76ELL);
} /* function */
/* SRC: classes/splfile.php line 177 */
int64 c_splfileobject::t_fseek(CVarRef v_offset, CVarRef v_whence) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::fseek);
  return x_hphp_splfileobject_fseek(((Object)(this)), toInt64(v_offset), toInt64(v_whence));
} /* function */
/* SRC: classes/splfile.php line 181 */
Variant c_splfileobject::t_fstat() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::fstat);
  return x_hphp_splfileobject_fstat(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 185 */
int64 c_splfileobject::t_ftell() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::ftell);
  return x_hphp_splfileobject_ftell(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 189 */
bool c_splfileobject::t_ftruncate(CVarRef v_size) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::ftruncate);
  return x_hphp_splfileobject_ftruncate(((Object)(this)), toInt64(v_size));
} /* function */
/* SRC: classes/splfile.php line 193 */
int64 c_splfileobject::t_fwrite(CVarRef v_str, CVarRef v_length) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::fwrite);
  return x_hphp_splfileobject_fwrite(((Object)(this)), toString(v_str), toInt64(v_length));
} /* function */
/* SRC: classes/splfile.php line 197 */
Variant c_splfileobject::t_getchildren() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::getChildren);
  return null;
} /* function */
/* SRC: classes/splfile.php line 201 */
Variant c_splfileobject::t_getcsvcontrol() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::getCsvControl);
  return x_hphp_splfileobject_getcvscontrol(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 205 */
int64 c_splfileobject::t_getflags() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::getFlags);
  return x_hphp_splfileobject_getflags(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 209 */
int64 c_splfileobject::t_getmaxlinelen() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::getMaxLineLen);
  return x_hphp_splfileobject_getmaxlinelen(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 213 */
bool c_splfileobject::t_haschildren() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::hasChildren);
  return false;
} /* function */
/* SRC: classes/splfile.php line 217 */
int64 c_splfileobject::t_key() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::key);
  return x_hphp_splfileobject_key(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 221 */
Variant c_splfileobject::t_next() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::next);
  return (x_hphp_splfileobject_next(((Object)(this))), null);
} /* function */
/* SRC: classes/splfile.php line 225 */
Variant c_splfileobject::t_rewind() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::rewind);
  return (x_hphp_splfileobject_rewind(((Object)(this))), null);
} /* function */
/* SRC: classes/splfile.php line 229 */
Variant c_splfileobject::t_seek(CVarRef v_line_pos) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::seek);
  return (x_hphp_splfileobject_seek(((Object)(this)), toInt64(v_line_pos)), null);
} /* function */
/* SRC: classes/splfile.php line 233 */
Variant c_splfileobject::t_setcsvcontrol(CStrRef v_delimiter //  = ","
, CStrRef v_enclosure //  = "\""
, CStrRef v_escape //  = "\\"
) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::setCsvControl);
  return (x_hphp_splfileobject_setcsvcontrol(((Object)(this)), v_delimiter, v_enclosure, v_escape), null);
} /* function */
/* SRC: classes/splfile.php line 239 */
Variant c_splfileobject::t_setflags(CVarRef v_flags) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::setFlags);
  return (x_hphp_splfileobject_setflags(((Object)(this)), toInt64(v_flags)), null);
} /* function */
/* SRC: classes/splfile.php line 243 */
Variant c_splfileobject::t_setmaxlinelen(CVarRef v_max_len) {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::setMaxLineLen);
  return (x_hphp_splfileobject_setmaxlinelen(((Object)(this)), toInt64(v_max_len)), null);
} /* function */
/* SRC: classes/splfile.php line 247 */
bool c_splfileobject::t_valid() {
  INSTANCE_METHOD_INJECTION(SplFileObject, SplFileObject::valid);
  return x_hphp_splfileobject_valid(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 3 */
Variant c_splfileinfo::os_get(const char *s, int64 hash) {
  return c_ObjectData::os_get(s, hash);
}
Variant &c_splfileinfo::os_lval(const char *s, int64 hash) {
  return c_ObjectData::os_lval(s, hash);
}
void c_splfileinfo::o_get(ArrayElementVec &props) const {
  c_ObjectData::o_get(props);
}
bool c_splfileinfo::o_exists(CStrRef s, int64 hash) const {
  return c_ObjectData::o_exists(s, hash);
}
Variant c_splfileinfo::o_get(CStrRef s, int64 hash) {
  return c_ObjectData::o_get(s, hash);
}
Variant c_splfileinfo::o_set(CStrRef s, int64 hash, CVarRef v,bool forInit /* = false */) {
  return c_ObjectData::o_set(s, hash, v, forInit);
}
Variant &c_splfileinfo::o_lval(CStrRef s, int64 hash) {
  return c_ObjectData::o_lval(s, hash);
}
Variant c_splfileinfo::os_constant(const char *s) {
  return c_ObjectData::os_constant(s);
}
IMPLEMENT_CLASS(splfileinfo)
ObjectData *c_splfileinfo::create(Variant v_file_name) {
  init();
  t___construct(v_file_name);
  return this;
}
ObjectData *c_splfileinfo::dynCreate(CArrRef params, bool init /* = true */) {
  if (init) {
    return (create(params.rvalAt(0)));
  } else return this;
}
void c_splfileinfo::dynConstruct(CArrRef params) {
  (t___construct(params.rvalAt(0)));
}
ObjectData *c_splfileinfo::cloneImpl() {
  c_splfileinfo *obj = NEW(c_splfileinfo)();
  cloneSet(obj);
  return obj;
}
void c_splfileinfo::cloneSet(c_splfileinfo *clone) {
  ObjectData::cloneSet(clone);
}
Variant c_splfileinfo::o_invoke(const char *s, CArrRef params, int64 hash, bool fatal) {
  if (hash < 0) hash = hash_string_i(s);
  switch (hash & 63) {
    case 5:
      HASH_GUARD(0x5676046725D241C5LL, setinfoclass) {
        int count = params.size();
        if (count <= 0) return (t_setinfoclass());
        return (t_setinfoclass(params.rvalAt(0)));
      }
      break;
    case 6:
      HASH_GUARD(0x6B2EAD4A44934786LL, getrealpath) {
        return (t_getrealpath());
      }
      HASH_GUARD(0x1D3B08AA0AF50F06LL, gettype) {
        return (t_gettype());
      }
      break;
    case 8:
      HASH_GUARD(0x1ADA46FCC8EFEC08LL, isdir) {
        return (t_isdir());
      }
      break;
    case 10:
      HASH_GUARD(0x01A800A73CD2604ALL, getinode) {
        return (t_getinode());
      }
      break;
    case 13:
      HASH_GUARD(0x1930CE336D39474DLL, getfilename) {
        return (t_getfilename());
      }
      break;
    case 14:
      HASH_GUARD(0x32ABF385AD4BE48ELL, getowner) {
        return (t_getowner());
      }
      break;
    case 15:
      HASH_GUARD(0x569FC7D8E9401C4FLL, isreadable) {
        return (t_isreadable());
      }
      break;
    case 19:
      HASH_GUARD(0x642C2D2994B34A13LL, __tostring) {
        return (t___tostring());
      }
      break;
    case 24:
      HASH_GUARD(0x1D5801BB72C51C58LL, islink) {
        return (t_islink());
      }
      break;
    case 28:
      HASH_GUARD(0x572E108C6731E29CLL, getbasename) {
        int count = params.size();
        if (count <= 0) return (t_getbasename());
        return (t_getbasename(params.rvalAt(0)));
      }
      break;
    case 29:
      HASH_GUARD(0x4C43532D60465F1DLL, isfile) {
        return (t_isfile());
      }
      break;
    case 31:
      HASH_GUARD(0x0D31D0AC229C615FLL, __construct) {
        return (t___construct(params.rvalAt(0)), null);
      }
      HASH_GUARD(0x4BC19906B553C59FLL, getatime) {
        return (t_getatime());
      }
      break;
    case 35:
      HASH_GUARD(0x638F2A56B8463A63LL, iswritable) {
        return (t_iswritable());
      }
      break;
    case 37:
      HASH_GUARD(0x5948407CA9CC4DA5LL, setfileclass) {
        int count = params.size();
        if (count <= 0) return (t_setfileclass());
        return (t_setfileclass(params.rvalAt(0)));
      }
      break;
    case 39:
      HASH_GUARD(0x00DCC39EDB16AFE7LL, getpathinfo) {
        int count = params.size();
        if (count <= 0) return (t_getpathinfo());
        return (t_getpathinfo(params.rvalAt(0)));
      }
      break;
    case 42:
      HASH_GUARD(0x0F9EDEC32565D86ALL, getgroup) {
        return (t_getgroup());
      }
      HASH_GUARD(0x6615B5496D03A6EALL, getsize) {
        return (t_getsize());
      }
      break;
    case 43:
      HASH_GUARD(0x0D6276BAB75513ABLL, getlinktarget) {
        return (t_getlinktarget());
      }
      break;
    case 47:
      HASH_GUARD(0x5640A4755D0078AFLL, getctime) {
        return (t_getctime());
      }
      break;
    case 52:
      HASH_GUARD(0x265BDC54C992EE74LL, getmtime) {
        return (t_getmtime());
      }
      break;
    case 53:
      HASH_GUARD(0x337DEC2D48BDFE35LL, openfile) {
        int count = params.size();
        if (count <= 0) return (t_openfile());
        if (count == 1) return (t_openfile(params.rvalAt(0)));
        if (count == 2) return (t_openfile(params.rvalAt(0), params.rvalAt(1)));
        return (t_openfile(params.rvalAt(0), params.rvalAt(1), params.rvalAt(2)));
      }
      break;
    case 56:
      HASH_GUARD(0x25070641C3D924F8LL, getpathname) {
        return (t_getpathname());
      }
      break;
    case 58:
      HASH_GUARD(0x3786834B2A0CCB7ALL, isexecutable) {
        return (t_isexecutable());
      }
      break;
    case 61:
      HASH_GUARD(0x4351578037A06E7DLL, getperms) {
        return (t_getperms());
      }
      break;
    case 63:
      HASH_GUARD(0x04C642C6C162243FLL, getpath) {
        return (t_getpath());
      }
      HASH_GUARD(0x7D50FA42F9D4923FLL, getfileinfo) {
        int count = params.size();
        if (count <= 0) return (t_getfileinfo());
        return (t_getfileinfo(params.rvalAt(0)));
      }
      break;
    default:
      break;
  }
  return c_ObjectData::o_invoke(s, params, hash, fatal);
}
Variant c_splfileinfo::o_invoke_few_args(const char *s, int64 hash, int count, CVarRef a0, CVarRef a1, CVarRef a2, CVarRef a3, CVarRef a4, CVarRef a5) {
  if (hash < 0) hash = hash_string_i(s);
  switch (hash & 63) {
    case 5:
      HASH_GUARD(0x5676046725D241C5LL, setinfoclass) {
        if (count <= 0) return (t_setinfoclass());
        return (t_setinfoclass(a0));
      }
      break;
    case 6:
      HASH_GUARD(0x6B2EAD4A44934786LL, getrealpath) {
        return (t_getrealpath());
      }
      HASH_GUARD(0x1D3B08AA0AF50F06LL, gettype) {
        return (t_gettype());
      }
      break;
    case 8:
      HASH_GUARD(0x1ADA46FCC8EFEC08LL, isdir) {
        return (t_isdir());
      }
      break;
    case 10:
      HASH_GUARD(0x01A800A73CD2604ALL, getinode) {
        return (t_getinode());
      }
      break;
    case 13:
      HASH_GUARD(0x1930CE336D39474DLL, getfilename) {
        return (t_getfilename());
      }
      break;
    case 14:
      HASH_GUARD(0x32ABF385AD4BE48ELL, getowner) {
        return (t_getowner());
      }
      break;
    case 15:
      HASH_GUARD(0x569FC7D8E9401C4FLL, isreadable) {
        return (t_isreadable());
      }
      break;
    case 19:
      HASH_GUARD(0x642C2D2994B34A13LL, __tostring) {
        return (t___tostring());
      }
      break;
    case 24:
      HASH_GUARD(0x1D5801BB72C51C58LL, islink) {
        return (t_islink());
      }
      break;
    case 28:
      HASH_GUARD(0x572E108C6731E29CLL, getbasename) {
        if (count <= 0) return (t_getbasename());
        return (t_getbasename(a0));
      }
      break;
    case 29:
      HASH_GUARD(0x4C43532D60465F1DLL, isfile) {
        return (t_isfile());
      }
      break;
    case 31:
      HASH_GUARD(0x0D31D0AC229C615FLL, __construct) {
        return (t___construct(a0), null);
      }
      HASH_GUARD(0x4BC19906B553C59FLL, getatime) {
        return (t_getatime());
      }
      break;
    case 35:
      HASH_GUARD(0x638F2A56B8463A63LL, iswritable) {
        return (t_iswritable());
      }
      break;
    case 37:
      HASH_GUARD(0x5948407CA9CC4DA5LL, setfileclass) {
        if (count <= 0) return (t_setfileclass());
        return (t_setfileclass(a0));
      }
      break;
    case 39:
      HASH_GUARD(0x00DCC39EDB16AFE7LL, getpathinfo) {
        if (count <= 0) return (t_getpathinfo());
        return (t_getpathinfo(a0));
      }
      break;
    case 42:
      HASH_GUARD(0x0F9EDEC32565D86ALL, getgroup) {
        return (t_getgroup());
      }
      HASH_GUARD(0x6615B5496D03A6EALL, getsize) {
        return (t_getsize());
      }
      break;
    case 43:
      HASH_GUARD(0x0D6276BAB75513ABLL, getlinktarget) {
        return (t_getlinktarget());
      }
      break;
    case 47:
      HASH_GUARD(0x5640A4755D0078AFLL, getctime) {
        return (t_getctime());
      }
      break;
    case 52:
      HASH_GUARD(0x265BDC54C992EE74LL, getmtime) {
        return (t_getmtime());
      }
      break;
    case 53:
      HASH_GUARD(0x337DEC2D48BDFE35LL, openfile) {
        if (count <= 0) return (t_openfile());
        if (count == 1) return (t_openfile(a0));
        if (count == 2) return (t_openfile(a0, a1));
        return (t_openfile(a0, a1, a2));
      }
      break;
    case 56:
      HASH_GUARD(0x25070641C3D924F8LL, getpathname) {
        return (t_getpathname());
      }
      break;
    case 58:
      HASH_GUARD(0x3786834B2A0CCB7ALL, isexecutable) {
        return (t_isexecutable());
      }
      break;
    case 61:
      HASH_GUARD(0x4351578037A06E7DLL, getperms) {
        return (t_getperms());
      }
      break;
    case 63:
      HASH_GUARD(0x04C642C6C162243FLL, getpath) {
        return (t_getpath());
      }
      HASH_GUARD(0x7D50FA42F9D4923FLL, getfileinfo) {
        if (count <= 0) return (t_getfileinfo());
        return (t_getfileinfo(a0));
      }
      break;
    default:
      break;
  }
  return c_ObjectData::o_invoke_few_args(s, hash, count, a0, a1, a2, a3, a4, a5);
}
Variant c_splfileinfo::os_invoke(const char *c, const char *s, CArrRef params, int64 hash, bool fatal) {
  return c_ObjectData::os_invoke(c, s, params, hash, fatal);
}
Variant c_splfileinfo::o_invoke_from_eval(const char *s, Eval::VariableEnvironment &env, const Eval::FunctionCallExpression *caller, int64 hash, bool fatal) {
  if (hash < 0) hash = hash_string_i(s);
  switch (hash & 63) {
    case 5:
      HASH_GUARD(0x5676046725D241C5LL, setinfoclass) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_setinfoclass());
        return (t_setinfoclass(a0));
      }
      break;
    case 6:
      HASH_GUARD(0x6B2EAD4A44934786LL, getrealpath) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getrealpath());
      }
      HASH_GUARD(0x1D3B08AA0AF50F06LL, gettype) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_gettype());
      }
      break;
    case 8:
      HASH_GUARD(0x1ADA46FCC8EFEC08LL, isdir) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_isdir());
      }
      break;
    case 10:
      HASH_GUARD(0x01A800A73CD2604ALL, getinode) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getinode());
      }
      break;
    case 13:
      HASH_GUARD(0x1930CE336D39474DLL, getfilename) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getfilename());
      }
      break;
    case 14:
      HASH_GUARD(0x32ABF385AD4BE48ELL, getowner) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getowner());
      }
      break;
    case 15:
      HASH_GUARD(0x569FC7D8E9401C4FLL, isreadable) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_isreadable());
      }
      break;
    case 19:
      HASH_GUARD(0x642C2D2994B34A13LL, __tostring) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t___tostring());
      }
      break;
    case 24:
      HASH_GUARD(0x1D5801BB72C51C58LL, islink) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_islink());
      }
      break;
    case 28:
      HASH_GUARD(0x572E108C6731E29CLL, getbasename) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_getbasename());
        return (t_getbasename(a0));
      }
      break;
    case 29:
      HASH_GUARD(0x4C43532D60465F1DLL, isfile) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_isfile());
      }
      break;
    case 31:
      HASH_GUARD(0x0D31D0AC229C615FLL, __construct) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t___construct(a0), null);
      }
      HASH_GUARD(0x4BC19906B553C59FLL, getatime) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getatime());
      }
      break;
    case 35:
      HASH_GUARD(0x638F2A56B8463A63LL, iswritable) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_iswritable());
      }
      break;
    case 37:
      HASH_GUARD(0x5948407CA9CC4DA5LL, setfileclass) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_setfileclass());
        return (t_setfileclass(a0));
      }
      break;
    case 39:
      HASH_GUARD(0x00DCC39EDB16AFE7LL, getpathinfo) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_getpathinfo());
        return (t_getpathinfo(a0));
      }
      break;
    case 42:
      HASH_GUARD(0x0F9EDEC32565D86ALL, getgroup) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getgroup());
      }
      HASH_GUARD(0x6615B5496D03A6EALL, getsize) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getsize());
      }
      break;
    case 43:
      HASH_GUARD(0x0D6276BAB75513ABLL, getlinktarget) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getlinktarget());
      }
      break;
    case 47:
      HASH_GUARD(0x5640A4755D0078AFLL, getctime) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getctime());
      }
      break;
    case 52:
      HASH_GUARD(0x265BDC54C992EE74LL, getmtime) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getmtime());
      }
      break;
    case 53:
      HASH_GUARD(0x337DEC2D48BDFE35LL, openfile) {
        Variant a0;
        Variant a1;
        Variant a2;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a1 = (*it)->eval(env);
          it++;
          if (it == params.end()) break;
          a2 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_openfile());
        if (count == 1) return (t_openfile(a0));
        if (count == 2) return (t_openfile(a0, a1));
        return (t_openfile(a0, a1, a2));
      }
      break;
    case 56:
      HASH_GUARD(0x25070641C3D924F8LL, getpathname) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getpathname());
      }
      break;
    case 58:
      HASH_GUARD(0x3786834B2A0CCB7ALL, isexecutable) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_isexecutable());
      }
      break;
    case 61:
      HASH_GUARD(0x4351578037A06E7DLL, getperms) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getperms());
      }
      break;
    case 63:
      HASH_GUARD(0x04C642C6C162243FLL, getpath) {
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        return (t_getpath());
      }
      HASH_GUARD(0x7D50FA42F9D4923FLL, getfileinfo) {
        Variant a0;
        const std::vector<Eval::ExpressionPtr> &params = caller->params();
        std::vector<Eval::ExpressionPtr>::const_iterator it = params.begin();
        do {
          if (it == params.end()) break;
          a0 = (*it)->eval(env);
          it++;
        } while(false);
        for (; it != params.end(); ++it) {
          (*it)->eval(env);
        }
        int count = params.size();
        if (count <= 0) return (t_getfileinfo());
        return (t_getfileinfo(a0));
      }
      break;
    default:
      break;
  }
  return c_ObjectData::o_invoke_from_eval(s, env, caller, hash, fatal);
}
Variant c_splfileinfo::os_invoke_from_eval(const char *c, const char *s, Eval::VariableEnvironment &env, const Eval::FunctionCallExpression *caller, int64 hash, bool fatal) {
  return c_ObjectData::os_invoke_from_eval(c, s, env, caller, hash, fatal);
}
Variant cw_splfileinfo$os_get(const char *s) {
  return c_splfileinfo::os_get(s, -1);
}
Variant &cw_splfileinfo$os_lval(const char *s) {
  return c_splfileinfo::os_lval(s, -1);
}
Variant cw_splfileinfo$os_constant(const char *s) {
  return c_splfileinfo::os_constant(s);
}
Variant cw_splfileinfo$os_invoke(const char *c, const char *s, CArrRef params, bool fatal /* = true */) {
  return c_splfileinfo::os_invoke(c, s, params, -1, fatal);
}
void c_splfileinfo::init() {
}
/* SRC: classes/splfile.php line 5 */
void c_splfileinfo::t___construct(Variant v_file_name) {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::__construct);
  bool oldInCtor = gasInCtor(true);
  x_hphp_splfileinfo___construct(((Object)(this)), toString(v_file_name));
  gasInCtor(oldInCtor);
} /* function */
/* SRC: classes/splfile.php line 9 */
String c_splfileinfo::t_getpath() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getPath);
  return x_hphp_splfileinfo_getpath(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 13 */
String c_splfileinfo::t_getfilename() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getFilename);
  return x_hphp_splfileinfo_getfilename(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 17 */
Object c_splfileinfo::t_getfileinfo(CStrRef v_class_name //  = ""
) {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getFileInfo);
  return x_hphp_splfileinfo_getfileinfo(((Object)(this)), v_class_name);
} /* function */
/* SRC: classes/splfile.php line 21 */
String c_splfileinfo::t_getbasename(CStrRef v_suffix //  = ""
) {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getBasename);
  return x_hphp_splfileinfo_getbasename(((Object)(this)), v_suffix);
} /* function */
/* SRC: classes/splfile.php line 25 */
String c_splfileinfo::t_getpathname() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getPathname);
  return x_hphp_splfileinfo_getpathname(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 29 */
Object c_splfileinfo::t_getpathinfo(CStrRef v_class_name //  = ""
) {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getPathInfo);
  return x_hphp_splfileinfo_getpathinfo(((Object)(this)), v_class_name);
} /* function */
/* SRC: classes/splfile.php line 33 */
int64 c_splfileinfo::t_getperms() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getPerms);
  return x_hphp_splfileinfo_getperms(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 37 */
int64 c_splfileinfo::t_getinode() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getInode);
  return x_hphp_splfileinfo_getinode(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 41 */
int64 c_splfileinfo::t_getsize() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getSize);
  return x_hphp_splfileinfo_getsize(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 45 */
int64 c_splfileinfo::t_getowner() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getOwner);
  return x_hphp_splfileinfo_getowner(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 49 */
int64 c_splfileinfo::t_getgroup() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getGroup);
  return x_hphp_splfileinfo_getgroup(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 53 */
int64 c_splfileinfo::t_getatime() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getATime);
  return x_hphp_splfileinfo_getatime(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 57 */
int64 c_splfileinfo::t_getmtime() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getMTime);
  return x_hphp_splfileinfo_getmtime(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 61 */
int64 c_splfileinfo::t_getctime() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getCTime);
  return x_hphp_splfileinfo_getctime(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 65 */
String c_splfileinfo::t_gettype() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getType);
  return x_hphp_splfileinfo_gettype(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 69 */
bool c_splfileinfo::t_iswritable() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::isWritable);
  return x_hphp_splfileinfo_iswritable(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 73 */
bool c_splfileinfo::t_isreadable() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::isReadable);
  return x_hphp_splfileinfo_isreadable(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 77 */
bool c_splfileinfo::t_isexecutable() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::isExecutable);
  return x_hphp_splfileinfo_isexecutable(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 81 */
bool c_splfileinfo::t_isfile() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::isFile);
  return x_hphp_splfileinfo_isfile(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 85 */
bool c_splfileinfo::t_isdir() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::isDir);
  return x_hphp_splfileinfo_isdir(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 89 */
bool c_splfileinfo::t_islink() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::isLink);
  return x_hphp_splfileinfo_islink(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 93 */
String c_splfileinfo::t_getlinktarget() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getLinkTarget);
  return x_hphp_splfileinfo_getlinktarget(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 97 */
Variant c_splfileinfo::t_getrealpath() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::getRealPath);
  return x_hphp_splfileinfo_getrealpath(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 101 */
String c_splfileinfo::t___tostring() {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::__toString);
  return x_hphp_splfileinfo___tostring(((Object)(this)));
} /* function */
/* SRC: classes/splfile.php line 105 */
Object c_splfileinfo::t_openfile(CStrRef v_mode //  = "r"
, bool v_use_include_path //  = false
, CVarRef v_context //  = null_variant
) {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::openFile);
  return x_hphp_splfileinfo_openfile(((Object)(this)), v_mode, v_use_include_path, v_context);
} /* function */
/* SRC: classes/splfile.php line 111 */
Variant c_splfileinfo::t_setfileclass(CStrRef v_class_name //  = "SplFileObject"
) {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::setFileClass);
  return (x_hphp_splfileinfo_setfileclass(((Object)(this)), v_class_name), null);
} /* function */
/* SRC: classes/splfile.php line 115 */
Variant c_splfileinfo::t_setinfoclass(CStrRef v_class_name //  = "SplFileInfo"
) {
  INSTANCE_METHOD_INJECTION(SplFileInfo, SplFileInfo::setInfoClass);
  return (x_hphp_splfileinfo_setinfoclass(((Object)(this)), v_class_name), null);
} /* function */
Object co_splfileobject(CArrRef params, bool init /* = true */) {
  return Object(p_splfileobject(NEW(c_splfileobject)())->dynCreate(params, init));
}
Object co_splfileinfo(CArrRef params, bool init /* = true */) {
  return Object(p_splfileinfo(NEW(c_splfileinfo)())->dynCreate(params, init));
}
Variant pm_php$classes$splfile_php(bool incOnce /* = false */, LVariableTable* variables /* = NULL */) {
  FUNCTION_INJECTION(run_init::classes/splfile.php);
  {
    DECLARE_SYSTEM_GLOBALS(g);
    bool &alreadyRun = g->run_pm_php$classes$splfile_php;
    if (alreadyRun) { if (incOnce) return true;}
    else alreadyRun = true;
    if (!variables) variables = g;
  }
  DECLARE_SYSTEM_GLOBALS(g);
  LVariableTable *gVariables __attribute__((__unused__)) = get_variable_table();
  return true;
} /* function */

///////////////////////////////////////////////////////////////////////////////
}
