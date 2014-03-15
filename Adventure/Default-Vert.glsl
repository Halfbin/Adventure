//
// Adventure
//

#version 330 core

uniform mat4 model_to_clip;
uniform vec2 tcoords_scale;

in vec2 attrib_vertpos;
in vec2 attrib_tcoords;

smooth out vec4 xformed_vertpos;
smooth out vec2 xformed_tcoords;

void main ()
{
  xformed_vertpos = model_to_clip * vec4 (attrib_vertpos, 0, 1);
  xformed_tcoords = attrib_tcoords;

  gl_Position = xformed_vertpos;
}
