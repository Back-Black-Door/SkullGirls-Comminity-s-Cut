#include "hello_world.h"

void UI(ImGuiContext* context) {
    // using ImGui context
    ImGui::SetCurrentContext(context);

	// create a simple window
    if (ImGui::Begin("Hello World Mod")) {
        ImGui::Text("Hello from my first mod!");
        ImGui::Text("This is working!");

		// sample button and counter
        static int counter = 0;
        if (ImGui::Button("Click me!")) {
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("Counter = %d", counter);
    }
    ImGui::End();
}