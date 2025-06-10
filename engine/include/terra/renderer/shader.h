
// #pragma once

// #include "terrapch.h"
// #include <glm/glm.hpp>
// #include <glad/glad.h> // ✅ THIS MUST BE BEFORE ANY USE OF GLenum




// namespace terra {

// class Shader
// {
// public:
//     Shader(const std::string& filepath);
//     Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
//     virtual ~Shader();

//     virtual void bind() const;
//     virtual void unbind() const;

//     virtual void set_int(const std::string& name, int value);
//     virtual void set_int_array(const std::string& name, int* values, uint32_t count);
//     virtual void set_float(const std::string& name, float value);
//     virtual void set_float3(const std::string& name, const glm::vec3& value);
//     virtual void set_float4(const std::string& name, const glm::vec4& value);
//     virtual void set_mat4(const std::string& name, const glm::mat4& value);

//     virtual const std::string& get_name() const { return m_name; }

//     void upload_uniform_int(const std::string& name, int value);
//     void upload_uniform_int_array(const std::string& name, int* values, uint32_t count);

//     void upload_uniform_float(const std::string& name, float value);
//     void upload_uniform_float2(const std::string& name, const glm::vec2& value);
//     void upload_uniform_float3(const std::string& name, const glm::vec3& value);
//     void upload_uniform_float4(const std::string& name, const glm::vec4& value);

//     void upload_uniform_mat3(const std::string& name, const glm::mat3& matrix);
//     void upload_uniform_mat4(const std::string& name, const glm::mat4& matrix);


//     // static ref<Shader> create(const std::string& filepath);
//     // static ref<Shader> create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);

// private:
//     std::string read_file(const std::string& filepath);
//     std::unordered_map<GLenum, std::string> preprocess(const std::string& source);
//     void compile(const std::unordered_map<GLenum, std::string>& shader_sources);

//     uint32_t m_rendererID;
//     std::string m_name;
// };

// class ShaderLibrary
// {
// public:
//     void add(const std::string& name, const ref<Shader>& shader);
//     void add(const ref<Shader>& shader);
//     ref<Shader> load(const std::string& filepath);
//     ref<Shader> load(const std::string& name, const std::string& filepath);

//     ref<Shader> get(const std::string& name);

//     bool exists(const std::string& name) const;
// private:
//     std::unordered_map<std::string, ref<Shader>> m_shaders;
// };

// }
