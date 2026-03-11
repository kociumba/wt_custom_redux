#ifndef CSV_EDITOR_HPP
#define CSV_EDITOR_HPP

#include <result.h>
#include <future>
#include <span>
#include <string>
#include <vector>

// ─── csv dasta ───

struct CsvData {
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    bool dirty = false;
};

// --- window state ---

struct ToolbarState {
    char current_path[512] = {};
};

struct SearchState {
    std::string query;
    bool open = false;
    bool request_focus = false;
    int match_index = -1;
    std::vector<std::pair<int, int>> matches;

    void clear() {
        matches.clear();
        match_index = -1;
    }

    std::pair<int, int> current() const {
        if (match_index < 0 || match_index >= (int)matches.size()) return {-1, -1};
        return matches[match_index];
    }

    void step(int delta) {
        if (matches.empty()) return;
        match_index = ((match_index + delta) % (int)matches.size() + (int)matches.size()) %
                      (int)matches.size();
    }
};

struct TableState {
    std::vector<std::vector<std::string>> edit_bufs;

    int scroll_to_row = -1;

    void rebuild(const CsvData& csv) {
        edit_bufs.resize(csv.rows.size());
        for (std::size_t r = 0; r < csv.rows.size(); ++r) {
            edit_bufs[r] = csv.rows[r];
            edit_bufs[r].resize(csv.headers.size());
        }
    }

    bool add_empty_row() {
        if (!edit_bufs.empty()) {
            edit_bufs.emplace_back(edit_bufs[0].size());
            return true;
        }

        return false;
    }

    void flush(CsvData& csv) const {
        csv.rows.resize(edit_bufs.size());
        for (std::size_t r = 0; r < edit_bufs.size(); ++r)
            csv.rows[r] = edit_bufs[r];
    }
};

struct StatusBarState {
    char message[256] = "Ready.";
    float message_timer = 0.f;
};

// ─── general state ───

struct state_t {
    CsvData csv;
    ToolbarState toolbar;
    TableState table;
    StatusBarState status_bar;
    SearchState search;

    std::future<ktl::result<CsvData, const char*>> csv_load_res;
    bool is_loading = false;
    std::string picked_path;
};

ktl::result<CsvData, const char*> load_csv(const char* path);
ktl::result<bool, const char*> save_csv(const char* path, const CsvData& csv);

void set_status(StatusBarState& s, const char* msg, float duration);

void draw_toolbar(state_t& app);
void draw_table(state_t& app);
void draw_status_bar(state_t& app, float dt);

void csv_editor_draw(state_t& app, float dt);

#endif /* CSV_EDITOR_HPP */
