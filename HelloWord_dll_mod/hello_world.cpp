#include "hello_world.h"
#include <cstdlib>

// Memory allocation functions that use the CRT from this DLL
static void* ImGuiAllocWrapper(size_t size, void* user_data) {
    (void)user_data;
    return malloc(size);
}

static void ImGuiFreeWrapper(void* ptr, void* user_data) {
    (void)user_data;
    free(ptr);
}

void UI(ImGuiContext* context) {
    // Step 1: Set context
    ImGui::SetCurrentContext(context);

    // Step 2: Set allocator functions
    ImGui::SetAllocatorFunctions(ImGuiAllocWrapper, ImGuiFreeWrapper, nullptr);

    // Step 3: Empty window works, but content causes crash
	if (ImGui::Begin("Hello World Mod")) {
        // TODO: Add content here
    }
    ImGui::End();
}