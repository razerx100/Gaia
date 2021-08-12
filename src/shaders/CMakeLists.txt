file(GLOB_RECURSE SH_SRC *.hlsl *.hlsli)

add_executable(Shaders
    ${SH_SRC}
)

set_target_properties(Shaders PROPERTIES LINKER_LANGUAGE CXX)

if(MSVC)
    set_target_properties(Shaders PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin/Debug/shaders")
    set_target_properties(Shaders PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/Release/shaders")
endif()