//
// Adventure
//

#version 330 core

uniform sampler2D tex;

smooth in vec4 xformed_vertpos;
smooth in vec2 xformed_tcoords;

out vec4 frag;

void main ()
{
  frag = texture (tex, xformed_tcoords);
}
