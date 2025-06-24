// #include "terra/renderer/material_manager.h"
// #include "terra/core/assert.h"
// #include "terra/core/logger.h"

// namespace terra {

// WebGPUContext* MaterialManager::s_context = nullptr;
// std::unordered_map<std::string, ref<Material>> MaterialManager::s_materials;

// void MaterialManager::init(WebGPUContext& context) {
//     s_context = &context;
//     create_default_materials();
// }

// void MaterialManager::shutdown() {
//     clear_materials();
//     s_context = nullptr;
// }

// ref<Material> MaterialManager::create_material(const std::string& name) {
//     TR_CORE_ASSERT(s_context, "MaterialManager not initialized");
    
//     if (has_material(name)) {
//         TR_CORE_WARN("Material '{}' already exists, returning existing material", name);
//         return get_material(name);
//     }
    
//     auto material = create_ref<Material>(*s_context, name);
//     s_materials[name] = material;
    
//     TR_CORE_INFO("Created material: {}", name);
//     return material;
// }

// ref<Material> MaterialManager::get_material(const std::string& name) {
//     auto it = s_materials.find(name);
//     TR_CORE_ASSERT(it != s_materials.end(), "Material not found");
//     return it->second;
// }

// bool MaterialManager::has_material(const std::string& name) {
//     return s_materials.find(name) != s_materials.end();
// }

// ref<Material> MaterialManager::create_basic_material(const std::string& name, ref<Shader> shader) {
//     auto material = create_material(name);
//     material->set_shader(shader);
    
//     // Define basic parameters
//     material->define_parameter("u_time", 0, MaterialParamType::Float, 
//                               WGPUShaderStage_Vertex | WGPUShaderStage_Fragment);
    
//     // Set default values
//     material->set_default_float("u_time", 0.0f);
    
//     return material;
// }

// ref<Material> MaterialManager::create_phong_material(const std::string& name, ref<Shader> shader) {
//     auto material = create_material(name);
//     material->set_shader(shader);
    
//     // Define Phong lighting parameters
//     material->define_parameter("u_model", 0, MaterialParamType::Matrix4x4, WGPUShaderStage_Vertex);
//     material->define_parameter("u_view", 1, MaterialParamType::Matrix4x4, WGPUShaderStage_Vertex);
//     material->define_parameter("u_projection", 2, MaterialParamType::Matrix4x4, WGPUShaderStage_Vertex);
//     material->define_parameter("u_ambient", 3, MaterialParamType::Float3, WGPUShaderStage_Fragment);
//     material->define_parameter("u_diffuse", 4, MaterialParamType::Float3, WGPUShaderStage_Fragment);
//     material->define_parameter("u_specular", 5, MaterialParamType::Float3, WGPUShaderStage_Fragment);
//     material->define_parameter("u_shininess", 6, MaterialParamType::Float, WGPUShaderStage_Fragment);
    
//     // Set default values
//     f32 identity_matrix[16] = {
//         1.0f, 0.0f, 0.0f, 0.0f,
//         0.0f, 1.0f, 0.0f, 0.0f,
//         0.0f, 0.0f, 1.0f, 0.0f,
//         0.0f, 0.0f, 0.0f, 1.0f
//     };
    
//     f32 default_ambient[3] = {0.1f, 0.1f, 0.1f};
//     f32 default_diffuse[3] = {0.7f, 0.7f, 0.7f};
//     f32 default_specular[3] = {1.0f, 1.0f, 1.0f};
    
//     material->set_default_matrix4x4("u_model", identity_matrix);
//     material->set_default_matrix4x4("u_view", identity_matrix);
//     material->set_default_matrix4x4("u_projection", identity_matrix);
//     material->set_default_float3("u_ambient", default_ambient);
//     material->set_default_float3("u_diffuse", default_diffuse);
//     material->set_default_float3("u_specular", default_specular);
//     material->set_default_float("u_shininess", 32.0f);
    
//     return material;
// }

// ref<Material> MaterialManager::create_pbr_material(const std::string& name, ref<Shader> shader) {
//     auto material = create_material(name);
//     material->set_shader(shader);
    
//     // Define PBR parameters
//     material->define_parameter("u_albedo", 0, MaterialParamType::Float3, WGPUShaderStage_Fragment);
//     material->define_parameter("u_metallic", 1, MaterialParamType::Float, WGPUShaderStage_Fragment);
//     material->define_parameter("u_roughness", 2, MaterialParamType::Float, WGPUShaderStage_Fragment);
//     material->define_parameter("u_ao", 3, MaterialParamType::Float, WGPUShaderStage_Fragment);
    
//     // Set default values
//     f32 default_albedo[3] = {0.5f, 0.5f, 0.5f};
    
//     material->set_default_float3("u_albedo", default_albedo);
//     material->set_default_float("u_metallic", 0.0f);
//     material->set_default_float("u_roughness", 0.5f);
//     material->set_default_float("u_ao", 1.0f);
    
//     return material;
// }

// ref<MaterialInstance> MaterialManager::create_material_instance(const std::string& material_name, Pipeline* pipeline) {
//     auto material = get_material(material_name);
//     return material->create_instance(pipeline);
// }

// void MaterialManager::clear_materials() {
//     s_materials.clear();
//     TR_CORE_INFO("Cleared all materials");
// }

// void MaterialManager::create_default_materials() {
//     // Create any default materials here if needed
//     TR_CORE_INFO("MaterialManager initialized");
// }

// } // namespace terra 
