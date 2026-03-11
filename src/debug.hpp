#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <imgui.h>
#include <imgui_internal.h>

#include <cstdarg>
#include <cstdio>
#include <type_traits>
#include <vector>

#include "defines.h"
#include "thirdparty/memory_usage.h"

struct draw_overlay {
    ImDrawList* draw_list = nullptr;
    ImVec2 pos;
    float font_size = 0;
    ImU32 shadow = IM_COL32(10, 10, 20, 160);

    void text(ImU32 col, const char* str, float line_h) {
        if (!draw_list) return;
        draw_list->AddText(
            ImGui::GetFont(), font_size, ImVec2(pos.x + 1.2f, pos.y + 1.2f), shadow, str);
        draw_list->AddText(ImGui::GetFont(), font_size, pos, col, str);
        pos.y += line_h;
    }
};

struct measure_overlay {
    ImVec2 start_pos;
    ImVec2 pos;
    float max_x = 0;
    float max_y = 0;
    float font_size = 0;

    void text(ImU32 col, const char* str, float line_h) {
        ImVec2 size = ImGui::CalcTextSize(str);
        max_x = ImMax(max_x, pos.x + size.x);
        max_y = ImMax(max_y, pos.y + size.y);
        pos.y += line_h;
    }

    ImVec2 size() const {
        return ImVec2(max_x - start_pos.x + font_size, max_y - start_pos.y + font_size);
    }
};

template <typename Mode>
inline void debug_overlay(ImVec2 pos, Mode& p) {
    static std::vector<float> frame_times;
    static float time_sum = 0.0f;
    constexpr int MAX_SAMPLES = 120;

    float dt_s = ImGui::GetIO().DeltaTime;
    frame_times.push_back(dt_s);
    time_sum += dt_s;

    if (frame_times.size() > MAX_SAMPLES) {
        time_sum -= frame_times.front();
        frame_times.erase(frame_times.begin());
    }

    const float avg_s = time_sum / static_cast<float>(frame_times.size());
    const float avg_ms = avg_s * 1'000.0f;
    const float avg_fps = (avg_s > 0.000'000'1f) ? 1.0f / avg_s : 0.0f;

    const float font_size = ImGui::GetFontSize();
    const float line_h = font_size * 1.10f;
    p.font_size = font_size;

    p.pos = pos;
    if constexpr (std::is_same_v<Mode, measure_overlay>) { p.start_pos = pos; }

    ImDrawList* draw_list = ImGui::GetForegroundDrawList();
    if (!draw_list) return;

    if constexpr (std::is_same_v<Mode, draw_overlay>) { p.draw_list = draw_list; }

    ImU32 color = IM_COL32(245, 245, 245, 255);
    ImU32 shadow = IM_COL32(10, 10, 20, 160);

    ImU32 fps_color = (avg_fps >= 60.f)   ? IM_COL32(80, 255, 120, 255)
                      : (avg_fps >= 30.f) ? IM_COL32(255, 220, 80, 255)
                                          : IM_COL32(255, 70, 70, 255);

    char buf[96];

    auto print = [&](ImU32 col, const char* fmt, ...) IM_FMTARGS(3) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);

        p.text(col, buf, line_h);
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
