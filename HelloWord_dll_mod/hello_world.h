#pragma once

// Compile ImGui directly in this mod
#include "imgui/imgui.h"

extern "C" {
    __declspec(dllexport) void UI(ImGuiContext* context);
}