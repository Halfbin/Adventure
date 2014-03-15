//
// Adventure
//

#include <Rk/unicode_convert.hpp>

#include "MinWin.hpp"

namespace Ad
{
  void true_main ();

  extern "C" int WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, UINT) try
  {
    true_main ();
    return 0;
  }
  catch (std::exception& e)
  {
    auto w_what = Rk::string_utf8_to_16 (e.what ());
    MessageBoxW (nullptr, (LPCWSTR) w_what.c_str (), L"Adventure", MB_ICONEXCLAMATION | MB_OK);
    return 1;
  }

}
