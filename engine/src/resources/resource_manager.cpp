#include "terra/resources/resource_manager.h"

namespace terra {

bool ResourceManager::load_geometry(
    const std::filesystem::path& path, 
    std::vector<f32>& vertex_data, 
    std::vector<u32>& index_data,
    bool is_3d
) {
    std::ifstream file(get_asset_path(path));
    if (!file.is_open()) {
        TR_CORE_ERROR("Failed to open geometry file: {}", path.string());
        return false;
    }

    vertex_data.clear();
    index_data.clear();

    enum class Section {
        None,
        Vertex,
        Indices
    };

    Section current_section = Section::None;
    std::string line;
    size_t line_number = 0;

    while (std::getline(file, line)) {
        ++line_number;

        // Normalize line endings (handle CRLF)
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        if (line.empty() || line[0] == '#')
            continue;

        // Section headers
        if (line == "[vertex]" || line == "[points]") {
            current_section = Section::Vertex;
            continue;
        } else if (line == "[indices]") {
            current_section = Section::Indices;
            continue;
        }

        std::istringstream ss(line);

        if (current_section == Section::Vertex) {
            if (is_3d) {
                float x, y, z, r, g, b;
                if (!(ss >> x >> y >> z >> r >> g >> b)) {
                    TR_CORE_ERROR("Invalid 3D vertex data at line {} in {}", line_number, path.string());
                    return false;
                }
                vertex_data.insert(vertex_data.end(), { x, y, z, r, g, b });
            } else {
                float x, y, r, g, b;
                if (!(ss >> x >> y >> r >> g >> b)) {
                    TR_CORE_ERROR("Invalid 2D vertex data at line {} in {}", line_number, path.string());
                    return false;
                }
                vertex_data.insert(vertex_data.end(), { x, y, r, g, b });
            }
        } else if (current_section == Section::Indices) {
            u32 i0, i1, i2;
            if (!(ss >> i0 >> i1 >> i2)) {
                TR_CORE_ERROR("Invalid index data at line {} in {}", line_number, path.string());
                return false;
            }
            index_data.insert(index_data.end(), { i0, i1, i2 });
        } else {
            TR_CORE_WARN("Line {} not in a recognized section: '{}'", line_number, line);
        }
    }

    if (vertex_data.empty() || index_data.empty()) {
        TR_CORE_ERROR("Geometry file {} did not contain valid vertex or index data.", path.string());
        return false;
    }

    return true;
}


std::string ResourceManager::read_file_as_string(const std::string& relative_path) {
    std::filesystem::path full_path = get_asset_path(relative_path);
    std::ifstream file(full_path);
    
    if (!file.is_open()) {
        TR_CORE_ERROR("Failed to open file: {}", full_path.string());
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace terra
