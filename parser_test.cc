#include <memory>
#include <iostream>
#include <sstream>
#include <fstream>

#include "gtest/gtest.h"
#include "error.hh"
#include "scanner.hh"
#include "preprocessor.hh"
#include "parser.hh"

namespace axcc {

class ParserTest : public ::testing::Test {
protected:
    ~ParserTest() {
        // Set the original buffer to std::cerr.
        std::cout.rdbuf(orig_cout_buf_);
        std::cerr.rdbuf(orig_cerr_buf_);
    }
    void InitParser(const std::string& fname) {
        TurnOffColorOutput();
        auto iter = files_.emplace(fname, ReadFile(fname)).first;
        Scanner scanner{iter->first, iter->second};
        tsp_ = scanner.Scan();
        Preprocessor pp{*tsp_, files_, std::list<std::string>{}};
        pp.Preprocess();
        parserp_ = std::make_unique<Parser>(*tsp_);
        // Capture std::cerr and std::cout to check the output.
        orig_cout_buf_ = std::cout.rdbuf(cout_buf_.rdbuf());
        orig_cerr_buf_ = std::cerr.rdbuf(cerr_buf_.rdbuf());
    }

    std::map<std::string, std::string> files_{};
    std::unique_ptr<TokenSequence> tsp_{};
    std::unique_ptr<Parser> parserp_{};
    std::stringstream cout_buf_{};
    std::stringstream cerr_buf_{};
    std::streambuf *orig_cout_buf_{};
    std::streambuf *orig_cerr_buf_{};
};

TEST_F(ParserTest, ObjectDecls) {
    InitParser("obj_decls.c");
    parserp_->Parse().Print(std::cout);
    EXPECT_EQ(cerr_buf_.str(), ReadFile("obj_decls_errs"));
    EXPECT_EQ(cout_buf_.str(), ReadFile("obj_decls_ast"));
}

TEST_F(ParserTest, FuncDecls) {
    InitParser("func_decls.c");
    parserp_->Parse().Print(std::cout);
    EXPECT_EQ(cerr_buf_.str(), ReadFile("func_decls_errs"));
    EXPECT_EQ(cout_buf_.str(), ReadFile("func_decls_ast"));
}

TEST_F(ParserTest, StructDecls) {
    InitParser("structs.c");
    parserp_->Parse().Print(std::cout);
    EXPECT_EQ(cerr_buf_.str(), ReadFile("structs_errs"));
    EXPECT_EQ(cout_buf_.str(), ReadFile("structs_ast"));
}

TEST_F(ParserTest, UnionDecls) {
    InitParser("unions.c");
    parserp_->Parse().Print(std::cout);
    EXPECT_EQ(cerr_buf_.str(), ReadFile("unions_errs"));
    EXPECT_EQ(cout_buf_.str(), ReadFile("unions_ast"));
}

TEST_F(ParserTest, EnumDecls) {
    InitParser("enums.c");
    parserp_->Parse().Print(std::cout);
    EXPECT_EQ(cerr_buf_.str(), ReadFile("enums_errs"));
    EXPECT_EQ(cout_buf_.str(), ReadFile("enums_ast"));
}

TEST_F(ParserTest, ComplicatedTypes) {
    InitParser("complicated_types.c");
    parserp_->Parse().Print(std::cout);
    EXPECT_EQ(cerr_buf_.str(), ReadFile("complicated_types_errs"));
    EXPECT_EQ(cout_buf_.str(), ReadFile("complicated_types_ast"));
}

TEST_F(ParserTest, Initializers) {
    InitParser("initializers.c");
    parserp_->Parse().Print(std::cout);
    EXPECT_EQ(cerr_buf_.str(), ReadFile("initializers_errs"));
    EXPECT_EQ(cout_buf_.str(), ReadFile("initializers_ast"));
}

TEST_F(ParserTest, Statements) {
    InitParser("statements.c");
    parserp_->Parse().Print(std::cout);
    EXPECT_EQ(cerr_buf_.str(), ReadFile("statements_errs"));
    EXPECT_EQ(cout_buf_.str(), ReadFile("statements_ast"));
}

TEST_F(ParserTest, Expressions) {
    InitParser("expressions.c");
    parserp_->Parse().Print(std::cout);
    EXPECT_EQ(cerr_buf_.str(), ReadFile("expressions_errs"));
    EXPECT_EQ(cout_buf_.str(), ReadFile("expressions_ast"));
}

}
