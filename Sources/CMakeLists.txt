# Target name
set(target gl_shaded_gltfscene)

# Define root directory
set(root_dir ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Includes
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Sources
file(GLOB header_dir
    ${root_dir}/Includes
)

file(GLOB_RECURSE headers
    ${header_dir}/*.hpp
)

file(GLOB_RECURSE sources
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

# MSVC compiler options
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	set(DEFAULT_COMPILE_OPTIONS ${DEFAULT_COMPILE_OPTIONS}
		/wd4201 # ignore warning in "glm nonstandard extension used : nameless struct/union"
	)
endif ()

# Build executable
add_executable(${target}
    ${sources})

# Project options
set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
)

# Compile options
target_compile_options(${target}
    PRIVATE

    PUBLIC
    ${DEFAULT_COMPILE_OPTIONS}

    INTERFACE
)

# Compile definitions
target_compile_definitions(${target}
    PRIVATE
    RESOURCES_DIR="${RESOURCES_DIR}"
)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(${target}
        PRIVATE
    
        PUBLIC
        ${DEFAULT_LINKER_OPTIONS}
    	${DEFAULT_LIBRARIES}
        glad
        glfw
        draco
        INTERFACE
    )
else()
    target_link_libraries(${target}
        PRIVATE
    
        PUBLIC
        ${DEFAULT_LINKER_OPTIONS}
    	${DEFAULT_LIBRARIES}
        glad
        glfw
        draco_static
        INTERFACE
    )
endif()