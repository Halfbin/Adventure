//
// Adventure
//

#include "Item.hpp"

#include "Gusher.hpp"

namespace Ad
{
  class Item :
    public Entity
  {
    Geom gusher;

    float t0, t1;

    v3f prev_pos;

    virtual void advance (float time, float step)
    {
      prev_pos = pos;

      t0 = time;
      t1 = time + step;

      pos += v3f {0,0,-1} * step;
    }

    virtual void draw (Frame& frame)
    {
      auto theta = 2 * Rk::lerp (t0, t1, frame.alpha);

      auto draw_ori = Rk::rotation (-0.8f * theta, v3f {0,0,1})
                    * Rk::rotation (0.45f,         v3f {1,0,0})
                    * Rk::rotation (theta,         v3f {0,0,1});

      auto draw_pos = Rk::lerp (prev_pos, pos, frame.alpha);

      auto scale = 0.4f;
      auto offset = Rk::conj (draw_ori, v3f (0, 0, scale));
      frame.draw (gusher, 0, gusher.index_count (), 0, { 1.f, 0.f, 0.f, 0.90f }, draw_pos + offset, draw_ori, { scale, scale, scale });
    }

    virtual void collide (v3f normal, float pdist)
    {
      pos += normal * pdist;
    }

  public:
    Item (Rk::cstring_ref path, v3f pos) :
      Entity (pos, identity),
      gusher (make_gusher ())
    {
      bbox_mins = { -0.5f, -0.5f,  0.0f };
      bbox_maxs = {  0.5f,  0.5f,  0.6f };
    }

    ~Item () = default;

  };

  Entity::Ptr create_item (Rk::cstring_ref path, v3f pos)
  {
    return std::make_unique <Item> (path, pos);
  }

}
