//
// Adventure
//

#version 330 core

uniform float twinkle;

smooth in vec4 xformed_vertpos;
smooth in vec4 xformed_colour;

out vec4 frag;

void main ()
{
  frag = vec4 (twinkle * xformed_colour.rgb, 1);
}
