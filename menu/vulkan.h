#ifndef FMENU_VULKAN_H
#define FMENU_VULKAN_H

#include <imgui.h>

namespace vulkan {
    extern auto init( const char * name, int win_w, int win_h ) -> bool;

    extern auto on_render( ImGuiIO & io ) -> void;
}

#endif
