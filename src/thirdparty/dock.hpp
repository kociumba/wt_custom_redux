// credit to: https://github.com/ocornut/imgui/issues/9169#issuecomment-3975678015
#ifndef DOCK_HPP
#define DOCK_HPP

#include <imgui_internal.h>
#include <memory>
#include <string>

class DockBuilder {
   public:
    DockBuilder(ImGuiID parent_id, DockBuilder* parent = nullptr)
        : m_parentId{parent_id}, m_parent{parent} {}

    DockBuilder& SetFlags(ImGuiDockNodeFlags flags) {
        ImGui::DockBuilderAddNode(m_parentId, flags);

        ImGuiDockNode* node = ImGui::DockBuilderGetNode(m_parentId);
        if (node) {
            node->LocalFlags |= flags;
        }
        return *this;
    }

    DockBuilder& Window(const std::string& name) {
        if (m_parent == nullptr) {
            ImGui::DockBuilderAddNode(m_parentId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(m_parentId, ImGui::GetMainViewport()->Size);
        }

        ImGui::DockBuilderDockWindow(name.c_str(), m_parentId);

        return *this;
    }

    DockBuilder& DockLeft(float size) {
        ImGuiID dock_id =
            ImGui::DockBuilderSplitNode(m_parentId, ImGuiDir_Left, size, nullptr, &m_parentId);
        m_left = std::make_unique<DockBuilder>(dock_id, this);

        return *m_left;
    }

    DockBuilder& DockRight(float size) {
        ImGuiID dock_id =
            ImGui::DockBuilderSplitNode(m_parentId, ImGuiDir_Right, size, nullptr, &m_parentId);
        m_right = std::make_unique<DockBuilder>(dock_id, this);

        return *m_right;
    }

    DockBuilder& DockTop(float size) {
        ImGuiID dock_id =
            ImGui::DockBuilderSplitNode(m_parentId, ImGuiDir_Up, size, nullptr, &m_parentId);
        m_top = std::make_unique<DockBuilder>(dock_id, this);

        return *m_top;
    }

    DockBuilder& DockBottom(float size) {
        ImGuiID dock_id =
            ImGui::DockBuilderSplitNode(m_parentId, ImGuiDir_Down, size, nullptr, &m_parentId);
        m_bottom = std::make_unique<DockBuilder>(dock_id, this);

        return *m_bottom;
    }

    DockBuilder& Done() {
        assert(m_parent && "dock_builder has no parent");

        return *m_parent;
    }

   private:
    DockBuilder* m_parent;
    ImGuiID m_parentId;
    std::unique_ptr<DockBuilder> m_left;
    std::unique_ptr<DockBuilder> m_right;
    std::unique_ptr<DockBuilder> m_top;
    std::unique_ptr<DockBuilder> m_bottom;

    int NumOfNodes() {
        int count = 1;  // count self

        if (m_left != nullptr) count++;
        if (m_right != nullptr) count++;
        if (m_top != nullptr) count++;
        if (m_bottom != nullptr) count++;

        return count;
    }
};

#endif /* DOCK_HPP */
