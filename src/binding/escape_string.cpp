#include "binding/binder.hpp"

namespace scc
{
    /**
     * @brief Escapes a string literal
     * @warning It removes the first and last character as well
     *
     * @param str
     * @return std::vector<char>
     */
    std::vector<char> Binder::escape_string(const std::string& str)
    {

        std::vector<char> result;
        result.reserve(str.size());
        for (size_t i = 1; i < str.size()-1; i++) // skip first and last quote
        {
            if (str[i] == '\\')
            {
                switch (str[i + 1])
                {
                case 'n':
                    result.push_back('\n');
                    break;
                case 't':
                    result.push_back('\t');
                    break;
                case 'r':
                    result.push_back('\r');
                    break;
                case '0':
                    result.push_back('\0');
                    break;
                case '\\':
                    result.push_back('\\');
                    break;
                case '\'':
                    result.push_back('\'');
                    break;
                case '\"':
                    result.push_back('\"');
                    break;
                default:
                    // TODO: string escape sequences
                    // there are more, but we dont support them yet
                    // https://en.wikipedia.org/wiki/Escape_sequences_in_C
                    break;
                }
                i++;
                continue;
            }

            result.push_back(str[i]);
        }
        result.push_back('\0');
        return result;
    }
}