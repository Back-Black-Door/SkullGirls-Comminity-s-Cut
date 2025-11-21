#include "pch.h"
#include "hello_world.h"

void UI(ImGuiContext* context) {
    // Устанавливаем контекст ImGui
    ImGui::SetCurrentContext(context);

    // Создаем простое окно
    if (ImGui::Begin("Hello World Mod")) {
        ImGui::Text("Hello from my first mod!");
        ImGui::Text("This is working!");

        // Простой пример с кнопкой
        static int counter = 0;
        if (ImGui::Button("Click me!")) {
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("Counter = %d", counter);
    }
    ImGui::End();
}