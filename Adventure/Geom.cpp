//
// Adventure
//

#include "Geom.hpp"

#include "GLError.hpp"

namespace Ad
{
  Geom::Geom (const Attrib* attribs, uint count, uint index_buffer)
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

    if (index_buffer)
      glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  }

  Geom::~Geom ()
  {
    glDeleteVertexArrays (1, &vao_name);
  }

}
