//
// Adventure
//

#version 330 core

uniform sampler2D tex;

smooth in vec4 xformed_vertpos;
smooth in vec4 xformed_colour;
smooth in vec4 xformed_normal;
smooth in vec2 xformed_tcoords;

out vec4 frag;

void main ()
{
  vec3 n = normalize (xformed_normal.xyz);
  float light = clamp (n.z, 0.0, 1.0);
  float amount = mix (1, light, xformed_normal.w);
  vec4 col = vec4 (texture (tex, xformed_tcoords).rgb + xformed_colour.rgb, xformed_colour.a);
  frag = vec4 (col.rgb * amount, col.a);
}
