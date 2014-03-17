//
// Adventure
//

#include "ModelShader.hpp"

#include <GL/glew.h>

namespace Ad
{
  ModelShader::ModelShader () :
    vert ("model-vert.glsl", GL_VERTEX_SHADER_ARB),
    frag ("model-frag.glsl", GL_FRAGMENT_SHADER_ARB)
  {
    prog.add_shader (vert);
    prog.add_shader (frag);

    prog.fix_attrib ("attrib_vertpos", attrib_vertpos);
    prog.fix_attrib ("attrib_colour",  attrib_colour);
    prog.fix_attrib ("attrib_normal",  attrib_normal);

    prog.link ();
    prog.use ();

    model_to_clip_loc  = prog.link_uniform ("model_to_clip");
    model_to_world_loc = prog.link_uniform ("model_to_world");
  }

  ModelShader::~ModelShader () = default;

}
