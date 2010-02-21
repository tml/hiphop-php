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

#ifndef __TEST_EXT_MATH_H__
#define __TEST_EXT_MATH_H__

// >>>>>> Generated by idl.php. Do NOT modify. <<<<<<

#include <test/test_cpp_ext.h>

///////////////////////////////////////////////////////////////////////////////

class TestExtMath : public TestCppExt {
 public:
  virtual bool RunTests(const std::string &which);

  bool test_pi();
  bool test_min();
  bool test_max();
  bool test_abs();
  bool test_is_finite();
  bool test_is_infinite();
  bool test_is_nan();
  bool test_ceil();
  bool test_floor();
  bool test_round();
  bool test_deg2rad();
  bool test_rad2deg();
  bool test_decbin();
  bool test_dechex();
  bool test_decoct();
  bool test_bindec();
  bool test_hexdec();
  bool test_octdec();
  bool test_base_convert();
  bool test_pow();
  bool test_exp();
  bool test_expm1();
  bool test_log10();
  bool test_log1p();
  bool test_log();
  bool test_cos();
  bool test_cosh();
  bool test_sin();
  bool test_sinh();
  bool test_tan();
  bool test_tanh();
  bool test_acos();
  bool test_acosh();
  bool test_asin();
  bool test_asinh();
  bool test_atan();
  bool test_atanh();
  bool test_atan2();
  bool test_hypot();
  bool test_fmod();
  bool test_sqrt();
  bool test_getrandmax();
  bool test_srand();
  bool test_rand();
  bool test_mt_getrandmax();
  bool test_mt_srand();
  bool test_mt_rand();
  bool test_lcg_value();
};

///////////////////////////////////////////////////////////////////////////////

#endif // __TEST_EXT_MATH_H__
