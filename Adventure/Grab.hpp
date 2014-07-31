//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>
#include <Rk/vector.hpp>

namespace Ad
{
  void grab_int   (int& value,   Rk::cstring_ref src, int min = INT_MIN, int max = INT_MAX);
  void grab_float (float& value, Rk::cstring_ref src, float min = -FLT_MAX, float max = FLT_MAX);
  void grab_v3i   (v3i& value,   Rk::cstring_ref src, v3i mins = v3i {INT_MIN,INT_MIN,INT_MIN}, v3i maxs = v3i {INT_MAX,INT_MAX,INT_MAX});
  void grab_v3f   (v3f& value,   Rk::cstring_ref src, v3f mins = v3f {-FLT_MAX,-FLT_MAX,-FLT_MAX}, v3f maxs = v3f {FLT_MAX,FLT_MAX,FLT_MAX});

}
