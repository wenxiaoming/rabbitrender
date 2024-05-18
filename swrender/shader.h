#ifndef SWRENDER_SHADER_H
#define SWRENDER_SHADER_H

#include <stdint.h>
#include <string>
#include "shader_api.h"

namespace swrender {

class Shader {
  public:
    Shader(ShaderType type);
    virtual ~Shader();

    int GetShaderId() const {
        return mShaderInfo.shader_id;
    }

    void UpdateSource(const std::string& source) {
        mShaderInfo.source = std::move(source);
    }

    const std::string &GetSource() {
        return mShaderInfo.source;
    }

  private:
    Shader(const Shader& other);
    Shader& operator=(const Shader& other);

  private:
    static int mShadeIndex;
    ShaderInfo mShaderInfo;
};

class Program {
  public:
    Program(void);
    virtual ~Program();

  private:
    Program(const Program& other);
    Program& operator=(const Program& other);

  private:
    ProgramInfo mProgramInfo;
};

}

#endif