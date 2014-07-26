//
// Adventure
//

#include <Rk/string_ref.hpp>

#include <algorithm>

namespace Ad
{
  class UTFNode
  {
    const UTFNode* parent;
    const u8*      ns;
    const u8*      ss;
    const u8*      ds;

    Rk::cstring_ref node_name;
    const u8*       next_ptr;
    const u8*       child_or_data;
    u32             data_len;

    friend UTFNode load_utf (const u8* file, const size_t length);
    
    bool has_next () const
    {
      return next_ptr != 0;
    }
    
    bool has_child () const
    {
      return data_len == 0;
    }

    UTFNode (const u8* node, const UTFNode* par, const u8* ns, const u8* ss, const u8* ds);

  public:
    UTFNode () = default;

    UTFNode (const UTFNode& other) = default;
    UTFNode& operator = (const UTFNode& other) = default;

    Rk::cstring_ref name () const
    {
      return node_name;
    }

    UTFNode next  () const;
    UTFNode child () const;

    u32 data_length () const
    {
      return data_len;
    }

    const u8* data () const;

    explicit operator bool () const
    {
      return !node_name.empty ();
    }

  };

  UTFNode load_utf (const u8* file, const size_t length);

}
