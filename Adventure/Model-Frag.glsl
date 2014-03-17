//
// Adventure
//

#version 330 core

smooth in vec4 xformed_vertpos;
smooth in vec4 xformed_colour;
smooth in vec3 xformed_normal;

out vec4 frag;

void main ()
{
  vec3 n = normalize (xformed_normal);
  float amount = 0.3 + 0.7 * clamp (dot (n, vec3 (-0.471, 0.471, -0.471)), 0.0, 1.0);
  frag = vec4 (xformed_colour.rgb * amount, xformed_colour.a);
}
