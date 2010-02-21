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

#ifndef __HPHP_VECTOR_STRING_H__
#define __HPHP_VECTOR_STRING_H__

#include <cpp/base/array/vector.h>
#include <cpp/base/array/map_string.h>
#include <cpp/base/array/map_variant.h>

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

/**
 * A vector of strings.
 */
class VectorString : public Vector {
 public:
  /**
   * Constructors.
   */
  VectorString(CStrRef v);
  VectorString(const std::vector<ArrayElement *> &elems);
  VectorString(const VectorString *src);
  VectorString(const VectorString *src, CStrRef v);
  VectorString(const VectorString *src, int index, CStrRef v);
  VectorString(const VectorString *src, const VectorString *elems, ArrayOp op);

  /**
   * Implementing ArrayData...
   */
  virtual ssize_t size() const;
  virtual Variant getValue(ssize_t pos) const;

  virtual ArrayData *lval(Variant *&ret, bool copy);
  virtual ArrayData *lval(int64   k, Variant *&ret, bool copy,
                          int64 prehash = -1);
  virtual ArrayData *lval(litstr  k, Variant *&ret, bool copy,
                          int64 prehash = -1);
  virtual ArrayData *lval(CStrRef k, Variant *&ret, bool copy,
                          int64 prehash = -1);
  virtual ArrayData *lval(CVarRef k, Variant *&ret, bool copy,
                          int64 prehash = -1);

  virtual ArrayData *set(int64   k, CVarRef v, bool copy, int64 prehash = -1);
  virtual ArrayData *set(litstr  k, CVarRef v, bool copy, int64 prehash = -1);
  virtual ArrayData *set(CStrRef k, CVarRef v, bool copy, int64 prehash = -1);
  virtual ArrayData *set(CVarRef k, CVarRef v, bool copy, int64 prehash = -1);

  virtual ArrayData *remove(int64   k, bool copy, int64 prehash = -1);
  virtual ArrayData *remove(litstr  k, bool copy, int64 prehash = -1);
  virtual ArrayData *remove(CStrRef k, bool copy, int64 prehash = -1);
  virtual ArrayData *remove(CVarRef k, bool copy, int64 prehash = -1);

  virtual ArrayData *copy() const;
  virtual ArrayData *append(CVarRef v, bool copy);
  virtual ArrayData *append(const ArrayData *elems, ArrayOp op, bool copy);
  virtual ArrayData *insert(ssize_t pos, CVarRef v, bool copy);

  virtual void onSetStatic();

  /**
   * Low level access to underlying data. Should be limited in use.
   */
  const HphpVector<String> &getElems() const { return m_elems;}

  /**
   * Memory allocator methods.
   */
  DECLARE_SMART_ALLOCATION(VectorString, SmartAllocatorImpl::NeedRestore);
  bool calculate(int &size) {
    return m_elems.calculate(size);
  }
  void backup(LinearAllocator &allocator) {
    m_elems.backup(allocator);
  }
  void restore(const char *&data) {
    m_elems.restore(data);
  }
  void sweep() {
    m_elems.sweep();
  }

 protected:
  Variant getImpl(int index) const;

 private:
  HphpVector<String> m_elems;

  bool setImpl(int index, CVarRef v, bool copy, ArrayData *&ret);
  ArrayData *removeImpl(int index, bool copy);

  template<typename T>
    ArrayData *setImpl(const T &k, CVarRef v, bool copy) {
    int index = getIndex(k, true);
    ArrayData *ret = NULL;
    if (setImpl(index, v, copy, ret)) {
      return ret;
    }
    if (v.isString()) {
      return NEW(MapString)(this, String(k), v.toString());
    }
    return NEW(MapVariant)(this, String(k), v);
  }
};

///////////////////////////////////////////////////////////////////////////////
}

#endif // __HPHP_VECTOR_STRING_H__
