//
// Adventure
//

#include "FLModel.hpp"

#include <Rk/file_stream.hpp>
#include <Rk/vector.hpp>

#include <vector>
#include <map>

#include <GL/glew.h>

#include "CStringRefLess.hpp"
#include "ModelShader.hpp"
#include "Buffer.hpp"
#include "FLUTF.hpp"
#include "Model.hpp"

namespace Ad
{
  struct FLCRC32
  {
    u32 state;

  public:
    FLCRC32 () :
      state (0xffffffff)
    { }

    FLCRC32 (const void* data, size_t length) :
      FLCRC32 ()
    {
      append (data, length);
    }

    void append (const void* raw, size_t length)
    {
      static const u32 table [256] = {
        0x00000000, 0x09073096, 0x120E612C, 0x1B0951BA, 
        0xFF6DC419, 0xF66AF48F, 0xED63A535, 0xE46495A3, 
        0xFEDB8832, 0xF7DCB8A4, 0xECD5E91E, 0xE5D2D988, 
        0x01B64C2B, 0x08B17CBD, 0x13B82D07, 0x1ABF1D91, 
        0xFDB71064, 0xF4B020F2, 0xEFB97148, 0xE6BE41DE, 
        0x02DAD47D, 0x0BDDE4EB, 0x10D4B551, 0x19D385C7, 
        0x036C9856, 0x0A6BA8C0, 0x1162F97A, 0x1865C9EC, 
        0xFC015C4F, 0xF5066CD9, 0xEE0F3D63, 0xE7080DF5, 
        0xFB6E20C8, 0xF269105E, 0xE96041E4, 0xE0677172, 
        0x0403E4D1, 0x0D04D447, 0x160D85FD, 0x1F0AB56B, 
        0x05B5A8FA, 0x0CB2986C, 0x17BBC9D6, 0x1EBCF940, 
        0xFAD86CE3, 0xF3DF5C75, 0xE8D60DCF, 0xE1D13D59, 
        0x06D930AC, 0x0FDE003A, 0x14D75180, 0x1DD06116, 
        0xF9B4F4B5, 0xF0B3C423, 0xEBBA9599, 0xE2BDA50F, 
        0xF802B89E, 0xF1058808, 0xEA0CD9B2, 0xE30BE924, 
        0x076F7C87, 0x0E684C11, 0x15611DAB, 0x1C662D3D, 
        0xF6DC4190, 0xFFDB7106, 0xE4D220BC, 0xEDD5102A, 
        0x09B18589, 0x00B6B51F, 0x1BBFE4A5, 0x12B8D433, 
        0x0807C9A2, 0x0100F934, 0x1A09A88E, 0x130E9818, 
        0xF76A0DBB, 0xFE6D3D2D, 0xE5646C97, 0xEC635C01, 
        0x0B6B51F4, 0x026C6162, 0x196530D8, 0x1062004E, 
        0xF40695ED, 0xFD01A57B, 0xE608F4C1, 0xEF0FC457, 
        0xF5B0D9C6, 0xFCB7E950, 0xE7BEB8EA, 0xEEB9887C, 
        0x0ADD1DDF, 0x03DA2D49, 0x18D37CF3, 0x11D44C65, 
        0x0DB26158, 0x04B551CE, 0x1FBC0074, 0x16BB30E2, 
        0xF2DFA541, 0xFBD895D7, 0xE0D1C46D, 0xE9D6F4FB, 
        0xF369E96A, 0xFA6ED9FC, 0xE1678846, 0xE860B8D0, 
        0x0C042D73, 0x05031DE5, 0x1E0A4C5F, 0x170D7CC9, 
        0xF005713C, 0xF90241AA, 0xE20B1010, 0xEB0C2086, 
        0x0F68B525, 0x066F85B3, 0x1D66D409, 0x1461E49F, 
        0x0EDEF90E, 0x07D9C998, 0x1CD09822, 0x15D7A8B4, 
        0xF1B33D17, 0xF8B40D81, 0xE3BD5C3B, 0xEABA6CAD, 
        0xEDB88320, 0xE4BFB3B6, 0xFFB6E20C, 0xF6B1D29A, 
        0x12D54739, 0x1BD277AF, 0x00DB2615, 0x09DC1683, 
        0x13630B12, 0x1A643B84, 0x016D6A3E, 0x086A5AA8, 
        0xEC0ECF0B, 0xE509FF9D, 0xFE00AE27, 0xF7079EB1, 
        0x100F9344, 0x1908A3D2, 0x0201F268, 0x0B06C2FE, 
        0xEF62575D, 0xE66567CB, 0xFD6C3671, 0xF46B06E7, 
        0xEED41B76, 0xE7D32BE0, 0xFCDA7A5A, 0xF5DD4ACC, 
        0x11B9DF6F, 0x18BEEFF9, 0x03B7BE43, 0x0AB08ED5, 
        0x16D6A3E8, 0x1FD1937E, 0x04D8C2C4, 0x0DDFF252, 
        0xE9BB67F1, 0xE0BC5767, 0xFBB506DD, 0xF2B2364B, 
        0xE80D2BDA, 0xE10A1B4C, 0xFA034AF6, 0xF3047A60, 
        0x1760EFC3, 0x1E67DF55, 0x056E8EEF, 0x0C69BE79, 
        0xEB61B38C, 0xE266831A, 0xF96FD2A0, 0xF068E236, 
        0x140C7795, 0x1D0B4703, 0x060216B9, 0x0F05262F, 
        0x15BA3BBE, 0x1CBD0B28, 0x07B45A92, 0x0EB36A04, 
        0xEAD7FFA7, 0xE3D0CF31, 0xF8D99E8B, 0xF1DEAE1D, 
        0x1B64C2B0, 0x1263F226, 0x096AA39C, 0x006D930A, 
        0xE40906A9, 0xED0E363F, 0xF6076785, 0xFF005713, 
        0xE5BF4A82, 0xECB87A14, 0xF7B12BAE, 0xFEB61B38, 
        0x1AD28E9B, 0x13D5BE0D, 0x08DCEFB7, 0x01DBDF21, 
        0xE6D3D2D4, 0xEFD4E242, 0xF4DDB3F8, 0xFDDA836E, 
        0x19BE16CD, 0x10B9265B, 0x0BB077E1, 0x02B74777, 
        0x18085AE6, 0x110F6A70, 0x0A063BCA, 0x03010B5C, 
        0xE7659EFF, 0xEE62AE69, 0xF56BFFD3, 0xFC6CCF45, 
        0xE00AE278, 0xE90DD2EE, 0xF2048354, 0xFB03B3C2, 
        0x1F672661, 0x166016F7, 0x0D69474D, 0x046E77DB, 
        0x1ED16A4A, 0x17D65ADC, 0x0CDF0B66, 0x05D83BF0, 
        0xE1BCAE53, 0xE8BB9EC5, 0xF3B2CF7F, 0xFAB5FFE9, 
        0x1DBDF21C, 0x14BAC28A, 0x0FB39330, 0x06B4A3A6, 
        0xE2D03605, 0xEBD70693, 0xF0DE5729, 0xF9D967BF, 
        0xE3667A2E, 0xEA614AB8, 0xF1681B02, 0xF86F2B94, 
        0x1C0BBE37, 0x150C8EA1, 0x0E05DF1B, 0x0702EF8D
      };

      auto data = (const u8*) raw;

      while (length > 0)
      {
        auto byte = tolower (*data++);
        state = table [(state ^ byte) & 0xff] ^ (state >> 8);
        length--;
      }
    }

    u32 value () const
    {
      return ~state;
    }

  };

  u32 flcrc32 (Rk::cstring_ref str)
  {
    FLCRC32 crc (str.data (), str.length ());
    return crc.value ();
  }

  class CMP
  {
    struct VMeshProxy
    {
      u32       id,
                length;
      const u8* data;
    };

    std::vector <VMeshProxy> vmeshes;

    struct Object
    {
      u32             index;
      Rk::cstring_ref file_name,
                      obj_name;
    };

    std::vector <Object> objs;

    struct Ref
    {
      u32 id;
      u16 start_vertex,
          num_vertices,
          start_index,
          num_indices,
          start_mesh,
          num_meshes;
    };

    std::vector <Ref> refs;

    struct Submodel
    {
      Rk::cstring_ref name;
      uint            lod0_ref,
                      lod_count;
    };

    std::map <Rk::cstring_ref, Submodel, CStringRefLess> submodels;

  public:
    void load_mesh_lib (UTFNode& lib)
    {
      for (auto lod = lib.child (); lod; lod = lod.next ())
      {
        auto vmdata = lod.child ();
        auto data = vmdata.data ();
        auto id = flcrc32 (lod.name ());
        VMeshProxy proxy { id, vmdata.data_length (), data };
        vmeshes.push_back (proxy);
      }
    }

    void load_cmpnd (UTFNode& cmpnd)
    {
      for (auto obj = cmpnd.child (); obj; obj = obj.next ())
      {
        Rk::cstring_ref file_name, obj_name;
        u32 index = 0xffffffff;

        for (auto key = obj.child (); key; key = key.next ())
        {
          if (key.name () == "File name")
            file_name = Rk::cstring_ref ((const char*) key.data ());
          if (key.name () == "Object name")
            obj_name = Rk::cstring_ref ((const char*) key.data ());
          else if (key.name () == "Index")
            index = *(const u32*) key.data ();
        }

        // Not actually an object?
        if (!file_name || !obj_name || index == 0xffffffff)
          continue;

        objs.push_back (Object { index, file_name, obj_name });
      }
    }

    void load_submodel (UTFNode& node)
    {
      auto multilevel = node.child ();
      while (multilevel.name () != "MultiLevel")
        multilevel = multilevel.next ();

      Submodel sm { node.name (), (uint) refs.size () };

      for (auto level = multilevel.child (); level; level = level.next ())
      {
        if (level.name ().slice (0, 5) != "Level")
          continue;

        auto part = level.child ();
        auto ref = part.child ();

        if (ref.data_length () != 60)
          throw std::runtime_error ("uh oh");

        auto data = ref.data ();
        auto id = *(const u32*) (data +  4);
        auto sv = *(const u16*) (data +  8);
        auto nv = *(const u16*) (data + 10);
        auto sr = *(const u16*) (data + 12);
        auto nr = *(const u16*) (data + 14);
        auto sm = *(const u16*) (data + 16);
        auto nm = *(const u16*) (data + 18);

        refs.push_back (Ref { id, sv, nv, sr, nr, sm, nm });
      }

      sm.lod_count = uint (refs.size () - sm.lod0_ref);
      submodels [sm.name] = sm;
    }

    Model compile_root_lod0 ()
    {
      // Assume Root is index 0
      Rk::cstring_ref root_filename;
      for (const auto& obj : objs)
      {
        if (obj.index == 0)
        {
          root_filename = obj.file_name;
          break;
        }
      }

      // Find corresponding submodel
      auto iter = submodels.find (root_filename);
      if (iter == submodels.end ())
        throw std::runtime_error ("Can't find root submodel");

      const auto& sm = iter->second;

      // Build geom based on ref data
      const auto& ref = refs.at (sm.lod0_ref);

      // Find relevant vmeshdata
      const u8* vmdata = nullptr;
      for (const auto& proxy : vmeshes)
      {
        if (proxy.id == ref.id)
          vmdata = proxy.data;
      }

      if (!vmdata)
        throw std::runtime_error ("Can't find vmeshdata");

      // Pull counts and fvf out of header
      auto mesh_count   = *(const u16*) (vmdata +  8);
      auto index_count  = *(const u16*) (vmdata + 10);
      auto fvf          = *(const u16*) (vmdata + 12);
      auto vertex_count = *(const u16*) (vmdata + 14);

      if (fvf != 0x112) // XYZIJKST
        throw std::runtime_error ("fvf unsupported");

      std::vector <Mesh> out_meshes;

      auto meshes_base = vmdata + 16;
      auto first_mesh = meshes_base + 12 * ref.start_mesh;
      
      // Find where our vertex/index data begin by going through unwanted leading meshes
      uint base_vertex = 0,
           base_index = 0;

      for (auto lead_mesh = meshes_base; lead_mesh != first_mesh; lead_mesh += 12)
      {
        auto first_vertex = *(const u16*) (lead_mesh + 4);
        auto last_vertex  = *(const u16*) (lead_mesh + 6);
        auto num_indices  = *(const u16*) (lead_mesh + 8);

        auto num_vertices = last_vertex - first_vertex + 1;

        base_vertex += num_vertices;
        base_index  += num_indices;
      }

      // Read our meshes and add up vertex/index counts
      uint relevant_vertices = 0,
           relevant_indices = 0;
      
      for (auto mesh = first_mesh; mesh != first_mesh + 12 * ref.num_meshes; mesh += 12)
      {
        auto first_vertex = *(const u16*) (mesh + 4);
        auto last_vertex  = *(const u16*) (mesh + 6);
        auto num_indices  = *(const u16*) (mesh + 8);

        auto num_vertices = last_vertex - first_vertex + 1;

        out_meshes.emplace_back (
          0, // assign later
          GL_UNSIGNED_SHORT,
          GL_TRIANGLES,
          relevant_vertices,
          relevant_vertices + num_vertices,
          relevant_indices * 2,
          num_indices,
          0
        );

        relevant_vertices += num_vertices;
        relevant_indices  += num_indices;
      }

      // Load indices
      auto indices_base = meshes_base  + 12 * mesh_count;
      auto first_index  = indices_base +  2 * base_index;

      Buffer indices (relevant_indices * 2, first_index);

      // Load vertices
      auto vertex_size = 32;
      auto pos_off = 0;
      auto nrm_off = 12;
      auto tex_off = 24;

      auto vertices_base = indices_base  +           2 * index_count;
      auto first_vertex  = vertices_base + vertex_size * base_vertex;

      struct Vert { v3f p, n; v2f t; };

      std::vector <Vert> trans_vertices (relevant_vertices);
      
      for (int i = 0; i != relevant_vertices; i++)
      {
        auto p = *(const v3f*) (first_vertex + i * 32 +  0);
        auto n = *(const v3f*) (first_vertex + i * 32 + 12);
        auto t = *(const v2f*) (first_vertex + i * 32 + 24);
        trans_vertices [i] = { v3f {-p.z, -p.x, p.y}, v3f {-n.z, -n.x, n.y}, t };
      }

      Buffer vertices (relevant_vertices * vertex_size, trans_vertices.data ());

      // Build geom
      Attrib attribs [] = {
        { ModelShader::attrib_vertpos, vertices.name (), 3, GL_FLOAT, vertex_size, pos_off },
        { ModelShader::attrib_normal,  vertices.name (), 3, GL_FLOAT, vertex_size, nrm_off },
        { ModelShader::attrib_tcoords, vertices.name (), 2, GL_FLOAT, vertex_size, tex_off }
      };

      std::vector <Geom> geoms;
      geoms.emplace_back (attribs, 3, indices.name (), relevant_indices, 2);

      vertices.release ();
      indices.release ();

      // Assign geom name to meshes
      for (auto& mesh : out_meshes)
        mesh.geom = geoms [0].name ();

      return Model (std::move (geoms), std::move (out_meshes));
    }

  };

  Model load_fl_cmp (const InitContext& init, Rk::cstring_ref path)
  {
    Rk::fio::in_stream file (init.data_rel (path));
    std::vector <u8> buffer (file.size ());
    file.read (buffer.data (), buffer.size ());

    auto root = load_utf (buffer.data (), buffer.size ());
    CMP cmp;

    for (auto node = root.child (); node; node = node.next ())
    {
      if (node.name () == "VMeshLibrary")
        cmp.load_mesh_lib (node);
      else if (node.name () == "Cmpnd")
        cmp.load_cmpnd (node);
      else if (node.name ().length () > 4 && std::equal (node.name ().end () - 4, node.name ().end (), ".3db"))
        cmp.load_submodel (node);
    }

    return cmp.compile_root_lod0 ();
  }

}
