//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>

#include <algorithm>

namespace Ad
{
  struct CStringRefLess
  {
    bool operator () (Rk::cstring_ref lhs, Rk::cstring_ref rhs)
    {
      return std::lexicographical_compare (lhs.begin (), lhs.end (), rhs.begin (), rhs.end ());
    }

  };

}
