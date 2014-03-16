//
// Adventure
//

#version 330 core

uniform mat4 model_to_clip;

in vec4 attrib_vertpos;
in vec4 attrib_colour;

smooth out vec4 xformed_vertpos;
smooth out vec4 xformed_colour;

void main ()
{
  xformed_vertpos = model_to_clip * attrib_vertpos;
  xformed_colour  = attrib_colour;

  gl_Position = xformed_vertpos;
}
