//
// Adventure
//

#pragma once

#include "Entity.hpp"

#include <Rk/string_ref.hpp>

namespace Ad
{
  Entity::Ptr create_item (Rk::cstring_ref path, v3f pos);

}
