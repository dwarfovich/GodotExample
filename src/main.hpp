#pragma once

#include <godot_cpp/classes/node.hpp>

class Main : public godot::Node
{
    GDCLASS(Main, godot::Node);

public:
    Main() = default;
    ~Main() = default;

    void _ready() override;

protected:
    // static void _bind_methods() { signal_binding<Main, event::signal_example>::add<double>(); }
protected:
    static void _bind_methods() {}


private:
};