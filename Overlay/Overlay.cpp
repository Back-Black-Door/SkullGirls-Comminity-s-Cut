#include <Windows.h>
#include <d3d9.h>
#include <math.h>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"


#include "../Console.h"
#include "Overlay.h"
#include "d3d9Wrapper.h"
#include "../modslualib.h"

namespace imgui_show {
    bool Show_Window = false;
    bool Show_Debug = false;
    bool Show_imgui_demo = false;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Overlay { 
    LRESULT CALLBACK Input(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
        {
            // Всегда передаем сообщения в ImGui для обработки
            ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

            // Получаем состояние ImGui
            ImGuiIO& io = ImGui::GetIO();
            // Обрабатываем нажатие Insert 
            if (uMsg == WM_KEYDOWN && wParam == VK_INSERT)
            {
                imgui_show::Show_Window = !imgui_show::Show_Window;
                return 0;
            }

            // Блокируем сообщения мыши, если ImGui хочет их обработать
            if (((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) && io.WantCaptureMouse) && imgui_show::Show_Window)
            {
                return true;
            }

            // Блокируем сообщения клавиатуры, если ImGui хочет их обработать
            if (((uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST) && io.WantCaptureKeyboard) && imgui_show::Show_Window)
            {
                return true;
            }

            return CallWindowProc(OriginalWndProc, hWnd, uMsg, wParam, lParam);
        }
    void Render() {
        if (imgui_show::Show_Window) {
            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();

            ImGui::NewFrame();
            //ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImVec2 min_size = ImVec2(100, 100);
            ImVec2 max_size = ImVec2(800, 600);
            ImGui::SetNextWindowSizeConstraints(min_size, max_size);
            // Установка начальной позиции и размера
            ImGui::SetNextWindowPos(ImVec2(100, 100), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
            // Окно управления камерой
            ImGui::Begin("Skullgirls Community's Cut", &imgui_show::Show_Window);
            ImGui::BeginTabBar("Nav_Bar");
            if (ImGui::BeginTabItem("Main")) {
                ImGui::Text("Welcome to Mod Loader !");
                ImGui::Text("To turn the overlay on/off, press INSERT");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Mods")) {
                static int selected = 0;
                if (mods.size() != 0) {
                    {
                        ImGui::BeginChild("left pane", ImVec2(150, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
                        for (size_t i = 0; i < mods.size(); ++i)
                        {
                            char label[128];
                            sprintf(label, mods[i]->ModInfo.modName.c_str(), i);
                            if (ImGui::Selectable(label, selected == i, ImGuiSelectableFlags_SelectOnNav))
                                selected = i;
                        }
                        ImGui::EndChild();
                    }
                    ImGui::SameLine();

                    // Right
                    {
                        ImGui::BeginGroup();
                        ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetFrameHeightWithSpacing())); // Leave room for 1 line below us
                        ImGui::Text(mods[selected]->ModInfo.modName.c_str(), selected);
                        ImGui::Separator();
                        if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
                        {
                            if (ImGui::BeginTabItem("Description"))
                            {
                                ImGui::TextWrapped(mods[selected]->ModInfo.modDesc.c_str());
                                ImGui::EndTabItem();
                            }
                            if (ImGui::BeginTabItem("Details"))
                            {
                                std::string IM_ModName = "Mod Name: " + mods[selected]->ModInfo.modName;
                                std::string IM_ModAuthor = "Mod Author: " + mods[selected]->ModInfo.modAuthor;
                                std::string IM_ModVersion = "Mod Version: " + std::to_string(mods[selected]->ModInfo.modVersion);
                                std::string IM_ModPath = "Mod Path: " + mods[selected]->ModInfo.modPath.string();
                                ImGui::Text(IM_ModName.c_str());
                                ImGui::Text(IM_ModAuthor.c_str());
                                ImGui::Text(IM_ModVersion.c_str());
                                ImGui::Text(IM_ModPath.c_str());
                                ImGui::EndTabItem();
                            }
                            ImGui::EndTabBar();
                        }
                        ImGui::EndChild();
                        ImGui::EndGroup();
                    }
                }
                else {
                    ImGui::Text("We don't have any mods!");
                }
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Settings")) {
                ImGui::Text("Test!");
                //ImGui::Checkbox("Debug On", &imgui_show::Show_Debug);
                ImGui::Checkbox("Console Show", &Console::Show_Console);
                ImGui::EndTabItem();
            }
            if (imgui_show::Show_Debug) {
                if (ImGui::BeginTabItem("Debug")) {
                    ImGui::Text("Test!");
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();

            // Получаем текущую позицию и размер окна
            ImVec2 current_pos = ImGui::GetWindowPos();
            ImVec2 current_size = ImGui::GetWindowSize();

            // Задаём границы (например, границы окна приложения или родительской области)
            ImVec2 min_bound(0, 0);
            ImVec2 max_bound = ImGui::GetMainViewport()->Size;

            // Корректируем позицию, чтобы окно не выходило за границы
            ImVec2 new_pos = current_pos;
            new_pos.x = ImClamp(new_pos.x, min_bound.x, max_bound.x - current_size.x);
            new_pos.y = ImClamp(new_pos.y, min_bound.y, max_bound.y - current_size.y);

            // Если позиция изменилась, применяем новую позицию
            if (new_pos.x != current_pos.x || new_pos.y != current_pos.y) {
                ImGui::SetWindowPos(new_pos);
            }
            if (Console::Show_Console) { Console::ShowConsole(); }
            if (!Console::Show_Console) { Console::HideConsole(); }
            ImGui::End();
            ImGui::EndFrame();
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        }
    }
}

