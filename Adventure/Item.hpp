//
// Adventure
//

#pragma once

#include "Entity.hpp"

#include <Rk/string_ref.hpp>

namespace Ad
{
  class Item :
    public Entity
  {
  public:
    typedef std::unique_ptr <Item> Ptr;

  };

  Item::Ptr create_item (Rk::cstring_ref path, v2i pos);

}
