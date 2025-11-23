#include "hello_world.h"
#include <iostream>

void UI(ImGuiContext* context) {
    // Set the ImGui context from the modloader
    ImGui::SetCurrentContext(context);

	// Create custom tab for the mod content
    if (ImGui::BeginTabItem("Hello World tab")) {
        ImGui::Text("Hello from Hello World mod!");

        // Button with counter and logging
        static int counter = 0;
        if (ImGui::Button("Click me!")) {
            counter++;
            std::cout << "[HelloWorld Mod] Button clicked! Counter: " << counter << std::endl;
        }
        ImGui::SameLine();
        ImGui::Text("Counter = %d", counter);

        ImGui::EndTabItem();
    }
}