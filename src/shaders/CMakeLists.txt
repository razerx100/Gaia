file(GLOB_RECURSE SRC *.hlsl)
add_library(Shaders ${SRC})
set_target_properties(Shaders PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(Shaders PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/src/shaders/precompiled)