#ifndef RAIID_H
#define RAIID_H

#include <imgui.h>

// RAII Wrapper for ImGui ID Push/Pop.  
// Useful for UIState lookups when combining multiple widgets into more complex layouts
struct [[nodiscard]] RAIID {
    RAIID(const char* id) { ImGui::PushID(id); }
    RAIID(const char* id_begin, const char* id_end) { ImGui::PushID(id_begin, id_end); }
    RAIID(const void* id) { ImGui::PushID(id); }
    RAIID(int id) { ImGui::PushID(id); }

    ~RAIID() { ImGui::PopID(); }
};

#endif // RAIID_H
