#pragma once
#include "imgui/imgui.h"

extern "C" {
    __declspec(dllexport) void UI(ImGuiContext* context);
}