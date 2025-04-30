#include "imgui_log.h"

void ImguiLog::clear() {
    buf.clear();
    idx_line_offsets.clear();
}

void ImguiLog::add_log(const char* fmt, ...) IM_FMTARGS(2) {
    int old_size = buf.size();
    va_list args;
    va_start(args, fmt);
    buf.appendfv(fmt, args);
    va_end(args);
    for (int new_size = buf.size(); old_size < new_size; old_size++) {
        if (buf[old_size] == '\n') {
            idx_line_offsets.push_back(old_size);
        }
    }
    scroll_to_bottom = true;
}

void ImguiLog::draw() {
    ImGui::SetNextWindowSize(ImVec2(500,400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Log Window", &open);
    if (ImGui::Button("Clear")) {
        clear();
    };
    ImGui::SameLine();
    const bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    filter.Draw("Filter", -100.0f);
    ImGui::Separator();
    ImGui::BeginChild("scrolling");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,1));
    if (copy) {
        ImGui::LogToClipboard();
    }

    if (filter.IsActive())
    {
        const char* buf_begin = buf.begin();
        const char* line = buf_begin;
        for (int line_no = 0; line != NULL; line_no++)
        {
            const char* line_end = (line_no < idx_line_offsets.Size) 
                ? buf_begin + idx_line_offsets[line_no] 
                : NULL;
            if (filter.PassFilter(line, line_end))
                ImGui::TextUnformatted(line, line_end);
            line = line_end && line_end[1] ? line_end + 1 : NULL;
        }
    }
    else
    {
        ImGui::TextUnformatted(buf.begin());
    }

    if (scroll_to_bottom) {
        ImGui::SetScrollHereY(1.0f);
    }
    scroll_to_bottom = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();
}