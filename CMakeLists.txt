cmake_minimum_required(VERSION 3.11)

project(Gaia
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

if(IMGUIFLAG STREQUAL "DISABLE")
    file(GLOB_RECURSE SRC src/App.cpp src/main.cpp src/Exceptions/*.cpp src/Graphics/*.cpp src/shaders/*.cpp src/Utility/*.cpp src/Window/*.cpp includes/*.hpp)
else()
    file(GLOB_RECURSE SRC src/*.cpp includes/*.hpp)
endif()

add_executable(${PROJECT_NAME} WIN32
    ${SRC}
    resources/resource.rc
)

target_include_directories(${PROJECT_NAME} PRIVATE includes/ external/ includes/Exceptions  includes/Graphics includes/Graphics/Bindable includes/Graphics/Drawable includes/Graphics/Vertex includes/Graphics/PipelineObjects includes/Macros includes/Utility includes/Window includes/ImGui)

if(IMGUIFLAG STREQUAL "ENABLE")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _IMGUI)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:_DEBUG>" "$<$<CONFIG:RELEASE>:NDEBUG>")

target_link_libraries(${PROJECT_NAME} PRIVATE
    d3d12.lib dxgi.lib dxguid.lib uuid.lib d3dcompiler.lib
    ${PROJECT_SOURCE_DIR}/external/libs/*.lib
)

if(MSVC)
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    string(REPLACE "/W3 " "/W4 " CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /fp:fast /MP /EHa /Ot)
endif()

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

add_subdirectory(src/shaders)

