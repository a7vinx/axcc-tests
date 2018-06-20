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

}
