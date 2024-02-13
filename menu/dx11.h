#ifndef FMENU_DX11_H
#define FMENU_DX11_H

#include "imgui.h"
#include <d3d11.h>
#include <tchar.h>

namespace dx11 {
  extern auto init(const wchar_t * name, int win_w, int win_h) -> bool;

  extern auto on_render(ImGuiIO &io, HWND window) -> void;

}

#endif
