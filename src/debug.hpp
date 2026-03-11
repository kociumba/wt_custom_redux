#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <imgui.h>
#include "defines.h"
#include "thirdparty/memory_usage.h"

inline void debug_overlay(ImVec2 pos) {
    static std::vector<float> frame_times;
    static float time_sum = 0.0f;
    constexpr int MAX_SAMPLES = 120;

    float dt_ms = ImGui::GetIO().DeltaTime / 1000;
    frame_times.push_back(dt_ms);
    time_sum += dt_ms;

    if (frame_times.size() > MAX_SAMPLES) {
        time_sum -= frame_times.front();
        frame_times.erase(frame_times.begin());
    }

    const float avg_ms = time_sum / static_cast<float>(frame_times.size());
    const float avg_fps = (avg_ms > 0.0001f) ? 1000.0f / avg_ms : 0.0f;

    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    if (!draw_list) return;

    ImU32 color = IM_COL32(245, 245, 245, 255);
    ImU32 shadow = IM_COL32(10, 10, 20, 160);

    ImU32 fps_color = (avg_fps >= 60.f)   ? IM_COL32(80, 255, 120, 255)
                      : (avg_fps >= 30.f) ? IM_COL32(255, 220, 80, 255)
                                          : IM_COL32(255, 70, 70, 255);

    char buf[96];
    const float font_size = ImGui::GetFontSize();
    const float line_h = font_size * 1.10f;

    auto print = [&](ImU32 col, const char* fmt, ...) IM_FMTARGS(3) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);

        draw_list->AddText(
            ImGui::GetFont(), font_size, ImVec2(pos.x + 1.2f, pos.y + 1.2f), shadow, buf);

        draw_list->AddText(ImGui::GetFont(), font_size, pos, col, buf);

        pos.y += line_h;
    };

    print(fps_color, "FPS %.1f (%.2f ms/frame)", avg_fps, avg_ms);

    print(color, BUILD_IDENTIFIER);

    print(color, "%s", get_memory_usage_str_mb());

    ImGuiIO& io = ImGui::GetIO();
    bool mouse_valid = ImGui::IsMousePosValid(&io.MousePos);
    float mouse_x = mouse_valid ? io.MousePos.x : 0.0f;
    float mouse_y = mouse_valid ? io.MousePos.y : 0.0f;

    print(color, "Mouse %.0f, %.0f", mouse_x, mouse_y);
}

#endif /* DEBUG_HPP */
