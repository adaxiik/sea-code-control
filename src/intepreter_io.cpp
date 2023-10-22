#include "interpreter_io.hpp"

namespace scc
{
    std::function<void(const char*)> InterpreterIO::stdout_callback = nullptr;
    std::function<void(const char*)> InterpreterIO::stderr_callback = nullptr;

    void InterpreterIO::write_stdout(const char* str)
    {
        if (stdout_callback)
            stdout_callback(str);
        else
            std::cout << str;
    }

    void InterpreterIO::write_stderr(const char* str)
    {
        if (stderr_callback)
            stderr_callback(str);
        else
            std::cerr << str;
    }

    void InterpreterIO::write_stdout(const std::string& str)
    {
        write_stdout(str.c_str());
    }

    void InterpreterIO::write_stderr(const std::string& str)
    {
        write_stderr(str.c_str());
    }

    void InterpreterIO::set_stdout_callback(std::function<void(const char*)> callback)
    {
        stdout_callback = callback;
    }

    void InterpreterIO::set_stderr_callback(std::function<void(const char*)> callback)
    {
        stderr_callback = callback;
    }
}