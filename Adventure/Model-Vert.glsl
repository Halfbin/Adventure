//
// Adventure
//

#version 330 core

uniform mat4 model_to_clip;
uniform mat4 model_to_world;

in vec4 attrib_vertpos;
in vec4 attrib_colour;
in vec4 attrib_normal;
in vec2 attrib_tcoords;

smooth out vec4 xformed_vertpos;
smooth out vec4 xformed_colour;
smooth out vec4 xformed_normal;
smooth out vec2 xformed_tcoords;

void main ()
{
  xformed_vertpos = model_to_clip * attrib_vertpos;
  xformed_colour  = attrib_colour;
  xformed_normal  = vec4 ((model_to_world * vec4 (attrib_normal.xyz, 0)).xyz, attrib_normal.w);
  xformed_tcoords = attrib_tcoords;

  gl_Position = xformed_vertpos;
}
