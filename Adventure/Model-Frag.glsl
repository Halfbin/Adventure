//
// Adventure
//

#version 330 core

smooth in vec4 xformed_vertpos;
smooth in vec4 xformed_colour;

out vec4 frag;

void main ()
{
  frag = xformed_colour;
}
