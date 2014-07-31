//
// Adventure
//

#include "Geom.hpp"

#include "GLError.hpp"

namespace Ad
{
  Geom::Geom (const Attrib* attribs, size_t count, uint idx_buf, uint new_idx_count, uint new_idx_size) :
    idx_size  (new_idx_size),
    idx_count (new_idx_count)
  {
    glGenVertexArrays (1, &vao_name);
    glBindVertexArray (vao_name);

    for (auto* attr = attribs; attr != attribs + count; attr++)
    {
      glBindBuffer (GL_ARRAY_BUFFER, attr -> buffer);
      glVertexAttribPointer (
        attr -> index,
        attr -> n,
        attr -> type,
        false,
        attr -> stride,
        (const void*) attr -> offset);
      glEnableVertexAttribArray (attr -> index);
    }

    if (idx_buf)
      glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, idx_buf);

    glBindVertexArray (0);
  }

  void Geom::clear ()
  {
    if (vao_name != 0)
      glDeleteVertexArrays (1, &vao_name);
  }

  uint Geom::index_type () const
  {
    switch (idx_size)
    {
      case 1: return GL_UNSIGNED_BYTE;
      case 2: return GL_UNSIGNED_SHORT;
      case 4: return GL_UNSIGNED_INT;
    }
  }

}
