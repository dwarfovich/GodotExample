cmake_minimum_required(VERSION 3.14)

set(GDEXTENSION_LIB_NAME temp_project)
set(GDEXTENSION_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/project/bin")

option(
  AUTOFORMAT_SRC_ON_CONFIGURE
    "If enabled, clang-format will be used to format all sources in src/ during configuration"
        ON
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_MESSAGE_LOG_LEVEL STATUS)

project("${GDEXTENSION_LIB_NAME}"
  LANGUAGES
    C CXX
  VERSION
    0.1.0
)

list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/"
)

#include(FetchContent)

include(godot-engine)
include(godot-cpp)


file(GLOB_RECURSE gdext_sources
     CONFIGURE_DEPENDS
       "${CMAKE_CURRENT_SOURCE_DIR}/src/*.[hc]"
       "${CMAKE_CURRENT_SOURCE_DIR}/src/*.[hc]pp"
)

add_library(${PROJECT_NAME}
    SHARED
      ${gdext_sources}
)

include(common_compiler_flags)

# Set compiler options for the gdextension library based on the compiler being used:
target_compile_options(${PROJECT_NAME} PUBLIC
    $<${compiler_is_msvc}:
        /EHsc
        /utf-8
        /Zc:preprocessor
    >
    $<$<NOT:${compiler_is_msvc}>:
        -g
        -Wno-unused-value
        $<${compiler_is_gnu}:
            -Wno-attributes
            -Wno-attributes=rl::
        >
        $<${compiler_is_clang}:
            -Wno-ignored-attributes
            -Wno-unknown-attributes
        >
        $<$<CONFIG:Debug>:
            -fno-omit-frame-pointer
            -O0
        >
        $<$<CONFIG:Release>:
            -O3
        >
    >
)

# Enable extension hot swapping:
target_compile_definitions(${PROJECT_NAME} PUBLIC
    HOT_RELOAD_ENABLED
)

# Define the directories containing inclusion headers (CMake needs to know this after running add_library):
target_include_directories(${PROJECT_NAME} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

if (NOT APPLE)
    # linker options for the gdextension library
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<NOT:${compiler_is_msvc}>:
            -static-libgcc
            -static-libstdc++
            -Wl,-R,'$$ORIGIN'
        >
    )
endif()

# =======================================================================
# Optional configuration / build features
# =======================================================================

if (USE_CCACHE_FOR_GDEXT_BUILD MATCHES ON)
    include(ccache)
endif()

if (AUTOFORMAT_SRC_ON_CONFIGURE MATCHES ON)
    include(clang-format)
endif()

target_link_libraries(${PROJECT_NAME}
    PUBLIC godot::cpp
    #PRIVATE fmt::fmt
    #PRIVATE fmt::fmt-header-only
    #PRIVATE spdlog::spdlog_header_only
)
# Tip1: PUBLIC: our .dll depends on the library. PRIVATE: the library is expendable.
# Tip2: godot::cpp, fmt::fmt etc... each library name is declared by each of it's
#       respective projects's own CMakeLists.txt, which is reached thanks to vcpkg

# Define system architecture for the build:
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(system_bits 64)
else()
    set(system_bits 32)
endif()

# Define built .dll name:
string(TOLOWER
    "${PROJECT_NAME}.${CMAKE_SYSTEM_NAME}.${system_bits}.${CMAKE_BUILD_TYPE}"
    gde_lib_name
)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    # This option tells CMake to generate position-independent code (PIC). 
    # PIC code can be loaded and executed at any address in memory. 
    # This is necessary when building shared/dynamic libraries.
    POSITION_INDEPENDENT_CODE ON
    # This option tells CMake to export the compile commands for the target. 
    # This can be useful for debugging and profiling purposes.	
    CMAKE_EXPORT_COMPILE_COMMANDS ON
    # This option tells CMake to enable interprocedural optimization for the target. 
    # This can improve the performance of the target by optimizing code across multiple functions.
    CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
    # This option tells CMake to place the static library archive for the target in the specified directory.
    ARCHIVE_OUTPUT_DIRECTORY "${GDEXTENSION_LIB_PATH}"
    # This option tells CMake to place the shared library for the target in the specified directory.
    LIBRARY_OUTPUT_DIRECTORY "${GDEXTENSION_LIB_PATH}"
    # This option tells CMake to place the runtime library for the target in the specified directory.
    RUNTIME_OUTPUT_DIRECTORY "${GDEXTENSION_LIB_PATH}"
    # This option tells CMake to place the program database (PDB) file for the target in the specified directory. 
    # The PDB file contains debugging information for the target.
    CMAKE_PDB_OUTPUT_DIRECTORY "${GDEXTENSION_LIB_PATH}"
    # This option tells CMake to place the compile-time PDB file for the target in the specified directory.
    # The compile-time PDB file contains debugging information that can be used to debug the target while it is being built.
    CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${GDEXTENSION_LIB_PATH}"
    # This option tells CMake to set the output name for the target to the specified value.
    OUTPUT_NAME "${gde_lib_name}"
)

# =======================================================================
# Print configuration report
# =======================================================================

# Include utility script that prints a handful of useful build/configuration cmake variables:
#include(cmake-utils)
#print_project_variables()

