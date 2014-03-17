//
// Adventure
//

#version 330 core

uniform mat4 model_to_clip;
uniform mat4 model_to_world;

in vec4 attrib_vertpos;
in vec4 attrib_colour;
in vec3 attrib_normal;

smooth out vec4 xformed_vertpos;
smooth out vec4 xformed_colour;
smooth out vec3 xformed_normal;

void main ()
{
  xformed_vertpos = model_to_clip * attrib_vertpos;
  xformed_colour  = attrib_colour;
  xformed_normal  = (model_to_world * vec4 (attrib_normal, 0)).xyz;

  gl_Position = xformed_vertpos;
}
