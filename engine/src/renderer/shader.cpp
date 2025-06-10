// #include "terrapch.h"
// #include "terra/renderer/shader.h"

// #include <fstream>
// #include <glad/glad.h>
// #include <glm/gtc/type_ptr.hpp>

// namespace terra {

// void ShaderLibrary::add(const std::string& name, const ref<Shader>& shader) {
//     TR_CORE_ASSERT(!exists(name), "Shader already exists!");
//     m_shaders[name] = shader;
// }

// void ShaderLibrary::add(const ref<Shader>& shader) {
//     auto& name = shader->get_name();
//     add(name, shader);
// }

// ref<Shader> ShaderLibrary::load(const std::string& filepath) {
//     auto shader = create_ref<Shader>(filepath);
//     add(shader);
//     return shader;
// }

// ref<Shader> ShaderLibrary::load(const std::string& name, const std::string& filepath) {
//     auto shader = create_ref<Shader>(filepath);
//     add(name, shader);
//     return shader;
// }

// ref<Shader> ShaderLibrary::get(const std::string& name) {
//     TR_CORE_ASSERT(exists(name), "Shader not found!");
//     return m_shaders[name];
// }

// bool ShaderLibrary::exists(const std::string& name) const {
//     return m_shaders.find(name) != m_shaders.end();
// }




// static GLenum shader_type_from_string(const std::string& type) {
//     if (type == "vertex")
//         return GL_VERTEX_SHADER;
//     if (type == "fragment" || type == "pixel")
//         return GL_FRAGMENT_SHADER;

//     TR_CORE_ASSERT(false, "Unknown shader type!");
//     return 0;
// }

// Shader::Shader(const std::string& filepath) {
//     TR_CORE_INFO("Loading shader from file: {0}", filepath);
//     std::string source = read_file(filepath);
//     auto shaderSources = preprocess(source);
//     compile(shaderSources);


//     // Extract name from filepath
//     auto lastSlash = filepath.find_last_of("/\\");
//     lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
//     auto lastDot = filepath.rfind('.');
//     auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
//     m_name = filepath.substr(lastSlash, count);
// }

// Shader::Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc) : m_name(name) {
    

//     std::unordered_map<GLenum, std::string> sources;
//     sources[GL_VERTEX_SHADER] = vertexSrc;
//     sources[GL_FRAGMENT_SHADER] = fragmentSrc;
//     compile(sources);
// }

// Shader::~Shader() {
//     glDeleteProgram(m_rendererID);
// }

// std::string Shader::read_file(const std::string& filepath) {
    

//     std::string result;
//     std::ifstream in(filepath, std::ios::in | std::ios::binary); // ifstream closes itself due to RAII
//     if (in)
//     {
//         in.seekg(0, std::ios::end);
//         size_t size = in.tellg();
//         if (size != -1)
//         {
//             result.resize(size);
//             in.seekg(0, std::ios::beg);
//             in.read(&result[0], size);
//         }
//         else
//         {
//             TR_CORE_ERROR("Could not read from file '{0}'", filepath);
//         }
//     }
//     else
//     {
//         TR_CORE_ERROR("Could not open file '{0}'", filepath);
//     }

//     return result;
// }

// std::unordered_map<GLenum, std::string> Shader::preprocess(const std::string& source) {
    

//     std::unordered_map<GLenum, std::string> shaderSources;

//     const char* typeToken = "#type";
//     size_t typeTokenLength = strlen(typeToken);
//     size_t pos = source.find(typeToken, 0);
//     while (pos != std::string::npos)
//     {
//         size_t eol = source.find_first_of("\r\n", pos);
//         TR_CORE_ASSERT(eol != std::string::npos, "Syntax error");
//         size_t begin = pos + typeTokenLength + 1;
//         std::string type = source.substr(begin, eol - begin);
//         TR_CORE_ASSERT(shader_type_from_string(type), "Invalid shader type specified");

//         size_t nextLinePos = source.find_first_not_of("\r\n", eol); //Start of shader code after shader type declaration line
//         TR_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
//         pos = source.find(typeToken, nextLinePos); //Start of next shader type declaration line

//         shaderSources[shader_type_from_string(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
//     }

//     return shaderSources;
// }

// void Shader::compile(const std::unordered_map<GLenum, std::string>& shaderSources) {
    

//     GLuint program = glCreateProgram();
//     TR_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");
//     std::array<GLenum, 2> glShaderIDs;
//     int glShaderIDIndex = 0;
//     for (auto& kv : shaderSources)
//     {
//         GLenum type = kv.first;
//         const std::string& source = kv.second;

//         TR_CORE_INFO("Compiling shader type {0}", type == GL_VERTEX_SHADER ? "Vertex" : "Fragment");
//         TR_CORE_INFO("Shader source:\n{0}", source);

//         GLuint shader = glCreateShader(type);

//         const GLchar* sourceCStr = source.c_str();
//         glShaderSource(shader, 1, &sourceCStr, 0);

//         glCompileShader(shader);

//         GLint isCompiled = 0;
//         glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
//         if (isCompiled == GL_FALSE)
//         {
//             GLint maxLength = 0;
//             glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

//             std::vector<GLchar> infoLog(maxLength);
//             glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

//             glDeleteShader(shader);

//             TR_CORE_ERROR("Shader compilation error: {0}", infoLog.data());
//             TR_CORE_ASSERT(false, "Shader compilation failure!");
//             break;
//         }

//         glAttachShader(program, shader);
//         glShaderIDs[glShaderIDIndex++] = shader;
//     }

//     m_rendererID = program;

//     // Link our program
//     TR_CORE_INFO("Linking shader program");
//     glLinkProgram(program);

//     // Note the different functions here: glGetProgram* instead of glGetShader*.
//     GLint isLinked = 0;
//     glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
//     if (isLinked == GL_FALSE)
//     {
//         GLint maxLength = 0;
//         glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

//         // The maxLength includes the NULL character
//         std::vector<GLchar> infoLog(maxLength);
//         glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

//         // We don't need the program anymore.
//         glDeleteProgram(program);

//         for (auto id : glShaderIDs)
//             glDeleteShader(id);

//         TR_CORE_ERROR("Shader link error: {0}", infoLog.data());
//         TR_CORE_ASSERT(false, "Shader link failure!");
//         return;
//     }

//     for (auto id : glShaderIDs)
//     {
//         glDetachShader(program, id);
//         glDeleteShader(id);
//     }


//     TR_CORE_INFO("Shader compiled and linked successfully!");
// }

// void Shader::bind() const {
    

//     glUseProgram(m_rendererID);
// }

// void Shader::unbind() const {
//     glUseProgram(0);
// }

// void Shader::set_int(const std::string& name, int value) {
//     upload_uniform_int(name, value);
// }

// void Shader::set_int_array(const std::string& name, int* values, uint32_t count) {
//     upload_uniform_int_array(name, values, count);
// }

// void Shader::set_float(const std::string& name, float value) {
//     upload_uniform_float(name, value);
// }

// void Shader::set_float3(const std::string& name, const glm::vec3& value) {
//     upload_uniform_float3(name, value);
// }

// void Shader::set_float4(const std::string& name, const glm::vec4& value) {
//     upload_uniform_float4(name, value);
// }

// void Shader::set_mat4(const std::string& name, const glm::mat4& value) {
//     upload_uniform_mat4(name, value);
// }


// void Shader::upload_uniform_int(const std::string& name, int value) {
//     GLint location = glGetUniformLocation(m_rendererID, name.c_str());
//     glUniform1i(location, value);
// }

// void Shader::upload_uniform_int_array(const std::string& name, int* values, uint32_t count) {
//     GLint location = glGetUniformLocation(m_rendererID, name.c_str());
//     glUniform1iv(location, count, values);
// }


// void Shader::upload_uniform_float(const std::string& name, float value) {
//     GLint location = glGetUniformLocation(m_rendererID, name.c_str());
//     glUniform1f(location, value);
// }

// void Shader::upload_uniform_float2(const std::string& name, const glm::vec2& value) {
//     GLint location = glGetUniformLocation(m_rendererID, name.c_str());
//     glUniform2f(location, value.x, value.y);
// }

// void Shader::upload_uniform_float3(const std::string& name, const glm::vec3& value) {
//     GLint location = glGetUniformLocation(m_rendererID, name.c_str());
//     glUniform3f(location, value.x, value.y, value.z);
// }

// void Shader::upload_uniform_float4(const std::string& name, const glm::vec4& value) {
//     GLint location = glGetUniformLocation(m_rendererID, name.c_str());
//     glUniform4f(location, value.x, value.y, value.z, value.w);
// }

// void Shader::upload_uniform_mat3(const std::string& name, const glm::mat3& matrix) {
//     GLint location = glGetUniformLocation(m_rendererID, name.c_str());
//     glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
// }

// void Shader::upload_uniform_mat4(const std::string& name, const glm::mat4& matrix) {
//     GLint location = glGetUniformLocation(m_rendererID, name.c_str());
//     glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
// }

// }
