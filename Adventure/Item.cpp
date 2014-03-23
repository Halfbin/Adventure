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
      auto theta = 2 * Rk::lerp (t0, t1, frame.alpha);

      auto ori = Rk::rotation (-0.8f * theta, v3f (0, 0, 1))
               * Rk::rotation (0.45f, v3f (1, 0, 0))
               * Rk::rotation (theta, v3f (0, 0, 1));

      auto scale = 0.4f;
      auto offset = Rk::conj (ori, v3f (0, 0, scale));
      auto pos = offset + Rk::compose_vector { position, 0 };
      gush.draw (frame, v4f (1.f, 0.1f, 0.1f, 0.85f), pos, ori, scale);
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
