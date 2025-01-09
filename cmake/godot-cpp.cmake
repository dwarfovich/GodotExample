# =======================================================================
# Godot C++ bindings library setup/configuration
# =======================================================================

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/godot-cpp)
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/extern/godot-cpp/cmake/"
)