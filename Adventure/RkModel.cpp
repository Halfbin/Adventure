//
// Adventure
//

#include "RkModel.hpp"

#include "Buffer.hpp"

#include <Rk/file_stream.hpp>

#include <GL/glew.h>

namespace Ad
{
  namespace RkModel
  {
    struct Header
    {
      char signature [8];
      u32  version;
      u16  geom_count;
      u16  mesh_count;
      u16  attrib_count;
      u16  material_count;
      u32  reserved;
    };

    static_assert (sizeof (Header) % 8 == 0, "sizeof (Header) not a multiple of 8");

    struct Geom
    {
      u16 vertex_size;
      u16 index_size;
      u32 vertex_count;
      u32 index_count;
      u16 attrib_count;
      u16 attrib_stride;
    };

    static_assert (sizeof (Geom) % 8 == 0, "sizeof (Geom) not a multiple of 8");

    struct Attrib
    {
      u8  location;
      u8  flags;
      u8  component_count;
      u8  component_size;
      u32 reserved_2;
    };

    static_assert (sizeof (Attrib) % 8 == 0, "sizeof (Attrib) not a multiple of 8");

    enum AttribFlags
    {
      attrib_normalize = 1,
      attrib_integer   = 2,
      attrib_unsigned  = 4
    };

    struct Mesh
    {
      u16 geom;
      u16 prim_type;
      u32 vertex_base;
      u32 vertex_max;
      u32 index_first;
      u32 index_count;
      u16 material;
      u16 reserved;
    };

    static_assert (sizeof (Mesh) % 8 == 0, "sizeof (Mesh) not a multiple of 8");

    enum PrimType
    {
      prim_points = 0,
      prim_lines,
      prim_line_strips,
      prim_triangles,
      prim_triangle_strips,
      prim_triangle_fans
    };

  }

  /*
  MODEL STRUCTURE

  header
  geoms
  attribs
  meshes
  data
  */

  std::vector <u8> load_file (Rk::cstring_ref path)
  {
    Rk::fio::in_stream file (path);
    std::vector <u8> buffer ((u32) file.size ());
    file.read (buffer.data (), buffer.size ());
    return buffer;
  }

  uint to_gltype (int size, bool integral, bool unsign)
  {
    if (integral)
    {
      switch (size)
      {
        case 1: return unsign ? GL_UNSIGNED_BYTE  : GL_BYTE;
        case 2: return unsign ? GL_UNSIGNED_SHORT : GL_SHORT;
        case 4: return unsign ? GL_UNSIGNED_INT   : GL_INT;
        default:
          throw std::runtime_error ("Invalid integral type size");
      }
    }
    else
    {
      if (unsign)
        throw std::runtime_error ("Floating-point types may not be unsigned");

      switch (size)
      {
        case 2: return GL_HALF_FLOAT;
        case 4: return GL_FLOAT;
        default:
          throw std::runtime_error ("Invalid floating-point type size");
      }
    }
  }

  Model load_rkmodel (const InitContext& init, Rk::cstring_ref path)
  {
    auto buffer = load_file (init.data_rel (path));
    auto file = buffer.data ();

    auto header = (const RkModel::Header*) (file + 0);
    if (Rk::cstring_ref (header->signature, 8) != "RKMODEL2")
      throw std::runtime_error ("Incorrect signature");
    if (header->version != ((0x2014 << 16) | (0x07 << 8) | 0x28))
      throw std::runtime_error ("Unsupported version");

    auto geoms_base   = file         +                        sizeof (RkModel::Header);
    auto attribs_base = geoms_base   + header->geom_count   * sizeof (RkModel::Geom);
    auto meshes_base  = attribs_base + header->attrib_count * sizeof (RkModel::Attrib);
    auto data_base    = meshes_base  + header->mesh_count   * sizeof (RkModel::Mesh);

    auto data_ptr = data_base;

    auto cur_attrib = (const RkModel::Attrib*) attribs_base;

    std::vector <Geom> geoms;
    geoms.reserve (header->geom_count);

    std::vector <Attrib> attribs_buf;

    for (auto ptr = geoms_base; ptr != attribs_base; ptr += sizeof (RkModel::Geom))
    {
      const auto& gh = *(const RkModel::Geom*) ptr;

      auto verts_size = gh.vertex_size * gh.vertex_count;
      Buffer verts (verts_size, data_ptr);
      data_ptr = Rk::align (data_ptr + verts_size, 8);

      auto idxs_size = gh.index_size * gh.index_count;
      Buffer idxs (idxs_size, data_ptr);
      data_ptr = Rk::align (data_ptr + idxs_size, 8);

      attribs_buf.resize (gh.attrib_count);
      uint attrib_offset = 0;

      for (auto& attrib_out : attribs_buf)
      {
        auto unsign = cur_attrib->flags & RkModel::attrib_unsigned;
        auto integ  = cur_attrib->flags & RkModel::attrib_integer;
        auto normed = cur_attrib->flags & RkModel::attrib_normalize;

        if (integ && normed)
            throw std::runtime_error ("Invalid attribute flags");

        auto comp_type = to_gltype (cur_attrib->component_size, integ || normed, unsign);

        attrib_out.index  = cur_attrib->location;
        attrib_out.buffer = verts.name ();
        attrib_out.n      = cur_attrib->component_count;
        attrib_out.type   = comp_type;
        attrib_out.stride = gh.attrib_stride;
        attrib_out.offset = attrib_offset;

        attrib_offset += cur_attrib->component_count * cur_attrib->component_size;
        cur_attrib++;
      }

      Geom geom (attribs_buf.data (), gh.attrib_count, idxs.name (), gh.index_count, gh.index_size);
      verts.release ();
      idxs.release ();

      geoms.push_back (std::move (geom));
    }

    std::vector <Mesh> meshes;
    meshes.reserve (header->mesh_count);

    for (auto ptr = meshes_base; ptr != data_base; ptr += sizeof (RkModel::Mesh))
    {
      const auto& ms = *(const RkModel::Mesh*) ptr;

      Mesh mesh_out;

      static const uint gl_prim_types [] = {
        GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN
      };

      if (ms.prim_type > 5)
        throw std::runtime_error ("Invalid primitive type");

      mesh_out.prim_type    = gl_prim_types [ms.prim_type];
      mesh_out.vertex_base  = ms.vertex_base;
      mesh_out.vertex_max   = ms.vertex_max;
      mesh_out.index_offset = ms.index_first * geoms [ms.geom].index_size ();
      mesh_out.index_count  = ms.index_count;
      mesh_out.material     = ms.material;

      meshes.push_back (mesh_out);
    }

    //Model model (
  }

}
