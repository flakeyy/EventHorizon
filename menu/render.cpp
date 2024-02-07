#include "dx11.h"
#include "fc2.hpp"

auto dx11::on_render(ImGuiIO &io, HWND window) -> void {
  if( fc2::get_error() != FC2_TEAM_ERROR_NO_ERROR)
  {
    puts("Universe4 is not running or is unresponsive.");
    std::exit(1);
  }

  static auto name = fc2::call< std::string >( "name", FC2_LUA_TYPE_STRING );

  ImGui::SetNextWindowSize( ImVec2( 256, 256 ) );
  if( ImGui::Begin("FMENU")) {
    ImGui::Text( "Hello %s", name.c_str());
  }
}