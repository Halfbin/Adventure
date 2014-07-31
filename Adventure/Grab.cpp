//
// Adventure
//

#include "Grab.hpp"

namespace Ad
{
  // shit.
  void grab_int (int& value, Rk::cstring_ref src, int min, int max)
  {
    auto str = Rk::to_string (src);
    std::istringstream ss (str);
    ss >> value;
    if (ss.bad () || !ss.eof ())
      throw std::runtime_error ("Error parsing int in INI");
    if (value < min || value > max)
      throw std::runtime_error ("Integer out-of-range in INI");
  }

  void grab_float (float& value, Rk::cstring_ref src, float min, float max)
  {
    auto str = Rk::to_string (src);
    std::istringstream ss (str);
    ss >> value;
    if (ss.bad () || !ss.eof ())
      throw std::runtime_error ("Error parsing float in INI");
    if (value < min || value > max)
      throw std::runtime_error ("Float out-of-range in INI");
  }

  void grab_v3i (v3i& value, Rk::cstring_ref src, v3i mins, v3i maxs)
  {
    auto str = Rk::to_string (src);
    std::istringstream ss (str);
    ss.setf (ss.skipws);
    ss >> value.x; ss.ignore (5000, ',');
    ss >> value.y; ss.ignore (5000, ',');
    ss >> value.z;
    if (ss.bad () || !ss.eof ())
      throw std::runtime_error ("Error parsing v3i in INI");
    if (Rk::inner (std::logical_or <> (), std::less <> (), value, mins) || Rk::inner (std::logical_or <> (), std::greater <> (), value, maxs))
      throw std::runtime_error ("v3i out-of-range in INI");
  }

  void grab_v3f (v3f& value, Rk::cstring_ref src, v3f mins, v3f maxs)
  {
    auto str = Rk::to_string (src);
    std::istringstream ss (str);
    ss.setf (ss.skipws);
    ss >> value.x; ss.ignore (5000, ',');
    ss >> value.y; ss.ignore (5000, ',');
    ss >> value.z;
    if (ss.bad () || !ss.eof ())
      throw std::runtime_error ("Error parsing v3f in INI");
    if (Rk::inner (std::logical_or <> (), std::less <> (), value, mins) || Rk::inner (std::logical_or <> (), std::greater <> (), value, maxs))
      throw std::runtime_error ("v3f out-of-range in INI");
  }

}
