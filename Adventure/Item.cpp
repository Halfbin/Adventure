//
// Adventure
//

#include "Item.hpp"

#include "Gusher.hpp"

namespace Ad
{
  class ItemImpl :
    public Item
  {
    Gusher gush;

    v2f position;
    float t0, t1;

    virtual void tick (float time, float step)
    {
      t0 = time;
      t1 = time + step;
    }

    virtual void draw (Frame& frame)
    {
      auto pos = Rk::compose_vector { position, 1.f };
      gush.draw (frame, v4f (1, 0, 0, 1), pos, identity, 16.0f);
    }

  public:
    ItemImpl (Rk::cstring_ref path, v2i pos)
    {
      position = pos;
    }

    ~ItemImpl () = default;

  };

  Item::Ptr create_item (Rk::cstring_ref path, v2i pos)
  {
    return std::make_unique <ItemImpl> (path, pos);
  }

}
