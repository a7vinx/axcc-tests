#include <memory>

#include "gtest/gtest.h"
#include "scanner.hh"
// Include cc file instead of header file so we can use private nested class
// of Preprocessor class directly. Is this the best way?
#include "preprocessor.cc"

namespace axcc {

class PreprocessorTest : public ::testing::Test {
protected:
    void InitPreprocessor(const std::string& fname) {
        auto iter = files_.emplace(fname, ReadFile(fname)).first;
        Scanner scanner{iter->first, iter->second};
        tsp_ = scanner.Scan();
        std::list<std::string> header_paths{};
        ppp_ = std::make_unique<Preprocessor>(*tsp_, files_, header_paths);
    }
    // Accessors of private functions of class Scanner
    bool AddMacro(const Preprocessor::Macro& macro) {
        return ppp_->AddMacro(macro); }
    void RmMacro(const std::string& ident) {
        ppp_->RmMacro(ident); }
    bool HasMacro(const std::string& ident) const {
        return ppp_->HasMacro(ident); }
    const Preprocessor::Macro* GetMacro(const std::string& ident) const {
        return ppp_->GetMacro(ident); }
    std::string FindHeader(const std::string& fname,
                           bool include_cur_path,
                           const std::string& cur_path) {
        return ppp_->FindHeader(fname, include_cur_path, cur_path); }

    std::map<std::string, std::string> files_{};
    std::unique_ptr<TokenSequence> tsp_{};
    std::unique_ptr<Preprocessor> ppp_{};
};

TEST_F(PreprocessorTest, MacrosOperation) {
    InitPreprocessor("testfile1.c");

    // test AddMacro()
    std::list<Token> repl;
    repl.emplace_back(TokenType::STRING, "a");
    repl.emplace_back(TokenType::STRING, "b");
    EXPECT_FALSE(AddMacro({"m1", repl}));
    EXPECT_FALSE(AddMacro({"m1", repl}));
    repl.emplace_back(TokenType::STRING, "c");
    // Now it should be a redefinition.
    EXPECT_TRUE(AddMacro({"m1", repl}));
    std::vector<std::string> params{"param1", "param2"};
    EXPECT_FALSE(AddMacro({"m2", repl, params, false}));
    EXPECT_FALSE(AddMacro({"m2", repl, params, false}));
    // it should be a redefinition.
    EXPECT_TRUE(AddMacro({"m2", repl, params, true}));

    // test HasMacro()
    EXPECT_TRUE(HasMacro("m1"));
    EXPECT_TRUE(HasMacro("m2"));
    EXPECT_FALSE(HasMacro("m3"));

    // test RmMacro()
    RmMacro("m1");
    EXPECT_FALSE(HasMacro("m1"));
    EXPECT_TRUE(HasMacro("m2"));

    // test GetMacro()
    EXPECT_EQ(GetMacro("m1"), nullptr);
    EXPECT_NE(GetMacro("m2"), nullptr);
}

TEST_F(PreprocessorTest, FindHeader) {
    InitPreprocessor("testfile1.c");
    EXPECT_EQ(FindHeader("file_not_exist.cc", false, ""), "");
    EXPECT_EQ(FindHeader("file_not_exist.cc", true,
                         "../testfiles/testfile1.c"), "");
    EXPECT_EQ(FindHeader("idents.c", true,
                         "../testfiles/testfile1.c"), "../testfiles/idents.c");
    EXPECT_EQ(FindHeader("idents.c", true, "testfile1.c"), "idents.c");
    EXPECT_EQ(FindHeader("stdio.h", false, ""), "/usr/include/stdio.h");
    // It should only search for a regular file.
    EXPECT_EQ(FindHeader("testfiles", true, "../preprocessor_test.cc"), "");
    // It should simplify the path.
    EXPECT_EQ(FindHeader("idents.c", true,
                         "../testfiles/../testfiles/testfile1.c"),
                         "../testfiles/idents.c");
    EXPECT_EQ(FindHeader("idents.c", true, "./testfile1.c"), "./idents.c");
}

bool ExpectTokenSequence(TokenSequence& ts, const std::list<Token>& tl) {
    Token* tp = ts.Begin();
    for (auto iter = tl.cbegin(); iter != tl.cend(); ++iter) {
        EXPECT_EQ(*tp, *iter);
        tp = ts.Next();
        if (IsEndToken(*tp) && std::next(iter, 1) != tl.cend())
            return false;
    }
    if (!IsEndToken(*tp))
        return false;
    return true;
}

void ExpectErrorList(std::stringstream& buf,
                     const std::vector<std::string>& err_vec) {
    std::string line;
    for (auto iter = err_vec.cbegin(); iter != err_vec.cend(); ++iter) {
        std::getline(buf, line);
        EXPECT_NE(line.find(*iter), std::string::npos);
        ++iter;
        std::getline(buf, line);
        EXPECT_EQ(line, *iter);
        std::getline(buf, line);
    }
}

TEST_F(PreprocessorTest, HandlePPDirectives) {
    InitPreprocessor("directives.c");
    // Capture std::cerr to check the output
    std::stringstream output_buf;
    std::streambuf *orig_buf = std::cerr.rdbuf(output_buf.rdbuf());
    // Perform preprocessing.
    ppp_->Preprocess();
    // Check the token sequence that has been preprocessed.
    std::list<Token> preprocessed{
        {TokenType::VOID, "void"},
        {TokenType::IDENTIFIER, "f2"},
        {TokenType::LPAR, "("},
        {TokenType::RPAR, ")"},
        {TokenType::SEMI, ";"}
    };
    EXPECT_TRUE(ExpectTokenSequence(*tsp_, preprocessed));
    // Check the status of macros in preprocessor.
    EXPECT_FALSE(HasMacro("obj_macro1"));
    EXPECT_TRUE(HasMacro("obj_macro2"));
    EXPECT_FALSE(HasMacro("func_macro1"));
    EXPECT_TRUE(HasMacro("func_macro2"));
    EXPECT_FALSE(HasMacro("obj_macro_ifdef_true"));
    EXPECT_TRUE(HasMacro("obj_macro_ifdef_false"));
    // Check the output of std::cerr
    std::vector<std::string> err_vec{
        "error in directives_include.h",
        "new_name_in_include:202:2: #error error in directives_include.h",
        "'file_not_exist' file not found",
        "directives.c:4:10: #include \"file_not_exist\"",
        "'\\\"file_not_exist_in_quotes\\\"' file not found",
        "directives.c:5:10: #include \"\\\"file_not_exist_in_quotes\\\"\"",
        "expected \"FILENAME\" or <FILENAME>",
        "directives.c:7:10: #include wrong_token",
        "macro name must be an identifier",
        "directives.c:15:9: #define @",
        "defined' cannot be used as a macro name",
        "directives.c:17:9: #define defined",
        "missing ')' in macro parameter list",
        "directives.c:19:29: #define func_macro_wrong(p1,",
        "expected identifier in macro parameter list",
        "directives.c:21:26: #define func_macro_wrong(@, +) something",
        "expected comma in macro parameter list",
        "directives.c:23:28: #define func_macro_wrong(p1+) something",
        "#else after #else",
        "directives.c:41:2: #else",
        "#else without #if",
        "directives.c:44:2: #else",
        "#endif without #if",
        "directives.c:46:2: #endif",
        "macro name missing",
        "directives.c:48:7: #ifdef",
        "macro name must be an identifier",
        "directives.c:51:8: #ifdef @",
        "extra tokens at end of directive",
        "directives.c:54:24: #ifdef macro_not_exist extra tokens",
        "invalid filename for #line directive",
        "\"name_in_quotes\":102:11: #line 100 not_a_name",
        "#line directive requires a positive integer argument",
        "\"name_in_quotes\":104:7: #line -100",
        "#line directive requires a simple digit sequence",
        "\"name_in_quotes\":106:7: #line 9f",
        "",
        "\"name_in_quotes\":109:2: #error",
        "this is an \"error\"",
        "\"name_in_quotes\":110:2: #error this is an \"error\"",
        "unterminated conditional directive",
        "directives.c:57:8: #ifdef obj_macro2"
    };
    ExpectErrorList(output_buf, err_vec);
    // Set the original buffer to std::cerr
    std::cerr.rdbuf(orig_buf);
}

}
