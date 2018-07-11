/** This test is for an integrated file: It is not a particualr algorithm **/
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TESTQC
#include <iostream>
#include <vector>
#include <string>
#include <boost/test/unit_test.hpp>
#include "qasm_semantic.hpp"


struct testqc
{
    testqc()
    {
    }
    
    void TestMapping(compiler::QasmRepresentation& qasm)
    {
        std::string first_key = "bleh2";
        auto indices = qasm.getMappedIndices(first_key, true);
        std::vector<size_t> qubit_indices = indices.getIndices();
        std::vector<size_t> correct_result = {0,1,2,3,4,5,6,7};
        BOOST_CHECK(qubit_indices == correct_result);
    }

};

BOOST_FIXTURE_TEST_SUITE(Test_Valid, testqc);

BOOST_AUTO_TEST_CASE(testMapping)
{
    #if YYDEBUG == 1
    extern int yydebug;
    yydebug = 1;
    #endif

    // open a file handle to a particular file:
    FILE *myfile = fopen("testqc.qasm", "r");
    
    compiler::QasmSemanticChecker sm(myfile);

    auto qasm_representation = sm.getQasmRepresentation();
    
    int result = sm.parseResult();

    BOOST_REQUIRE(result == 0);   // Stop here if it fails.
    TestMapping(qasm_representation);
}

BOOST_AUTO_TEST_SUITE_END();