#include <gtest/gtest.h> // googletest header file
#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>
#include "dirent-compat.h"

#include <cqasm.hpp>

/**
 * Reads the given file into the given string buffer and returns true if it
 * exists, otherwise do nothing with the buffer and return false.
 */
bool read_file(const std::string &filename, std::string &output) {
    std::ifstream stream(filename);
    if (!stream.is_open()) {
        return false;
    }
    output.clear();
    stream.seekg(0, std::ios::end);
    output.reserve(stream.tellg());
    stream.seekg(0, std::ios::beg);
    output.assign(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
    return true;
}

/**
 * Overwrites or creates the given file with the given string.
 */
void write_file(const std::string &filename, const std::string &input) {
    std::ofstream stream(filename);
    stream << input;
}

/**
 * Tests the default parsing pipeline, varying only the input files, and
 * matching debug dumps of the output against golden versions.
 */
class ParsingTest : public ::testing::Test {
private:
    std::string path;

public:
    explicit ParsingTest(const std::string &path) : path(path) {}

    void TestBody() override {

        // Parse the test input file.
        std::string input;
        ASSERT_TRUE(read_file(path + "/input.cq", input));
        cqasm::v2::parser::ParseResult parse_result;
        auto version = cqasm::version::parse_string(input, "input.cq");
        if (version > cqasm::version::Version("2.0") || version < cqasm::version::Version("2.0")) {
            std::ostringstream ss;
            ss << "detected version " << version;
            parse_result.errors.push_back(ss.str());
        } else {
            parse_result = cqasm::v2::parser::parse_string(input, "input.cq");
        }

        // Check the parse result.
        std::ostringstream ss;
        if (parse_result.errors.empty()) {
            ss << "SUCCESS" << std::endl;
            ss << *parse_result.root << std::endl;
        } else {
            ss << "ERROR" << std::endl;
            for (const auto &error : parse_result.errors) {
                ss << error << std::endl;
            }
        }
        std::string ast_result = ss.str();
        write_file(path + "/ast.actual.txt", ast_result);
        std::string ast_golden;
        EXPECT_TRUE(read_file(path + "/ast.golden.txt", ast_golden));
        EXPECT_TRUE(ast_result == ast_golden);

        // TODO: semantic analysis

    }

};

/**
 * Entry point.
 */
int main(int argc, char** argv) {
    testing::InitGoogleTest();

    // Discover the tests. They should live in a directory tree with the
    // following structure:
    //
    // <CWD>
    //  '- v2-parsing
    //      |- <suite-name>                   test suite directory
    //      |   |- <test-name>                test case directory
    //      |   |   |- input.cq               the input file
    //      |   |   |- ast.golden             the golden AST or parse error dump
    //      |   |   |- [ast.actual.txt]       output file with the actual data
    //      |   |   |- [semantic.golden.txt]  the golden semantic tree or
    //      |   |   |                           analysis error dump, if parsing
    //      |   |   |                           should succeed
    //      |   |   '- [semantic.actual.txt]  output file with the actual data,
    //      |   |                               if parsing actually succeeded
    //      |   |- ...                        other test case directories
    //      |   :
    //      |- ...                            other test suite directories
    //      :
    DIR *parsing_dir = opendir("v2-parsing");
    if (!parsing_dir) {
        throw std::runtime_error("failed to open dir for parsing tests");
    }
    while (dirent *parsing_dir_ent = readdir(parsing_dir)) {
        if (parsing_dir_ent->d_name[0] == '.') {
            continue;
        }
        auto suite_name = std::string(parsing_dir_ent->d_name);
        auto suite_path = "v2-parsing/" + suite_name;
        DIR *suite_dir = opendir(suite_path.c_str());
        if (!suite_dir) {
            continue;
        }
        while (dirent *suite_dir_ent = readdir(suite_dir)) {
            if (suite_dir_ent->d_name[0] == '.') {
                continue;
            }
            auto test_name = std::string(suite_dir_ent->d_name);
            auto test_path = suite_path + "/";
            test_path += test_name;
            {
                std::ifstream stream(test_path + "/input.cq");
                if (!stream.is_open()) {
                    continue;
                }
            }
            ::testing::RegisterTest(
                suite_name.c_str(), test_name.c_str(),
                nullptr, nullptr,
                __FILE__, __LINE__,
                [=]() -> ParsingTest* { return new ParsingTest(test_path); });
        }
        closedir(suite_dir);
    }
    closedir(parsing_dir);

    return RUN_ALL_TESTS();
}
