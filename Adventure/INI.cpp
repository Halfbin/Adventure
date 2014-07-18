//
// Adventure
//

#include "INI.hpp"

#include <Rk/file_stream.hpp>
#include <Rk/utf8.hpp>

namespace Ad
{
  namespace
  {
    bool is_hspace (char32 cp)
    {
      return (cp == 0x20 || // SPACE
              cp == 0xa0 || // NO-BREAK SPACE
              cp == 0x1680 || // OGHAM SPACE MARK
              (cp >= 0x2000 && cp <= 0x200a) || // (GENERAL PUNCTUATION Zs)
              cp == 0x202f || // NARROW NO-BREAK SPACE
              cp == 0x205f || // MEDIUM MATHEMATICAL SPACE
              cp == 0x3000); // IDEOGRAPHIC SPACE
    }
    
    // Treats CRLF as two line-breaks, but INI doesn't care anyway.
    uint is_linebreak (char32 cp)
    {
      return (cp == 0x000a || // LF
              cp == 0x000b || // VT
              cp == 0x000c || // FF
              cp == 0x000d || // CR
              cp == 0x0085 || // NEL
              cp == 0x2028 || // LS
              cp == 0x2029);  // PS
    }

  }

  struct INILoader::Impl
  {
    std::vector <char> buffer;
    const char* cur;
    const char* end;
    Rk::cstring_ref section, key, value;

    bool skip_hspace (Rk::utf8_decoder&);

  };

  INILoader::INILoader (Rk::cstring_ref path) :
    impl (new Impl)
  {
    // Load INI
    Rk::fio::in_stream file (path);
    impl->buffer.resize (file.size ());
    file.read (impl->buffer.data (), impl->buffer.size ());

    impl->cur = impl->buffer.data ();
    impl->end = impl->cur + impl->buffer.size ();
  }

  bool INILoader::Impl::skip_hspace (Rk::utf8_decoder& dec)
  {
    for (;;)
    {
      // Leave cur pointing to next non-whitespace codepoint
      cur = dec.get_pointer ();
      auto stat = dec.decode ();
      if (stat == Rk::got_codepoint)
      {
        if (!is_hspace (dec.codepoint ()) && !is_linebreak (dec.codepoint ()))
          return true;
      }
      else if (stat == Rk::idle)
      {
        return false;
      }
      else
      {
        throw std::runtime_error ("Malformed INI");
      }
    }
  }

  INIStatus INILoader::proceed ()
  {
    Rk::utf8_decoder dec (impl->cur, impl->end);

    // Skip space
    auto more = impl->skip_hspace (dec);
    if (!more)
    {
      if (impl->section)
      {
        impl->section = { };
        return INIStatus::done_section;
      }

      return INIStatus::done;
    }

    // Section header?
    if (dec.codepoint () == '[')
    {
      if (impl->section)
      {
        impl->section = { };
        return INIStatus::done_section;
      }

      const char* name_begin = dec.get_pointer ();

      for (;;)
      {
        impl->cur = dec.get_pointer ();
        auto stat = dec.decode ();
        if (stat == Rk::got_codepoint)
        {
          if (dec.codepoint () == '[' || is_linebreak (dec.codepoint ()))
            throw std::runtime_error ("Invalid syntax in INI section header");
          else if (dec.codepoint () == ']')
            break;
        }
        else
        {
          throw std::runtime_error ("Malformed INI");
        }
      }

      // Pull out name and step past terminating ']'
      impl->section = Rk::cstring_ref (name_begin, impl->cur);
      impl->cur = dec.get_pointer ();

      // Blank k/v for added safety
      impl->key   = { };
      impl->value = { };

      return INIStatus::got_section;
    }

    // Expect a KV pair. First grab the key.
    const char* key_begin = impl->cur;

    for (;;)
    {
      impl->cur = dec.get_pointer ();
      auto stat = dec.decode ();
      if (stat == Rk::got_codepoint)
      {
        if (is_linebreak (dec.codepoint ()))
          throw std::runtime_error ("Invalid syntax in INI key");
        else if (is_hspace (dec.codepoint ()))
          break;
      }
      else
      {
        throw std::runtime_error ("Malformed INI");
      }
    }

    impl->key = Rk::cstring_ref (key_begin, impl->cur);

    // whitespace between key and '='
    more = impl->skip_hspace (dec);
    if (!more || dec.codepoint () != '=')
      throw std::runtime_error ("Malformed INI");

    // whitespace between '=' and value
    more = impl->skip_hspace (dec);
    if (!more)
      throw std::runtime_error ("Malformed INI");

    // Grab value
    const char* value_begin = impl->cur;
    const char* trailing_space = nullptr;

    for (;;)
    {
      impl->cur = dec.get_pointer ();
      auto stat = dec.decode ();
      if (stat == Rk::got_codepoint)
      {
        if (is_linebreak (dec.codepoint ()))
          break;
        else if (is_hspace (dec.codepoint ()) && !trailing_space)
          trailing_space = impl->cur;
        else
          trailing_space = nullptr;
      }
      else
      {
        throw std::runtime_error ("Malformed INI");
      }
    }

    if (trailing_space)
      impl->value = Rk::cstring_ref (value_begin, trailing_space);
    else
      impl->value = Rk::cstring_ref (value_begin, impl->cur);

    impl->cur = dec.get_pointer ();
    return INIStatus::got_pair;
  }

  Rk::cstring_ref INILoader::section () const
  {
    return impl->section;
  }

  Rk::cstring_ref INILoader::key () const
  {
    return impl->key;
  }

  Rk::cstring_ref INILoader::value () const
  {
    return impl->value;
  }

}
