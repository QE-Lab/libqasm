/** \file
 * Defines the types for the cQASM \ref cqasm::v2::ast "abstract syntax tree",
 * based on the classes from cqasm::tree.
 *
 * This file only contains utility stuff on top of the abstract syntax tree
 * structure generated by \ref tree-gen based on `cqasm-ast.tree`. Refer to
 * the cqasm::ast namespace documentation for more information.
 */

#pragma once

#include "cqasm-v2-ast-gen.hpp"
#include <iostream>
#include <sstream>

namespace cqasm {
namespace v2 {
namespace ast {

/**
 * Special/temporary string builder node, used to build strings from fragments
 * and escape sequences while parsing. This is abstracted out of the AST; it
 * should never appear after parsing completes.
 */
class StringBuilder : public cqasm::tree::Base {
public:
    std::ostringstream stream;

    /**
     * Pushes a string fragment into the string.
     */
    void push_string(const std::string &str);

    /**
     * Pushes an escape sequence into the string.
     */
    void push_escape(const std::string &escape);

};

/**
 * Flattens a comma-separated or semicolon-separated unit to make the AST a bit
 * more readable. lhs and rhs represent the left-hand side and right-hand side
 * of the comma or semicolon, of which the latter may be nullptr to represent
 * trailing commas/semicolons. A unit of the specified type will be returned,
 * containing the one-level flattened list of all comma/semicolon-separated
 * elements appearing on the LHS and RHS. That is, when LHS and/or RHS are not
 * already comma/semicolon-separated units, they are added to the result
 * directly; otherwise, the result is extended with their elements.
 *
 * This function assumes ownership of the given new-allocated unit pointers and
 * returns ownership to a new-allocated unit pointer. The returned pointer may
 * be lhs, rhs, or a new unit.
 */
template <class T>
Unit *flatten(Unit *lhs, Unit *rhs = nullptr) {
    if (auto lhs_cast = dynamic_cast<T*>(lhs)) {
        if (!rhs) {
            return lhs;
        } else if (auto rhs_cast = dynamic_cast<T*>(rhs)) {
            lhs_cast->elements.extend(rhs_cast->elements);
            delete rhs;
            return lhs;
        } else {
            lhs_cast->elements.add_raw(rhs);
            return lhs;
        }
    } else {
        if (!rhs) {
            auto retval = new T();
            retval->elements.add_raw(lhs);
            return retval;
        } else if (auto rhs_cast = dynamic_cast<T*>(rhs)) {
            auto retval = new T();
            retval->elements.add_raw(lhs);
            retval->elements.extend(rhs_cast->elements);
            delete rhs;
            return retval;
        } else {
            auto retval = new T();
            retval->elements.add_raw(lhs);
            retval->elements.add_raw(rhs);
            return retval;
        }
    }
}

} // namespace ast
} // namespace v2
} // namespace cqasm