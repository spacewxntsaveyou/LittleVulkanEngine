cmake_minimum_required(VERSION 4.1)
project(lve  LANGUAGES C CXX)

# 1. Set C/C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_C_STANDARD 17)
set (CMAKE_C_STANDARD_REQUIRED ON)

# 2. Platform specific settings/flags

# Set MSVC runtime
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()
# Linux specific compiler flags
if (UNIX AND NOT APPLE)
    #set(CMAKE_CXX_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

# 3. Find Vulkan (MUST be called AFTER project)
find_package(Vulkan REQUIRED COMPONENTS glslc)

# 4. Library Flags
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# 5. Libraries
add_subdirectory(thirdparty/glfw-3.4)
add_subdirectory(thirdparty/glm-1.0.3)
add_subdirectory(thirdparty/tinyobjloader-2.0)



# 6. Create your executable and link libraries
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/systems/*.cpp")
add_executable("${CMAKE_PROJECT_NAME}" ${MY_SOURCES})

target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/" "${CMAKE_CURRENT_SOURCE_DIR}/systems/")

# Link against the imported CMake target
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE
    glfw
    glm
    tinyobjloader
    Vulkan::Vulkan 
)

# 7. Create output directories and copies "ressources"
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/ressources/models)

add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/ressources/models/
        ${CMAKE_BINARY_DIR}/ressources/models/
        COMMENT "Copying model files to Output directory"
)


# 8. Compile shaders
file(GLOB_RECURSE SHADER_SOURCES "shaders/*.vert" "shaders/*.frag")

       foreach (SOURCE_FILE ${SHADER_SOURCES})
           #Creates output filename (ie shader.vert.spv)
           get_filename_component(FILE_NAME ${SOURCE_FILE} NAME)
           set(OUTPUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
           #Triggers compilation
           add_custom_command(
                   OUTPUT ${OUTPUT_FILE}
                   COMMAND Vulkan::glslc "${SOURCE_FILE}" -o "${OUTPUT_FILE}"
                   DEPENDS ${SOURCE_FILE}
                   COMMENT "Compiling ${SOURCE_FILE} to SPIR-V"
           )
           #Collect all .spv into a list
           list (APPEND SPIRV_FILES ${OUTPUT_FILE})
       endforeach ()

add_custom_target(CompileShaders ALL DEPENDS ${SPIRV_FILES})
add_dependencies(lve CompileShaders)