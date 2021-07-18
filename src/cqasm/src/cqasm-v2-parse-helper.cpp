/** \file
 * Implementation for \ref include/cqasm-v2-parse-helper.hpp "cqasm-v2-parse-helper.hpp".
 */

#include "cqasm-v2-parse-helper.hpp"
#include "cqasm-v2-parser.hpp"
#include "cqasm-v2-lexer.hpp"

namespace cqasm {
namespace v2 {
namespace parser {

/**
 * Parse the given file.
 */
ParseResult parse_file(const std::string &filename) {
    return std::move(ParseHelper(filename, "", true).result);
}

/**
 * Parse using the given file pointer.
 */
ParseResult parse_file(FILE *file, const std::string &filename) {
    return std::move(ParseHelper(filename, file).result);
}

/**
 * Parse the given string. A filename may be given in addition for use within
 * error messages.
 */
ParseResult parse_string(const std::string &data, const std::string &filename) {
    return std::move(ParseHelper(filename, data, false).result);
}

/**
 * Parse a string or file with flex/bison. If use_file is set, the file
 * specified by filename is read and data is ignored. Otherwise, filename
 * is used only for error messages, and data is read instead. Don't use
 * this directly, use parse().
 */
ParseHelper::ParseHelper(
    const std::string &filename,
    const std::string &data,
    bool use_file
) : filename(filename) {

    // Create the scanner.
    if (!construct()) return;

    // Open the file or pass the data buffer to flex.
    if (use_file) {
        fptr = fopen(filename.c_str(), "r");
        if (!fptr) {
            std::ostringstream sb;
            sb << "Failed to open input file " << filename << ": "
               << strerror(errno);
            push_error(sb.str());
            return;
        }
        cqasm_v2set_in(fptr, (yyscan_t)scanner);
    } else {
        buf = cqasm_v2_scan_string(data.c_str(), (yyscan_t)scanner);
    }

    // Do the actual parsing.
    parse();

}

/**
 * Construct the analyzer internals for the given filename, and analyze
 * the file.
 */
ParseHelper::ParseHelper(
    const std::string &filename,
    FILE *fptr
) : filename(filename) {

    // Create the scanner.
    if (!construct()) return;

    // Open the file or pass the data buffer to flex.
    cqasm_v2set_in(fptr, (yyscan_t)scanner);

    // Do the actual parsing.
    parse();

}

/**
 * Initializes the scanner. Returns whether this was successful.
 */
bool ParseHelper::construct() {
    int retcode = cqasm_v2lex_init((yyscan_t*)&scanner);
    if (retcode) {
        std::ostringstream sb;
        sb << "Failed to construct scanner: " << strerror(retcode);
        push_error(sb.str());
        return false;
    } else {
        return true;
    }
}

/**
 * Does the actual parsing.
 */
void ParseHelper::parse() {
    int retcode = cqasm_v2parse((yyscan_t) scanner, *this);
    if (retcode == 2) {
        std::ostringstream sb;
        sb << "Out of memory while parsing " << filename;
        push_error(sb.str());
        return;
    } else if (retcode) {
        std::ostringstream sb;
        sb << "Failed to parse " << filename;
        push_error(sb.str());
        return;
    }
    if (result.errors.empty() && !result.root.is_well_formed()) {
        std::cerr << *result.root;
        throw std::runtime_error("internal error: no parse errors returned, but AST is incomplete. AST was dumped.");
    }
}

/**
 * Destroys the analyzer.
 */
ParseHelper::~ParseHelper() {
    if (fptr) {
        fclose(fptr);
    }
    if (buf) {
        cqasm_v2_delete_buffer((YY_BUFFER_STATE)buf, (yyscan_t)scanner);
    }
    if (scanner) {
        cqasm_v2lex_destroy((yyscan_t)scanner);
    }
}

/**
 * Pushes an error.
 */
void ParseHelper::push_error(const std::string &error) {
    result.errors.push_back(error);
}

} // namespace parser
} // namespace v2
} // namespace cqasm
