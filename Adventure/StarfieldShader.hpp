//
// Adventure
//

#pragma once

#include "Shader.hpp"

namespace Ad
{
  class StarfieldShader
  {
    Shader vert, frag;
    ShaderProgram prog;

    uint sky_to_clip_loc,
         twinkle_loc;

  public:
    StarfieldShader ();
    ~StarfieldShader ();

    void use ()
    {
      prog.use ();
    }

    void done ()
    {
      prog.done ();
    }

    enum
    {
      attrib_vertpos = 0,
      attrib_colour  = 1
    };

    uint sky_to_clip () const
    {
      return sky_to_clip_loc;
    }

    uint twinkle () const
    {
      return twinkle_loc;
    }

  };

}
