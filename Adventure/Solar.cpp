//
// Adventure
//

#include "Solar.hpp"

#include "Grab.hpp"

namespace Ad
{
  SolarType* SolarType::head_ptr = nullptr;

  SolarType::SolarType (Rk::cstring_ref new_type_name) :
    type_name (new_type_name),
    next_ptr  (head_ptr)
  {
    head_ptr = this;
  }

  void Solar::draw (Frame& frame)
  {
    frame.draw (layer_solar, model, pos, ori, scale);
  }

  bool SolarFactory::param (Rk::cstring_ref key, Rk::cstring_ref value)
  {
    if (key == "pos")
    {
      grab_v3i (pos, value);
    }
    else if (key == "rot")
    {
      v3f rot;
      grab_v3f (rot, value);

      static const float d2r = 0.01745329251f;
      ori = Rk::rotation (d2r * rot.x, v3f {1,0,0})
          * Rk::rotation (d2r * rot.y, v3f {0,1,0})
          * Rk::rotation (d2r * rot.z, v3f {0,0,1});
    }
    else
    {
      return false;
    }
    
    return true;
  }

}
