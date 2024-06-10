################################################################################
# MachineIndependent
################################################################################
set(SHADER_SOURCES
	Shader.cpp
	VertexShader.cpp
	PixelShader.cpp
	Config.cpp
	Color.cpp
)

set(SHADER_HEADERS
	Shader.hpp
	VertexShader.hpp
	PixelShader.hpp
	Config.hpp
)

set (BUILD_SHARED_LIB_NAME ShaderLib)
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/base")
add_library(${BUILD_SHARED_LIB_NAME} STATIC ${SHADER_SOURCES} ${SHADER_HEADERS})
set_property(TARGET ${BUILD_SHARED_LIB_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET ${BUILD_SHARED_LIB_NAME} PROPERTY FOLDER glslang)

# Root directory for build-time generated include files
set(GLSLANG_GENERATED_INCLUDEDIR "${CMAKE_BINARY_DIR}/include")

target_link_libraries(${BUILD_SHARED_LIB_NAME} PRIVATE OGLCompiler OSDependent GenericCodeGen)