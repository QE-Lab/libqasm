/** \file
 * Implementation for the internal Python-wrapped functions and classes.
 */

#include "cqasm-py.hpp"
#include "cqasm-v1.hpp"
#include "cqasm-v2.hpp"

namespace v1 = cqasm::v1;
namespace v2 = cqasm::v2;

/**
 * Creates a new 1.x semantic analyzer. When without_defaults is specified,
 * the default instruction set and error models are not loaded into the
 * instruction and error model tables, so you have to specify the entire
 * instruction set using register_instruction() and register_error_model();
 * otherwise those functions only add to the defaults. Unlike the C++
 * version of the analyzer class, the initial mappings and functions are not
 * configurable at all; the defaults for these are always used.
 */
V1Analyzer::V1Analyzer(const std::string &max_version, bool without_defaults) {
    if (without_defaults) {
        a = std::unique_ptr<v1::analyzer::Analyzer>(
            new v1::analyzer::Analyzer(max_version)
        );
        a->register_default_functions_and_mappings();
    } else {
        a = std::unique_ptr<v1::analyzer::Analyzer>(
            new v1::analyzer::Analyzer(v1::default_analyzer(max_version))
        );
    }
}

/**
 * Registers an instruction type. The arguments are passed straight to
 * instruction::Instruction's constructor.
 */
void V1Analyzer::register_instruction(
    const std::string &name,
    const std::string &param_types,
    bool allow_conditional,
    bool allow_parallel,
    bool allow_reused_qubits,
    bool allow_different_index_sizes
) {
    a->register_instruction(
        name,
        param_types,
        allow_conditional,
        allow_parallel,
        allow_reused_qubits,
        allow_different_index_sizes
    );
}

/**
 * Registers an error model. The arguments are passed straight to
 * error_model::ErrorModel's constructor.
 */
void V1Analyzer::register_error_model(
    const std::string &name,
    const std::string &param_types
) {
    a->register_error_model(name, param_types);
}

/**
 * Only parses the given file. The file must be in 1.x syntax; no version
 * check or conversion is performed. Returns a vector of strings, of which the
 * first is always present and is the CBOR serialization of the v1.x AST. Any
 * additional strings represent error messages.
 */
std::vector<std::string> V1Analyzer::parse_file(
    const std::string &filename
) {
    auto result = v1::parser::parse_file(filename);
    std::vector<std::string> retval{""};
    if (result.errors.empty()) {
        retval[0] = ::tree::base::serialize(result.root);
    }
    retval.insert(retval.end(), result.errors.begin(), result.errors.end());
    return retval;
}

/**
 * Same as parse_file(), but instead receives the file contents directly.
 * The filename, if specified, is only used when reporting errors.
 */
std::vector<std::string> V1Analyzer::parse_string(
    const std::string &data,
    const std::string &filename
) {
    auto result = v1::parser::parse_string(data, filename);
    std::vector<std::string> retval{""};
    if (result.errors.empty()) {
        retval[0] = ::tree::base::serialize(result.root);
    }
    retval.insert(retval.end(), result.errors.begin(), result.errors.end());
    return retval;
}

/**
 * Parses and analyzes the given file. If the file is written in a later
 * file version, this function may try to reduce it to the maximum 1.x API
 * version support advertised using this object's constructor. Returns a
 * vector of strings, of which the first is always present and is the CBOR
 * serialization of the v1.x semantic tree. Any additional strings represent
 * error messages.
 */
std::vector<std::string> V1Analyzer::analyze_file(
    const std::string &filename
) const {
    auto result = a->analyze(filename);
    std::vector<std::string> retval{""};
    if (result.errors.empty()) {
        retval[0] = ::tree::base::serialize(result.root);
    }
    retval.insert(retval.end(), result.errors.begin(), result.errors.end());
    return retval;
}

/**
 * Same as analyze_file(), but instead receives the file contents directly.
 * The filename, if specified, is only used when reporting errors.
 */
std::vector<std::string> V1Analyzer::analyze_string(
    const std::string &data,
    const std::string &filename
) const {
    auto result = a->analyze_string(data, filename);
    std::vector<std::string> retval{""};
    if (result.errors.empty()) {
        retval[0] = ::tree::base::serialize(result.root);
    }
    retval.insert(retval.end(), result.errors.begin(), result.errors.end());
    return retval;
}

/**
 * Creates a new 2.x semantic analyzer.
 *
 * TODO
 */
V2Analyzer::V2Analyzer() {}

/**
 * Only parses the given file. The file must be in x.x syntax; no version
 * check or conversion is performed. Returns a vector of strings, of which the
 * first is always present and is the CBOR serialization of the vx.x AST. Any
 * additional strings represent error messages.
 */
std::vector<std::string> V2Analyzer::parse_file(
    const std::string &filename
) {
    auto result = v2::parser::parse_file(filename);
    std::vector<std::string> retval{""};
    if (result.errors.empty()) {
        retval[0] = ::tree::base::serialize(result.root);
    }
    retval.insert(retval.end(), result.errors.begin(), result.errors.end());
    return retval;
}

/**
 * Same as parse_file(), but instead receives the file contents directly.
 * The filename, if specified, is only used when reporting errors.
 */
std::vector<std::string> V2Analyzer::parse_string(
    const std::string &data,
    const std::string &filename
) {
    auto result = v2::parser::parse_string(data, filename);
    std::vector<std::string> retval{""};
    if (result.errors.empty()) {
        retval[0] = ::tree::base::serialize(result.root);
    }
    retval.insert(retval.end(), result.errors.begin(), result.errors.end());
    return retval;
}

/**
 * Parses and analyzes the given file. If the file is written in a later or
 * earlier file version, this function may try to convert it. Returns a
 * vector of strings, of which the first is always present and is the CBOR
 * serialization of the v1.x semantic tree. Any additional strings represent
 * error messages.
 */
std::vector<std::string> V2Analyzer::analyze_file(
    const std::string &filename
) const {
    (void)filename;
    return {"", "not yet implemented!"};
}

/**
 * Same as analyze_file(), but instead receives the file contents directly.
 * The filename, if specified, is only used when reporting errors.
 */
std::vector<std::string> V2Analyzer::analyze_string(
    const std::string &data,
    const std::string &filename
) const {
    (void)data;
    (void)filename;
    return {"", "not yet implemented!"};
}
