//
// Adventure
//

#include "Shader.hpp"

#include <Rk/matrix.hpp>

#include <GL/glew.h>

namespace Ad
{
  class DefaultShader :
    public ShaderProgram
  {
    Shader vert, frag;
    ShaderProgram prog;

    uint model_to_clip_loc,
         tcoords_scale_loc;

  public:
    DefaultShader ();
    ~DefaultShader ();

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
      attrib_tcoords = 1,

      tex_unit = 0
    };

    uint model_to_clip () const
    {
      return model_to_clip_loc;
    }

    uint tcoords_scale () const
    {
      return tcoords_scale_loc;
    }

  };

}
