//
// Adventure
//

#include <Rk/string_ref.hpp>

#include <memory>

namespace Ad
{
  class Shader
  {
    uint glname;

  public:
    Shader (Rk::cstring_ref path, uint type);
    ~Shader ();

    uint name () const
    {
      return glname;
    }

  };

  class ShaderProgram
  {
    uint glname;

  public:
    ShaderProgram ();
    ~ShaderProgram ();

    void add_shader (const Shader& shader);

    void        use ();
    static void done ();

    void fix_attrib   (const char* name, u32 location);
    void link         ();
    u32  link_uniform (const char* name);

  };

}
