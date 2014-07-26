//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>

namespace Ad
{
  struct InitContext
  {
    Rk::cstring_ref data_path;

    std::string data_rel (Rk::cstring_ref rel) const
    {
      return to_string (data_path).append (rel.data (), rel.length ());
    }

  };

}
