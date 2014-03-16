//
// Adventure
//

#include "Gusher.hpp"

#include "ModelShader.hpp"

namespace Ad
{
  // Basically an elongated hexagonal bipyramid

  namespace
  {
    const float s = 0.8660254037f; // sin 60 = sqrt (3) / 2
    const float c = 0.5f;          // cos 60 =    1     / 2

    const v3f gusher_verts [14] = {
      { 0, 0, -1 }, // 0 bottom
      { 0, 0,  1 }, // 1 top

      { -1.f, 0.f, -0.5f }, // 2 lower hex
      {   -c,  -s, -0.5f }, // 3
      {    c,  -s, -0.5f }, // 4
      {  1.f, 0.f, -0.5f }, // 5
      {    c,   s, -0.5f }, // 6
      {   -c,   s, -0.5f }, // 7

      { -1.f, 0.f, 0.5f }, // 8 upper hex
      {   -c,  -s, 0.5f }, // 9
      {    c,  -s, 0.5f }, // 10
      {  1.f, 0.f, 0.5f }, // 11
      {    c,   s, 0.5f }, // 12
      {   -c,   s, 0.5f }  // 13
    };

    const u8 gusher_tris [72] = {
      0, 3, 2, // underside
      0, 4, 3,
      0, 5, 4,
      0, 6, 5,
      0, 7, 6,
      0, 2, 7,

      1,  8,  9, // cap
      1,  9, 10,
      1, 10, 11,
      1, 11, 12,
      1, 12, 13,
      1, 13,  8,

      2,  3,  8,  3,  9,  8, // A
      3,  4,  9,  4, 10,  9, // B
      4,  5, 10,  5, 11, 10, // C
      5,  6, 11,  6, 12, 11, // D
      6,  7, 12,  7, 13, 12, // E
      7,  2, 13,  2,  8, 13  // F
    };

  } // local

  Gusher::Gusher () :
    data (sizeof (gusher_verts), gusher_verts),
    idxs (sizeof (gusher_tris),  gusher_tris),
    geom (
      { { ModelShader::attrib_vertpos, data.name (), 3, GL_FLOAT, 0, 0 } },
      idxs.name ())
  { }

  void Gusher::draw (Frame& frame, v4f col, v3f pos, vsf ori, float scale)
  {
    frame.draw (geom.name (), 72, GL_UNSIGNED_BYTE, col, pos, ori, v3f { scale, scale, scale });
  }

}
