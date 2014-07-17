//
// Adventure
//

#include "StarfieldShader.hpp"

#include <GL/glew.h>

namespace Ad
{
  StarfieldShader::StarfieldShader () :
    vert ("starfield-vert.glsl", GL_VERTEX_SHADER_ARB),
    frag ("starfield-frag.glsl", GL_FRAGMENT_SHADER_ARB)
  {
    prog.add_shader (vert);
    prog.add_shader (frag);

    prog.fix_attrib ("attrib_vertpos", attrib_vertpos);
    prog.fix_attrib ("attrib_colour",  attrib_colour);

    prog.link ();
    prog.use ();

    sky_to_clip_loc  = prog.link_uniform ("sky_to_clip");
    twinkle_loc      = prog.link_uniform ("twinkle");
  }

  StarfieldShader::~StarfieldShader () = default;

}
