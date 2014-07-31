//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>

#include "Model.hpp"
#include "Init.hpp"

namespace Ad
{
  Model load_rkmodel (const InitContext&, Rk::cstring_ref path);

}
