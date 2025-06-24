// #pragma once

// #include "terrapch.h"
// #include "terra/renderer/material.h"
// #include <unordered_map>
// #include <memory>

// namespace terra {

// class WebGPUContext;

// // Centralized material management system
// class MaterialManager {
// public:
//     static void init(WebGPUContext& context);
//     static void shutdown();
    
//     // Material creation and management
//     static ref<Material> create_material(const std::string& name);
//     static ref<Material> get_material(const std::string& name);
//     static bool has_material(const std::string& name);
    
//     // Predefined material creators
//     static ref<Material> create_basic_material(const std::string& name, ref<Shader> shader);
//     static ref<Material> create_phong_material(const std::string& name, ref<Shader> shader);
//     static ref<Material> create_pbr_material(const std::string& name, ref<Shader> shader);
    
//     // Material instance creation
//     static ref<MaterialInstance> create_material_instance(const std::string& material_name, Pipeline* pipeline);
    
//     // Cleanup
//     static void clear_materials();

// private:
//     static WebGPUContext* s_context;
//     static std::unordered_map<std::string, ref<Material>> s_materials;
    
//     static void create_default_materials();
// };

// } // namespace terra 
