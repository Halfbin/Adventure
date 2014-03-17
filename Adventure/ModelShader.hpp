//
// Adventure
//

#pragma once

#include "Shader.hpp"

namespace Ad
{
  class ModelShader :
    public ShaderProgram
  {
    Shader vert, frag;
    ShaderProgram prog;

    uint model_to_clip_loc,
         model_to_world_loc;

  public:
    ModelShader ();
    ~ModelShader ();

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
      attrib_colour  = 1,
      attrib_normal  = 2
    };

    uint model_to_clip () const
    {
      return model_to_clip_loc;
    }

    uint model_to_world () const
    {
      return model_to_world_loc;
    }

  };

}
