//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>

#include "Model.hpp"
#include "Init.hpp"

namespace Ad
{
  Model load_fl_cmp (const InitContext& init, Rk::cstring_ref path);

}
