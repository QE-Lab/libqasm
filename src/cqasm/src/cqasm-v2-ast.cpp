/** \file
 * Implementation for \ref include/cqasm-v2-ast.hpp "cqasm-v2-ast.hpp".
 */

#include "cqasm-v2-ast.hpp"
#include <stdexcept>

namespace cqasm {
namespace v2 {
namespace ast {

/**
 * Pushes a string fragment into the string.
 */
void StringBuilder::push_string(const std::string &str) {
    stream << str;
}

/**
 * Pushes an escape sequence into the string.
 */
void StringBuilder::push_escape(const std::string &escape) {
    if (escape == "\\t") {
        stream << '\t';
    } else if (escape == "\\n") {
        stream << '\n';
    } else if (escape == "\\r") {
        stream << '\r';
    } else if (escape == "\\'") {
        stream << '\'';
    } else if (escape == "\\\"") {
        stream << '\"';
    } else if (escape == "\\\\") {
        stream << '\\';
    } else {
        stream << escape;
    }
}

} // namespace ast
} // namespace v2
} // namespace cqasm
