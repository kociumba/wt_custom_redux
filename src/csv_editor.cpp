#include "csv_editor.hpp"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_stdlib.h>
#include <tinyfiledialogs.h>

#include <once.h>

#include <csv.hpp>

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>

#include "globals.h"
#include "strings.h"
#include "thirdparty/progress.hpp"

// TODO: only remaining bug with this is "" -> [empty] on save, can not be easely fixed with custom format specifiers
//       this should still work with warthunder but it could have some edge cases

void hide_tab_bar() {
    ImGuiDockNode* node = ImGui::GetWindowDockNode();
    if (node != nullptr) { node->LocalFlags |= ImGuiDockNodeFlags_HiddenTabBar; }
}

ktl::result<CsvData, const char*> load_csv(const char* path) {
    CsvData out;

    csv::CSVFormat format;
    // format.delimiter(';').quote('\'').variable_columns(csv::VariableColumnPolicy::KEEP);

    csv::CSVReader reader(path /*, format*/);

    out.headers = reader.get_col_names();
    if (out.headers.empty()) return ktl::err("CSV has no columns");

    for (csv::CSVRow& row : reader) {
        std::vector<std::string> r;
        r.reserve(out.headers.size());
        for (auto& field : row)
            r.push_back(field.get<std::string>());
        out.rows.push_back(std::move(r));
    }

    return std::move(out);
}

std::string out_str(const std::string& str) {
    if (str.empty() || str.size() == 0) { return str; }
    return '"' + double_quotes(str) + '"';
}

ktl::result<bool, const char*> save_csv(const char* path, CsvData& csv) {
    std::ofstream f(path, std::ios::binary);
    if (!f.is_open()) return {false, "Failed to open file for writing"};

    for (size_t i = 0; i < csv.headers.size(); ++i) {
        if (i > 0) f << ';';
        f << out_str(csv.headers[i]);
        // f << csv.headers[i];
    }
    f << '\n';

    for (const auto& row : csv.rows) {
        for (size_t i = 0; i < row.size(); ++i) {
            if (i > 0) f << ';';
            f << out_str(row[i]);
            // f << row[i];
        }
        f << '\n';
    }

    return true;
}

void set_status(StatusBarState& s, const char* msg, float duration = 5.f) {
    std::snprintf(s.message, sizeof(s.message), "%s", msg);
    s.message_timer = duration;
}

static bool str_contains_icase(const std::string& str, const char* substr) {
    if (!substr || substr[0] == '\0') return false;
    const std::size_t nlen = std::strlen(substr);
    if (nlen > str.size()) return false;
    auto it = std::search(
        str.begin(), str.end(), substr, substr + nlen, [](unsigned char a, unsigned char b) {
            return std::tolower(a) == std::tolower(b);
        });

    return it != str.end();
}

static void rebuild_matches(state_t& s) {
    s.search.clear();
    if (s.search.query.empty()) { return; }

    const int col_count = static_cast<int>(s.csv.headers.size());
    for (int r = 0; r < (int)s.table.edit_bufs.size(); ++r) {
        for (int c = 0; c < col_count; ++c) {
            if (str_contains_icase(s.table.edit_bufs[r][c], s.search.query.c_str()))
                s.search.matches.push_back({r, c});
        }
    }

    if (!s.search.matches.empty()) s.search.match_index = 0;
}

static void jump_to_current_match(state_t& app) {
    auto [row, col] = app.search.current();
    if (row >= 0) app.table.scroll_to_row = row;
}

// ─── Window: Toolbar ---

void draw_toolbar(state_t& s) {
    ImGui::Begin("Toolbar##csv");

    ktl_once(hide_tab_bar());

    const ImVec2 avail = ImGui::GetContentRegionAvail();
    const bool is_sidebar = avail.y > avail.x;

    auto calc_width = [&](float factor) -> float {
        return (avail.x - ImGui::GetStyle().ItemSpacing.x) * factor;
    };

    auto flex_button = [&](const char* label,
                           float width_factor = 1.0f,
                           bool same_line_as_prev = false,
                           bool same_line_as_next = false) -> bool {
        if (is_sidebar) {
            if (same_line_as_prev) { ImGui::SameLine(0, ImGui::GetStyle().ItemSpacing.x); }

            float spacing = ImGui::GetStyle().ItemSpacing.x;
            bool in_shared_row = same_line_as_prev || same_line_as_next;
            float effective_avail = in_shared_row ? (avail.x - spacing) : avail.x;
            float width = effective_avail * width_factor;

            return ImGui::Button(label, ImVec2(width, 0.f));
        } else {
            if (same_line_as_prev) ImGui::SameLine();
            return ImGui::Button(label);
        }
    };

    if (flex_button("Open", 1.)) {
        const char* filters[] = {"*.csv", "*.txt"};
        const char* picked = tinyfd_openFileDialog("Open CSV", "", 2, filters, "CSV files", 0);

        if (picked) {
            s.is_loading = true;
            s.csv_load_res = std::async(std::launch::async,
                [path = std::string(picked)] { return load_csv(path.c_str()); });
            s.picked_path = picked;
        }
    }

    ImGui::SetItemTooltip("you can also drag and drop files to open them");

    if (s.is_loading && s.csv_load_res.valid()) {
        if (s.csv_load_res.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            auto [data, err] = s.csv_load_res.get();
            s.is_loading = false;

            if (err) {
                set_status(s.status_bar, err);
            } else {
                s.csv = std::move(data);
                s.table.rebuild(s.csv);
                s.csv.dirty = false;
                std::snprintf(s.toolbar.current_path,
                    sizeof(s.toolbar.current_path),
                    "%s",
                    s.picked_path.c_str());
                set_status(s.status_bar, "File loaded.");

                s.search.clear();
                s.search.query[0] = '\0';
            }
        }
    }

    ImGui::Spacing();
    if (!is_sidebar) ImGui::SameLine();

    const bool has_path = s.toolbar.current_path[0] != '\0';
    if (!has_path) ImGui::BeginDisabled();
    if (flex_button("Save", .5, false, true)) {
        s.table.flush(s.csv);
        auto [ok, err] = save_csv(s.toolbar.current_path, s.csv);
        if (err)
            set_status(s.status_bar, err);
        else {
            s.csv.dirty = false;
            set_status(s.status_bar, "Saved.");
        }
    }
    if (!has_path) ImGui::EndDisabled();

    if (flex_button("Save As", .5, true)) {
        const char* filters[] = {"*.csv"};
        const char* picked = tinyfd_saveFileDialog("Save CSV As",
            has_path ? s.toolbar.current_path : "output.csv",
            1,
            filters,
            "CSV files");

        if (picked) {
            s.table.flush(s.csv);
            auto [ok, err] = save_csv(picked, s.csv);
            if (err)
                set_status(s.status_bar, err);
            else {
                std::snprintf(s.toolbar.current_path, sizeof(s.toolbar.current_path), "%s", picked);
                s.csv.dirty = false;
                set_status(s.status_bar, "Saved.");
            }
        }
    }

    if (s.csv.dirty) {
        if (!is_sidebar) {
            ImGui::SameLine();
            ImGui::TextDisabled("(unsaved)");
        } else {
            ImGui::TextDisabled("(unsaved changes)");
        }
    }

    ImGui::Spacing();
    if (is_sidebar) ImGui::Separator();
    ImGui::Spacing();
    if (!is_sidebar) ImGui::SameLine();

    if (flex_button("Add Row")) {
        if (s.table.add_empty_row()) { s.csv.dirty = true; }
    }

    ImGui::Spacing();
    if (is_sidebar) ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_F)) {
        s.search.open = true;
        s.search.request_focus = true;
    }

    const float search_w = is_sidebar ? avail.x : ImMax(80.f, avail.x - 80.f);
    if (!is_sidebar) ImGui::SameLine();

    if (s.search.request_focus) {
        ImGui::SetKeyboardFocusHere();
        s.search.request_focus = false;
    }

    ImGui::SetNextItemWidth(search_w);
    const bool query_changed = ImGui::InputTextWithHint(
        "##search", "search (ctrl + f)", &s.search.query, ImGuiInputTextFlags_AutoSelectAll);

    if (query_changed) {
        rebuild_matches(s);
        jump_to_current_match(s);
    }

    if (ImGui::IsItemFocused()) {
        const bool shift = ImGui::GetIO().KeyShift;
        if (ImGui::IsKeyPressed(ImGuiKey_Enter, false)) {
            s.search.step(shift ? -1 : +1);
            jump_to_current_match(s);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_Escape, false)) {
            s.search.open = false;
            s.search.clear();
            s.search.query[0] = '\0';
        }
    }

    if (s.search.query[0] != '\0') {
        const int total = static_cast<int>(s.search.matches.size());
        if (!is_sidebar) ImGui::SameLine();
        if (total == 0)
            ImGui::TextDisabled("no matches");
        else
            ImGui::TextDisabled("%d / %d", s.search.match_index + 1, total);
    }

    ImGui::End();
}

// ─── Window: CSV Table ───

void draw_table(state_t& s) {
    ImGui::Begin("CSV Table##csv");

    ktl_once(hide_tab_bar());

    if (s.is_loading) {
        ImVec2 window_size = ImGui::GetWindowSize();
        float radius = window_size.x * 0.1f;

        ImVec2 cursor_pos =
            ImVec2((window_size.x - radius) * 0.5f, (window_size.y - radius) * 0.5f);

        ImGui::SetCursorPos(cursor_pos);
        ImGui::Spinner("Loading...", radius / 2, 5, ImGui::GetColorU32(ImGuiCol_ButtonActive));
        ImGui::End();
        return;
    } else if (s.csv.headers.empty()) {
        ImGui::TextDisabled("No file loaded.");
        ImGui::End();
        return;
    }

    const int col_count = static_cast<int>(s.csv.headers.size());

    constexpr ImGuiTableFlags table_flags =
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollX |
        ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
        ImGuiTableFlags_SizingFixedFit;

    if (!ImGui::BeginTable("##data", col_count + 1, table_flags)) {
        ImGui::End();
        return;
    }

    if (s.table.scroll_to_row >= 0) {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float row_h =
            ImGui::GetTextLineHeight() + style.FramePadding.y * 2.f + style.CellPadding.y * 2.f;
        ImGui::SetScrollY((s.table.scroll_to_row) * row_h);
        s.table.scroll_to_row = -1;
    }

    ImGui::TableSetupScrollFreeze(1, 1);
    ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed, 40.f);
    for (const auto& h : s.csv.headers)
        ImGui::TableSetupColumn(h.c_str());
    ImGui::TableHeadersRow();

    auto [match_row, match_col] = s.search.current();

    ImGuiListClipper clipper;
    clipper.Begin(static_cast<int>(s.table.edit_bufs.size()));

    while (clipper.Step()) {
        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::TextDisabled("%d", row + 1);

            for (int col = 0; col < col_count; ++col) {
                ImGui::TableSetColumnIndex(col + 1);

                auto& cell = s.table.edit_bufs[row][col];

                char id[32];
                std::snprintf(id, sizeof(id), "##r%dc%d", row, col);

                const bool is_match = (row == match_row && col == match_col);
                const bool has_query = s.search.query[0] != '\0';
                const bool is_hit = has_query && str_contains_icase(cell, s.search.query.c_str());

                if (is_match) {
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.50f, 0.75f, 0.12f, 0.60f));
                    ImGui::PushStyleColor(
                        ImGuiCol_FrameBgHovered, ImVec4(0.55f, 0.80f, 0.15f, 0.75f));
                    ImGui::PushStyleColor(
                        ImGuiCol_FrameBgActive, ImVec4(0.60f, 0.85f, 0.18f, 0.90f));
                } else if (is_hit) {
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.30f, 0.45f, 0.08f, 0.35f));
                    ImGui::PushStyleColor(
                        ImGuiCol_FrameBgHovered, ImVec4(0.35f, 0.50f, 0.10f, 0.50f));
                    ImGui::PushStyleColor(
                        ImGuiCol_FrameBgActive, ImVec4(0.40f, 0.55f, 0.12f, 0.65f));
                } else {
                    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(1.f, 1.f, 1.f, 0.06f));
                    ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.f, 1.f, 1.f, 0.12f));
                }

                ImGui::SetNextItemWidth(-FLT_MIN);
                if (ImGui::InputText(id, &cell)) {
                    s.csv.dirty = true;
                    if (s.search.query[0] != '\0') rebuild_matches(s);
                }

                ImGui::PopStyleColor(3);
            }
        }
    }
    clipper.End();

    ImGui::EndTable();
    ImGui::End();
}

// ─── Window: Status Bar ───

void draw_status_bar(state_t& s, float dt) {
    if (s.status_bar.message_timer > 0.f) {
        s.status_bar.message_timer -= dt;
        if (s.status_bar.message_timer <= 0.f)
            std::snprintf(s.status_bar.message, sizeof(s.status_bar.message), "Ready.");
    }

    ImGui::Begin("Status##csv",
        nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);

    ktl_once(hide_tab_bar());

    ImGui::Text("%s", s.status_bar.message);

    if (s.toolbar.current_path[0]) {
        ImGui::SameLine();
        ImGui::TextDisabled("  |  %s  |  %zu rows  |  %zu cols",
            s.toolbar.current_path,
            s.csv.rows.size(),
            s.csv.headers.size());
    }

    ImGui::End();
}

static void handle_dropped_file(state_t& s) {
    if (!dropped) return;
    dropped = false;

    if (dropped_payload.empty()) return;

    s.is_loading = true;
    s.csv_load_res =
        std::async(std::launch::async, [path = dropped_payload] { return load_csv(path.c_str()); });
    s.picked_path = dropped_payload;

    dropped_payload.clear();
}

void csv_editor_draw(state_t& s, float dt) {
    handle_dropped_file(s);

    draw_toolbar(s);
    draw_table(s);
    draw_status_bar(s, dt);
}