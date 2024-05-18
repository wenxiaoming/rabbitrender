#include "shader_api.h"
#include "shader.h"
#include <memory>
#include "Compiler.h"

namespace swrender {

std::vector<std::shared_ptr<Shader>> g_ShaderList;
std::vector<ProgramInfo> g_ProgramInfo;

int CreateShader(ShaderType type) {
    std::shared_ptr<Shader> shader = std::make_shared<Shader>(type);
    g_ShaderList.push_back(shader);
    return 0;
}

int ShaderSource(int id, std::string &source) {
    auto iter = std::find_if(g_ShaderList.begin(), g_ShaderList.end(),
                             [id](const std::shared_ptr<Shader> shader) {
                                 return id == shader->GetShaderId();
                             });
    if (iter != g_ShaderList.end()) {
        std::shared_ptr<Shader> shader = *iter;
        shader->UpdateSource(source);
        g_ShaderList.erase(iter);
        g_ShaderList.push_back(shader);
    }
    return 0;
}

int CompilerShader(int id) {
    auto iter = std::find_if(g_ShaderList.begin(), g_ShaderList.end(),
                             [id](const std::shared_ptr<Shader> shader) {
                                 return id == shader->GetShaderId();
                             });
    if (iter != g_ShaderList.end()) {
        // call compiler api to do shader compile
        CompileShader((*iter)->GetSource().c_str(), "vert");
    }

    return 0;
}

int LinkShader() {
    return 0;
}

int UserProgram() {
   return 0;
}

int DeleteShader(int shader) {
    (void)shader;
    return 0;
}

}