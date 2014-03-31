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
    v3f pos;
    vsf ori;
    v3f bbox_mins, bbox_maxs;

    Entity (v3f new_pos, vsf new_ori) :
      pos (new_pos),
      ori (new_ori)
    { }

  public:
    typedef std::unique_ptr <Entity> Ptr;

    virtual void advance (float time, float step) = 0;
    virtual void draw    (Frame& frame) = 0;
    virtual void collide (v3f normal, float pdist) = 0;

    virtual ~Entity () = 0;

    v3f position () const { return pos; }
    vsf orientation () const { return ori; }

    v3f bound_mins () const { return pos + bbox_mins; }
    v3f bound_maxs () const { return pos + bbox_maxs; }

  };

}
