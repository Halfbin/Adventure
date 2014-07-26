//
// Adventure
//

#pragma once

#include "Frame.hpp"

#include <memory>

namespace Ad
{
  class Entity
  {
  protected:
    v3i solar_pos;
    v3f field_pos;
    vsf ori;
    v3f bbox_mins, bbox_maxs;

    Entity (v3i new_solar_pos, v3f new_field_pos, vsf new_ori) :
      solar_pos (new_solar_pos),
      field_pos (new_field_pos),
      ori (new_ori)
    { }

  public:
    typedef std::unique_ptr <Entity> Ptr;

    virtual void advance (float time, float step) = 0;
    virtual void draw    (Frame& frame) = 0;
    virtual void collide (v3f normal, float pdist) = 0;

    virtual ~Entity () = 0;

    v3i solar_position () const { return solar_pos; }
    v3f field_position () const { return field_pos; }
    vsf orientation    () const { return ori; }

    v3f bound_mins () const { return field_pos + bbox_mins; }
    v3f bound_maxs () const { return field_pos + bbox_maxs; }

  };

}
