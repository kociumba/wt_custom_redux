#ifndef THEME_H
#define THEME_H

#include <imgui.h>

inline void green_theme(ImGuiStyle* dst = nullptr) {
    ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.09f, 0.94f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.09f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.11f, 0.98f);

    colors[ImGuiCol_Border] = ImVec4(0.42f, 0.60f, 0.10f, 0.30f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.16f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.60f, 0.10f, 0.20f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.60f, 0.10f, 0.40f);

    colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.18f, 0.05f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.08f, 0.09f, 0.50f);

    colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.08f, 0.09f, 0.50f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.42f, 0.60f, 0.10f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.42f, 0.60f, 0.10f, 1.00f);

    colors[ImGuiCol_CheckMark] = ImVec4(0.42f, 0.60f, 0.10f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.42f, 0.60f, 0.10f, 0.80f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.70f, 0.15f, 1.00f);

    colors[ImGuiCol_Button] = ImVec4(0.15f, 0.16f, 0.18f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.42f, 0.60f, 0.10f, 0.25f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.60f, 0.10f, 0.60f);

    colors[ImGuiCol_Header] = ImVec4(0.42f, 0.60f, 0.10f, 0.30f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.42f, 0.60f, 0.10f, 0.50f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.42f, 0.60f, 0.10f, 0.70f);

    colors[ImGuiCol_Separator] = ImVec4(0.42f, 0.60f, 0.10f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.42f, 0.60f, 0.10f, 0.80f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.42f, 0.60f, 0.10f, 1.00f);

    colors[ImGuiCol_ResizeGrip] = ImVec4(0.42f, 0.60f, 0.10f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.42f, 0.60f, 0.10f, 0.50f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.42f, 0.60f, 0.10f, 0.80f);

    colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.42f, 0.60f, 0.10f, 0.40f);
    colors[ImGuiCol_TabActive] = ImVec4(0.42f, 0.60f, 0.10f, 0.70f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.12f, 0.12f, 0.13f, 0.50f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.25f, 0.15f, 0.80f);

    colors[ImGuiCol_DockingPreview] = ImVec4(0.42f, 0.60f, 0.10f, 0.40f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);

    colors[ImGuiCol_PlotLines] = ImVec4(0.42f, 0.60f, 0.10f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.60f, 0.80f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.42f, 0.60f, 0.10f, 0.70f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.50f, 0.70f, 0.15f, 1.00f);

    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.15f, 0.20f, 0.10f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.42f, 0.60f, 0.10f, 0.50f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.42f, 0.60f, 0.10f, 0.20f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.03f);

    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.42f, 0.60f, 0.10f, 0.50f);

    colors[ImGuiCol_DragDropTarget] = ImVec4(0.60f, 0.90f, 0.20f, 0.90f);

    colors[ImGuiCol_NavHighlight] = ImVec4(0.42f, 0.60f, 0.10f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.75f);

    style->WindowRounding = 8.0f;
    style->ChildRounding = 6.0f;
    style->FrameRounding = 4.0f;
    style->GrabRounding = 4.0f;
    style->PopupRounding = 6.0f;
    style->ScrollbarRounding = 9.0f;
    style->TabRounding = 6.0f;

    style->WindowPadding = ImVec2(12.0f, 12.0f);
    style->FramePadding = ImVec2(8.0f, 6.0f);
    style->ItemSpacing = ImVec2(8.0f, 6.0f);
    style->ItemInnerSpacing = ImVec2(6.0f, 4.0f);
    style->CellPadding = ImVec2(2.0f, 2.0f);

    style->WindowBorderSize = 1.0f;
    style->FrameBorderSize = 0.0f;
    style->PopupBorderSize = 1.0f;
    style->TabBorderSize = 0.0f;

    style->AntiAliasedLines = true;
    style->AntiAliasedFill = true;

    style->ScrollbarSize = 10.0f;

    style->GrabMinSize = 12.0f;

    style->WindowTitleAlign = ImVec2(0.50f, 0.50f);

    style->ButtonTextAlign = ImVec2(0.50f, 0.50f);

    style->Alpha = 1.0f;
}

#endif /* THEME_H */
