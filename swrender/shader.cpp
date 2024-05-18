#include "shader.h"

namespace swrender {

int Shader::mShadeIndex = 0;

Shader::Shader(ShaderType type) {
	mShaderInfo.type = type;
	mShaderInfo.shader_id = mShadeIndex++;
}

Shader::~Shader() {

}

}
