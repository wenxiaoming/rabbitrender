#ifndef GLSL_COMPILER_H
#define GLSL_COMPILER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_SHADER_LIBRARY

void CompileShader(const char *shaderString, const std::string &ShaderTypeName);

#endif

#ifdef __cplusplus
}
#endif

#endif