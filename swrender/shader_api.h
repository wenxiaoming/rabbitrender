#ifndef SWRENDER_SHADER_API_H
#define SWRENDER_SHADER_API_H

#include <string>
#include <vector>

namespace swrender {

enum class ShaderType {
    VERTEX_SHADER,
    FRAGMENT_SHADER
};

typedef struct _ShaderInfo {
    int shader_id;
    ShaderType type;
    std::string source;
} ShaderInfo;

typedef struct _ProgramInfo {
    int program_id;
    std::vector<ShaderInfo> shader_info;
} ProgramInfo;

#ifdef __cplusplus
extern "C" {
#endif

int CreateShader(ShaderType type);
int ShaderSource(int id, std::string &source);
int CompilerShader(int id);
int LinkShader();
int CreateProgram();
int AttachShader();
int UserProgram();
int DeleteShader(int id);
int DeleteProgram(int id);

#ifdef __cplusplus
}
#endif

}

#endif