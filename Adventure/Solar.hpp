//
// Adventure
//

#pragma once

#include <Rk/string_ref.hpp>

#include <memory>

#include "Frame.hpp"
#include "Model.hpp"

namespace Ad
{
  class Solar
  {
    Model model;
    v3i   pos;
    vsf   ori;
    v3f   scale;

  public:
    Solar (Model new_model, v3i new_pos, vsf new_ori, v3f new_scale = {1,1,1}) :
      model (std::move (new_model)),
      pos   (new_pos),
      ori   (new_ori),
      scale (new_scale)
    { }

    Solar (const Solar&) = delete;
    Solar& operator = (const Solar&) = delete;

    Solar (Solar&& other) :
      model (std::move (other.model)),
      pos   (other.pos),
      ori   (other.ori),
      scale (other.scale)
    { }

    Solar& operator = (Solar&& other)
    {
      model = std::move (other.model);
      pos   = other.pos;
      ori   = other.ori;
      scale = other.scale;
      return *this;
    }

    void draw (Frame& frame);

    v3i solar_position () const { return pos; }
    vsf orientation    () const { return ori; }

  };

  class SolarFactory
  {
  protected:
    v3i pos;
    vsf ori;

  public:
    typedef std::unique_ptr <SolarFactory> Ptr;

    virtual bool  param  (Rk::cstring_ref key, Rk::cstring_ref value);
    virtual Solar create () = 0;

  };

  class SolarType
  {
    static SolarType* head_ptr;
    SolarType*        next_ptr;
    
    Rk::cstring_ref type_name;

  public:
    SolarType (Rk::cstring_ref type_name);

    virtual SolarFactory::Ptr make_factory () = 0;

    Rk::cstring_ref name () const
    {
      return type_name;
    }

    SolarType* next () const
    {
      return next_ptr;
    }

    static SolarType* head ()
    {
      return head_ptr;
    }

  };

}
