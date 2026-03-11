#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#undef GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <print>

#include "csv_editor.hpp"
#include "debug.hpp"
#include "theme.h"
#include "thirdparty/dock.hpp"

extern "C" {
extern const uint8_t _binary_Roboto_Regular_ttf_start[];
extern const uint8_t _binary_Roboto_Regular_ttf_end[];
}

state_t state;
bool resized = false;
bool show_debug = false;

static void glfw_error_cb(int error, const char* description) {
    std::println("GLFW Error {}: {}", error, description);
}

static void glfw_resize_cb(GLFWwindow* window, int width, int height) {
    //resized = true;
}

int main(int argc, char* argv[]) {
    glfwSetErrorCallback(glfw_error_cb);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 330 core";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow(
        (int)(1280 * main_scale), (int)(800 * main_scale), "WT custom", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, glfw_resize_cb);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigDpiScaleFonts = true;

    // ImGui::StyleColorsDark();
    green_theme();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImFontConfig def_cfg;
    def_cfg.SizePixels = 16;
    def_cfg.FontDataOwnedByAtlas = false;
    def_cfg.OversampleH = 2;
    def_cfg.OversampleV = 1;
    // io.Fonts->AddFontFromMemoryTTF((void*)_binary_NotoSans_Regular_ttf_start,
    //     _binary_NotoSans_Regular_ttf_end - _binary_NotoSans_Regular_ttf_start,
    //     18,
    //     &def_cfg);

    io.Fonts->AddFontFromMemoryTTF((void*)_binary_Roboto_Regular_ttf_start,
        _binary_Roboto_Regular_ttf_end - _binary_Roboto_Regular_ttf_start,
        16,
        &def_cfg);

    def_cfg.MergeMode = true;

    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 0.f);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGuiID main_id = ImGui::GetID("Main DockSpace");

        if (ImGui::DockBuilderGetNode(main_id) == nullptr || resized) {
            DockBuilder(main_id)
                .Window("Toolbar##csv")
                .DockRight(0.75f)
                .Window("CSV Table##csv")
                .DockBottom(0.1)
                .Window("Status##csv")
                .Done();

            resized = false;
        }

        ImGui::DockSpaceOverViewport(main_id);

        auto pos = ImGui::GetCursorPos();

        csv_editor_draw(state, io.DeltaTime);

        if (ImGui::IsKeyPressed(ImGuiKey_F3)) { show_debug = !show_debug; }
        if (show_debug) { debug_overlay(pos); }

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w,
            clear_color.y * clear_color.w,
            clear_color.z * clear_color.w,
            clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}