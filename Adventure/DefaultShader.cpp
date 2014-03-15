//
// Adventure
//

#include "DefaultShader.hpp"

#include <GL/glew.h>

namespace Ad
{
  DefaultShader::DefaultShader () :
    vert ("default-vert.glsl", GL_VERTEX_SHADER_ARB),
    frag ("default-frag.glsl", GL_FRAGMENT_SHADER_ARB)
  {
    prog.add_shader (vert);
    prog.add_shader (frag);

    prog.fix_attrib ("attrib_vertpos", attrib_vertpos);
    prog.fix_attrib ("attrib_tcoords", attrib_tcoords);

    prog.link ();
    prog.use ();

    auto tex = prog.link_uniform ("tex");
    glUniform1i (tex, tex_unit);

    model_to_clip_loc = prog.link_uniform ("model_to_clip");
    tcoords_scale_loc = prog.link_uniform ("tcoords_scale");
  }

  DefaultShader::~DefaultShader () = default;

}
