#pragma once
#include <functional>
#include <iostream>

namespace scc
{
    class InterpreterIO
    {
        static std::function<void(const char*)> stdout_callback;
        static std::function<void(const char*)> stderr_callback;

    public:
        static void write_stdout(const char* str);

        static void write_stderr(const char* str);

        static void write_stdout(const std::string& str);

        static void write_stderr(const std::string& str);

        static void set_stdout_callback(std::function<void(const char*)> callback);

        static void set_stderr_callback(std::function<void(const char*)> callback);

    };
}