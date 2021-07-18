/** \file
 * Defines SWIG'd things for the Python interface. This is just the *internal*
 * interface to the library; the user-facing API is written in pure-Python and
 * wraps around this.
 */

#pragma once

// Don't include any libqasm headers! We don't want SWIG to generate Python
// wrappers for the entire world. Those headers are only included in the source
// file that provides the implementations.
#include <string>
#include <memory>
#include <vector>

// Forward declarations for internal types.
namespace cqasm {
namespace v1 {
namespace analyzer {
class Analyzer;
} // namespace analyzer
} // namespace v1
} // namespace cqasm

/**
 * Main class for parsing and analyzing cQASM files with the v1 API.
 */
class V1Analyzer {
private:

    /**
     * Reference to the actual C++ analyzer that this wraps.
     */
    std::unique_ptr<cqasm::v1::analyzer::Analyzer> a;

public:

    /**
     * Creates a new 1.x semantic analyzer. When without_defaults is specified,
     * the default instruction set and error models are not loaded into the
     * instruction and error model tables, so you have to specify the entire
     * instruction set using register_instruction() and register_error_model();
     * otherwise those functions only add to the defaults. Unlike the C++
     * version of the analyzer class, the initial mappings and functions are not
     * configurable at all; the defaults for these are always used.
     */
    V1Analyzer(const std::string &max_version = "1.0", bool without_defaults=false);

    /**
     * Registers an instruction type. The arguments are passed straight to
     * instruction::Instruction's constructor.
     */
    void register_instruction(
        const std::string &name,
        const std::string &param_types = "",
        bool allow_conditional = true,
        bool allow_parallel = true,
        bool allow_reused_qubits = false,
        bool allow_different_index_sizes = false
    );

    /**
     * Registers an error model. The arguments are passed straight to
     * error_model::ErrorModel's constructor.
     */
    void register_error_model(
        const std::string &name,
        const std::string &param_types = ""
    );

    /**
     * Only parses the given file. The file must be in 1.x syntax; no version
     * check or conversion is performed. Returns a vector of strings, of which
     * the first is always present and is the CBOR serialization of the v1.x
     * AST. Any additional strings represent error messages.
     */
    static std::vector<std::string> parse_file(
        const std::string &filename
    );

    /**
     * Same as parse_file(), but instead receives the file contents directly.
     * The filename, if specified, is only used when reporting errors.
     */
    static std::vector<std::string> parse_string(
        const std::string &data,
        const std::string &filename = "<unknown>"
    );

    /**
     * Parses and analyzes the given file. If the file is written in a later
     * file version, this function may try to reduce it to the maximum 1.x API
     * version support advertised using this object's constructor. Returns a
     * vector of strings, of which the first is always present and is the CBOR
     * serialization of the v1.x semantic tree. Any additional strings represent
     * error messages.
     */
    std::vector<std::string> analyze_file(
        const std::string &filename
    ) const;

    /**
     * Same as analyze_file(), but instead receives the file contents directly.
     * The filename, if specified, is only used when reporting errors.
     */
    std::vector<std::string> analyze_string(
        const std::string &data,
        const std::string &filename = "<unknown>"
    ) const;

};

/**
 * Main class for parsing and analyzing cQASM files with the v2 API.
 */
class V2Analyzer {
public:

    /**
     * Creates a new 2.x semantic analyzer.
     *
     * TODO
     */
    V2Analyzer();

    /**
     * Only parses the given file. The file must be in x.x syntax; no version
     * check or conversion is performed. Returns a vector of strings, of which
     * the first is always present and is the CBOR serialization of the vx.x
     * AST. Any additional strings represent error messages.
     */
    static std::vector<std::string> parse_file(
        const std::string &filename
    );

    /**
     * Same as parse_file(), but instead receives the file contents directly.
     * The filename, if specified, is only used when reporting errors.
     */
    static std::vector<std::string> parse_string(
        const std::string &data,
        const std::string &filename = "<unknown>"
    );

    /**
     * Parses and analyzes the given file. If the file is written in a later or
     * earlier file version, this function may try to convert it. Returns a
     * vector of strings, of which the first is always present and is the CBOR
     * serialization of the v1.x semantic tree. Any additional strings represent
     * error messages.
     */
    std::vector<std::string> analyze_file(
        const std::string &filename
    ) const;

    /**
     * Same as analyze_file(), but instead receives the file contents directly.
     * The filename, if specified, is only used when reporting errors.
     */
    std::vector<std::string> analyze_string(
        const std::string &data,
        const std::string &filename = "<unknown>"
    ) const;

};
