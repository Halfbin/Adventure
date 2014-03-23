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
      { 0, 0, -0.75f }, // 0 bottom
      { 0, 0,  0.75f }, // 1 top

      { -1.f, 0.f, -0.33f }, // 2 lower hex
      {   -c,  -s, -0.33f }, // 3
      {    c,  -s, -0.33f }, // 4
      {  1.f, 0.f, -0.33f }, // 5
      {    c,   s, -0.33f }, // 6
      {   -c,   s, -0.33f }, // 7

      { -1.f, 0.f, 0.33f }, // 8 upper hex
      {   -c,  -s, 0.33f }, // 9
      {    c,  -s, 0.33f }, // 10
      {  1.f, 0.f, 0.33f }, // 11
      {    c,   s, 0.33f }, // 12
      {   -c,   s, 0.33f }  // 13
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

    struct Vert
    {
      v3f pos;
      v3f norm;
    };

    static_assert (sizeof (Vert) == 24, "uh-oh");

    struct ModelData
    {
      std::vector <Vert> verts;
      std::vector <u8>   idxs;
    };

    ModelData add_normals (const v3f* verts, const u8* idxs, uint n_idxs)
    {
      ModelData data;

      for (int i = 0; i != n_idxs; i += 3)
      {
        auto a = verts [idxs [i + 0]],
             b = verts [idxs [i + 1]],
             c = verts [idxs [i + 2]];

        auto n = unit (cross (b - a, c - a));

        data.verts.push_back (Vert { a, n });
        data.verts.push_back (Vert { b, n });
        data.verts.push_back (Vert { c, n });
        data.idxs.insert (data.idxs.end (), { u8 (i + 0), u8 (i + 1), u8 (i + 2) });
      }

      return data;
    }

    const ModelData& model ()
    {
      static ModelData data = add_normals (gusher_verts, gusher_tris, 72);
      return data;
    }

  } // local

  Gusher::Gusher () :
    data (model ().verts.size () * sizeof (Vert), model ().verts.data ()),
    idxs (model ().idxs.size  () * sizeof (u8),   model ().idxs.data  ()),
    geom (
      { { ModelShader::attrib_vertpos, data.name (), 3, GL_FLOAT, 24, 0 },
        { ModelShader::attrib_normal,  data.name (), 3, GL_FLOAT, 24, 12 }
      },
      idxs.name ())
  { }

  void Gusher::draw (Frame& frame, v4f col, v3f pos, vsf ori, float scale)
  {
    frame.draw (geom.name (), model ().idxs.size (), GL_UNSIGNED_BYTE, 0, col, pos, ori, v3f { scale, scale, scale });
  }

}
