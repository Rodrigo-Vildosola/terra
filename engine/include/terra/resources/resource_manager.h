#pragma once

#include "terrapch.h"


namespace terra {

class ResourceManager {
public:

    static std::filesystem::path get_asset_path(const std::string& relative_path) {
        return std::filesystem::path(ENGINE_ASSETS_DIR) / relative_path;
    }

    static bool load_geometry(
        const std::filesystem::path& path, 
        std::vector<f32>& vertex_data, 
        std::vector<u32>& index_data
    );

};

    
}
