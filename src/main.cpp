#include "main.hpp"
#include <godot_cpp/variant/utility_functions.hpp>

void Main::_ready()
{
    godot::UtilityFunctions::print("foo!");
}