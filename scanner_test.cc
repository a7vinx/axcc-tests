#include <memory>

#include "scanner.hh"
#include "gtest/gtest.h"

namespace axcc {

TEST(FileExistTest, FileExist) {
    EXPECT_TRUE(FileExist("../scanner_test.cc"));
    EXPECT_FALSE(FileExist("scanner_test_not_exist.cc"));
    EXPECT_FALSE(FileExist("../testfiles"));
    EXPECT_TRUE(FileExist("../testfiles/testfile1.c"));
}

TEST(ReadFileTest, ReadFile) {
    EXPECT_TRUE(ReadFile("file_not_exist").empty());
    EXPECT_EQ(ReadFile("testfile1.c"), "int main() {\nreturn 0;\n}\n");
}

class ScannerTest : public ::testing::Test {
protected:
    void InitScanner(const std::string& fname) {
        fname_ = fname;
        fcontent_ = ReadFile(fname_);
        scp_ = std::move(std::make_unique<Scanner>(fname_, fcontent_));
    }
    // Accessors of private data members of class Scanner
    TokenSequence* TS() const { return scp_->tsp_.get(); }
    unsigned int CurRow() const { return scp_->cur_row_; }
    unsigned int CurColumn() const { return scp_->cur_column_; }
    std::string::const_iterator CurLinePtr() const { return scp_->cur_linep_; }
    std::string::const_iterator CurCharPtr() const { return scp_->cur_charp_; }
    std::size_t CurLineLen() const { return scp_->cur_line_len_; }
    // Accessors of private functions of class Scanner
    char Begin() const { return scp_->Begin(); }
    char CurChar() const { return scp_->CurChar(); }
    char Next() { return scp_->Next(); }
    char NextN(int n) { return scp_->NextN(n); }
    char LookAhead() const { return scp_->LookAhead(); }
    char LookAheadN(int n) const { return scp_->LookAheadN(n); }
    bool NextIs(char c) const { return scp_->NextIs(c); }
    bool Try(char c) { return scp_->Try(c); }
    bool Try(const std::string& chars) { return scp_->Try(chars); }
    unsigned int FindNextNewline() { return scp_->FindNextNewline(); }
    void MakeTokenInTS(const TokenType& tag, const std::string& token_str) {
        return scp_->MakeTokenInTS(tag, token_str);
    }
    void MakeTokenInTS(const TokenType& tag) { return scp_->MakeTokenInTS(tag); }

    std::string fname_{};
    std::string fcontent_{};
    std::unique_ptr<Scanner> scp_{};
};

TEST_F(ScannerTest, FindNextNewline) {
    InitScanner("testfile1.c");
    EXPECT_EQ(FindNextNewline(), 12);
    NextN(13);
    EXPECT_EQ(FindNextNewline(), 9);
    NextN(11);
    EXPECT_EQ(FindNextNewline(), 0);

}

TEST_F(ScannerTest, Traverse) {
    InitScanner("testfile1.c");
    EXPECT_EQ(CurLineLen(), 12);
    EXPECT_EQ(*CurLinePtr(), 'i');
    EXPECT_EQ(*CurCharPtr(), 'i');

    // Begin()
    EXPECT_EQ(Begin(), 'i');
    EXPECT_EQ(CurRow(), 1);
    EXPECT_EQ(CurColumn(), 1);

    // LookAhead()
    EXPECT_EQ(LookAhead(), 'n');
    EXPECT_EQ(*CurCharPtr(), 'i');
    EXPECT_EQ(*CurLinePtr(), 'i');

    // Next()
    EXPECT_EQ(Next(), 'n');
    EXPECT_EQ(*CurCharPtr(), 'n');
    EXPECT_EQ(*CurLinePtr(), 'i');
    EXPECT_EQ(CurLineLen(), 12);
    EXPECT_EQ(CurRow(), 1);
    EXPECT_EQ(CurColumn(), 2);

    // CurChar()
    EXPECT_EQ(CurChar(), 'n');

    // NextIs()
    EXPECT_TRUE(NextIs('t'));
    EXPECT_FALSE(NextIs('f'));

    // Move to '{'
    for (int i = 2; i < 12; i++)
        Next();

    // LookAheadN()
    EXPECT_EQ(LookAheadN(1), '\n');
    EXPECT_EQ(*CurCharPtr(), '{');
    EXPECT_EQ(*CurLinePtr(), 'i');
    EXPECT_EQ(CurLineLen(), 12);
    EXPECT_EQ(CurRow(), 1);
    EXPECT_EQ(CurColumn(), 12);
    EXPECT_EQ(LookAheadN(3), 'e');
    EXPECT_EQ(*CurCharPtr(), '{');
    EXPECT_EQ(*CurLinePtr(), 'i');
    EXPECT_EQ(CurLineLen(), 12);
    EXPECT_EQ(CurRow(), 1);
    EXPECT_EQ(CurColumn(), 12);

    // Next()
    EXPECT_EQ(Next(), '\n');
    EXPECT_EQ(*CurCharPtr(), '\n');
    EXPECT_EQ(*CurLinePtr(), 'i');
    EXPECT_EQ(CurLineLen(), 12);
    EXPECT_EQ(CurRow(), 1);
    EXPECT_EQ(CurColumn(), 13);
    EXPECT_EQ(Next(), 'r');
    EXPECT_EQ(*CurCharPtr(), 'r');
    EXPECT_EQ(*CurLinePtr(), 'r');
    EXPECT_EQ(CurLineLen(), 9);
    EXPECT_EQ(CurRow(), 2);
    EXPECT_EQ(CurColumn(), 1);

    // bool Try(char);
    EXPECT_TRUE(Try('e'));
    EXPECT_EQ(*CurCharPtr(), 'e');
    EXPECT_EQ(*CurLinePtr(), 'r');
    EXPECT_EQ(CurLineLen(), 9);
    EXPECT_EQ(CurRow(), 2);
    EXPECT_EQ(CurColumn(), 2);
    EXPECT_FALSE(Try('f'));
    EXPECT_EQ(*CurCharPtr(), 'e');
    EXPECT_EQ(*CurLinePtr(), 'r');
    EXPECT_EQ(CurLineLen(), 9);
    EXPECT_EQ(CurRow(), 2);
    EXPECT_EQ(CurColumn(), 2);

    // bool Try(const std::string&);
    EXPECT_TRUE(Try("tur"));
    EXPECT_EQ(*CurCharPtr(), 'r');
    EXPECT_EQ(*CurLinePtr(), 'r');
    EXPECT_EQ(CurLineLen(), 9);
    EXPECT_EQ(CurRow(), 2);
    EXPECT_EQ(CurColumn(), 5);
    EXPECT_FALSE(Try("n8"));
    EXPECT_EQ(*CurCharPtr(), 'r');
    EXPECT_EQ(*CurLinePtr(), 'r');
    EXPECT_EQ(CurLineLen(), 9);
    EXPECT_EQ(CurRow(), 2);
    EXPECT_EQ(CurColumn(), 5);
}

// Traverse with backslash-newline
TEST_F(ScannerTest, TraverseWithBSNL) {
    InitScanner("testfile1_bsnl.c");
    EXPECT_EQ(CurLineLen(), 1);
    EXPECT_EQ(*CurLinePtr(), '\\');
    EXPECT_EQ(*CurCharPtr(), '\\');

    // Begin()
    EXPECT_EQ(Begin(), 'i');
    EXPECT_EQ(CurRow(), 2);
    EXPECT_EQ(CurColumn(), 1);
    NextN(12);
    EXPECT_EQ(*CurCharPtr(), ' ');
    // LookAhead()
    EXPECT_EQ(LookAhead(), 'r');
    EXPECT_EQ(*CurCharPtr(), ' ');
    // LookAheadN()
    EXPECT_EQ(LookAheadN(6), 'n');
    EXPECT_EQ(LookAheadN(11), '}');
    EXPECT_EQ(LookAheadN(12), '\n');
    EXPECT_EQ(LookAheadN(13), 0);
    // Next()
    EXPECT_EQ(Next(), 'r');
    EXPECT_EQ(*CurCharPtr(), 'r');
    EXPECT_EQ(*CurLinePtr(), 'r');
    EXPECT_EQ(CurLineLen(), 5);
    EXPECT_EQ(CurRow(), 3);
    EXPECT_EQ(CurColumn(), 1);
    // bool Try(const std::string&);
    EXPECT_TRUE(Try("eturn"));
    EXPECT_EQ(*CurCharPtr(), 'n');
    EXPECT_EQ(*CurLinePtr(), 'r');
    EXPECT_EQ(CurLineLen(), 5);
    EXPECT_EQ(CurRow(), 4);
    EXPECT_EQ(CurColumn(), 2);
    // Next()
    NextN(3);
    EXPECT_EQ(Next(), '\n');
    // bool Try(char c);
    EXPECT_TRUE(Try('}'));
    EXPECT_EQ(*CurCharPtr(), '}');
    EXPECT_EQ(*CurLinePtr(), '}');
    EXPECT_EQ(CurLineLen(), 1);
    EXPECT_EQ(CurRow(), 6);
    EXPECT_EQ(CurColumn(), 1);
    EXPECT_EQ(Next(), '\n');
    EXPECT_EQ(Next(), 0);
}

TEST_F(ScannerTest, MakeTokenInTS) {
    InitScanner("testfile1.c");
    MakeTokenInTS(TokenType::INT);
    for (int i = 0; i < 4; i++)
        Next();
    MakeTokenInTS(TokenType::IDENTIFIER, "main");

    // Check the first token
    Token* tp = TS()->Begin();
    EXPECT_EQ(tp->Tag(), TokenType::INT);
    EXPECT_EQ(tp->TokenStr(), "int");

    // Check the second token
    tp = TS()->Next();
    EXPECT_EQ(tp->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tp->TokenStr(), "main");
    EXPECT_EQ(*(tp->LocPtr()->fnamep), "testfile1.c");
    EXPECT_EQ(tp->LocPtr()->row, 1);
    EXPECT_EQ(tp->LocPtr()->column, 5);
    EXPECT_EQ(*(tp->LocPtr()->linep), 'i');
    EXPECT_EQ(tp->LocPtr()->line_len, 12);
}

TEST_F(ScannerTest, ScanPunctuator) {
    InitScanner("punctuators.c");
    std::unique_ptr<TokenSequence> tsp = scp_->Scan();
    EXPECT_EQ(tsp->Begin()->Tag(), TokenType::LPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    // Do a comprehensive check
    Token* tp = tsp->Next();
    EXPECT_EQ(tp->Tag(), TokenType::LBRACE);
    EXPECT_EQ(tp->TokenStr(), "{");
    EXPECT_EQ(*(tp->LocPtr()->fnamep), "punctuators.c");
    EXPECT_EQ(tp->LocPtr()->row, 2);
    EXPECT_EQ(tp->LocPtr()->column, 1);
    EXPECT_EQ(*(tp->LocPtr()->linep), '{');
    EXPECT_EQ(tp->LocPtr()->line_len, 2);
    // Continue
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LSBRACKET);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RSBRACKET);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::QUES);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::TILDE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::COMMA);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::ELLIP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::DOT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LSBRACKET);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::COLON);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEQ);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::EXCL);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::MUL_ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::AST);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::MOD_ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::DSHARP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SHARP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::PCT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LOGICAL_AND);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::AND_ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::AMP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LOGICAL_OR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::OR_ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::VBAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::XOR_ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::CARET);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::EQ);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INC);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::ADD_ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::PLUS);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::DEC);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::ARROW);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SUB_ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::MINUS);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::DSHARP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SHARP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SHL_ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SHL);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LESS_EQ);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LSBRACKET);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LABRACKET);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SHR_ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SHR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::GREATER_EQ);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RABRACKET);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::DIV_ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SLASH);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INVALID);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INVALID);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::END);
}

TEST_F(ScannerTest, SkipComment) {
    InitScanner("comments.c");
    std::unique_ptr<TokenSequence> tsp = scp_->Scan();
    EXPECT_EQ(tsp->Begin()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::PLUS);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::PLUS);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SLASH);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::END);
}

void ExpectTokenAndNewline(TokenSequence& ts, TokenType type,
                           const std::string& token_str) {
    Token* tp = ts.Next();
    EXPECT_EQ(tp->Tag(), type);
    EXPECT_EQ(tp->TokenStr(), token_str);
    EXPECT_EQ(ts.Next()->Tag(), TokenType::NEWLINE);
}

TEST_F(ScannerTest, ScanNumConstant) {
    InitScanner("num_constants.c");
    std::unique_ptr<TokenSequence> tsp = scp_->Scan();
    auto expect_i_constant_and_newline =
        [&ts = *tsp](const std::string& token_str) {
            ExpectTokenAndNewline(ts, TokenType::I_CONSTANT, token_str);
    };
    auto expect_f_constant_and_newline =
        [&ts = *tsp](const std::string& token_str) {
            ExpectTokenAndNewline(ts, TokenType::F_CONSTANT, token_str);
    };
    // Valid integer constant
    EXPECT_EQ(tsp->Begin()->Tag(), TokenType::NEWLINE);
    expect_i_constant_and_newline("8");
    expect_i_constant_and_newline("11");
    expect_i_constant_and_newline("9999234567");
    expect_i_constant_and_newline("1l");
    expect_i_constant_and_newline("2ull");
    expect_i_constant_and_newline("0x3LL");
    expect_i_constant_and_newline("0x0230");
    expect_i_constant_and_newline("0x3F");
    expect_i_constant_and_newline("0x1e2");
    expect_i_constant_and_newline("0x12f");
    expect_i_constant_and_newline("0707");
    // Invalid integer constant
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_i_constant_and_newline("2lL");
    expect_i_constant_and_newline("2Ll");
    expect_i_constant_and_newline("1d2");
    expect_i_constant_and_newline("1p2");
    expect_i_constant_and_newline("1gpp");
    expect_i_constant_and_newline("11_");
    expect_i_constant_and_newline("1_.p");
    expect_i_constant_and_newline("1d.2");
    expect_i_constant_and_newline("1p.2");
    expect_i_constant_and_newline("0x4G4");
    expect_i_constant_and_newline("0x1g0x2");
    expect_i_constant_and_newline("0X1g.0x2");
    expect_i_constant_and_newline("0x1g.0.");
    // 0x/1
    Token* tp = tsp->Next();
    EXPECT_EQ(tp->Tag(), TokenType::I_CONSTANT);
    EXPECT_EQ(tp->TokenStr(), "0x");
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SLASH);
    expect_i_constant_and_newline("1");
    expect_i_constant_and_newline("0xge.1");
    expect_i_constant_and_newline("0908");
    expect_i_constant_and_newline("0f1g");
    // Valid floating constant
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_f_constant_and_newline("1.1");
    expect_f_constant_and_newline(".29");
    expect_f_constant_and_newline("4.");
    expect_f_constant_and_newline("5.f");
    expect_f_constant_and_newline("1.3f");
    expect_f_constant_and_newline("44677.22l");
    expect_f_constant_and_newline("1e2");
    expect_f_constant_and_newline("1.3e2");
    expect_f_constant_and_newline("22.e+0F");
    expect_f_constant_and_newline(".1e+2");
    expect_f_constant_and_newline(".2e-3");
    expect_f_constant_and_newline("0x1.1e2");
    expect_f_constant_and_newline("0X1p2");
    expect_f_constant_and_newline("0x1.3p-2");
    // Invalid floating constant
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_f_constant_and_newline("7.2fl");
    expect_f_constant_and_newline("1ef");
    expect_f_constant_and_newline("1e.4");
    expect_f_constant_and_newline("1.d.2");
    expect_f_constant_and_newline("1.5g2");
    expect_f_constant_and_newline("322.p2");
    expect_f_constant_and_newline(".9P2");
    expect_f_constant_and_newline(".9.2");
    expect_f_constant_and_newline("0x.g4");
    expect_f_constant_and_newline("0x7pf3");
    expect_f_constant_and_newline("0x.3");
    expect_f_constant_and_newline("0x3.");
    // 0x1.ep++2
    tp = tsp->Next();
    EXPECT_EQ(tp->Tag(), TokenType::F_CONSTANT);
    EXPECT_EQ(tp->TokenStr(), "0x1.ep+");
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::PLUS);
    expect_i_constant_and_newline("2");
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::END);
}

TEST_F(ScannerTest, ScanCharConstant) {
    InitScanner("char_constants.c");
    std::unique_ptr<TokenSequence> tsp = scp_->Scan();
    auto expect_c_constant_and_newline =
        [&ts = *tsp](const std::string& token_str) {
            ExpectTokenAndNewline(ts, TokenType::C_CONSTANT, token_str);
    };
    // Valid character constant
    EXPECT_EQ(tsp->Begin()->Tag(), TokenType::NEWLINE);
    expect_c_constant_and_newline("'Z'");
    expect_c_constant_and_newline("'1'");
    expect_c_constant_and_newline("'@'");
    expect_c_constant_and_newline("'\\''");
    expect_c_constant_and_newline("'\"'");
    expect_c_constant_and_newline("'\\\"'");
    expect_c_constant_and_newline("'\\\\'");
    expect_c_constant_and_newline("'\\a'");
    expect_c_constant_and_newline("'\\n'");
    expect_c_constant_and_newline("'\\77'");
    expect_c_constant_and_newline("'\\x88'");
    expect_c_constant_and_newline("'\\u0041'");
    expect_c_constant_and_newline("u'\\u7834'");
    expect_c_constant_and_newline("u'\\xffff'");
    expect_c_constant_and_newline("U'\\u7834'");
    expect_c_constant_and_newline("U'\\xffffffff'");
    expect_c_constant_and_newline("U'\\U0001f34c'");
    expect_c_constant_and_newline("L'\\u7834'");
    expect_c_constant_and_newline("L'\\U0001f34c'");
    expect_c_constant_and_newline("L'\\xffffffff'");
    // multi-character character constant
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_c_constant_and_newline("'\\578'");
    expect_c_constant_and_newline("'AAU'");
    expect_c_constant_and_newline("'\\u78348'");
    // unicode character literals may not contain multiple characters
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_c_constant_and_newline("u'AB'");
    expect_c_constant_and_newline("u'\\1F'");
    expect_c_constant_and_newline("U'\\u78348'");
    // wchar_t character literals may not contain multiple characters
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_c_constant_and_newline("L'AB'");
    // incomplete universal character name
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_c_constant_and_newline("'\\u2d3'");
    expect_c_constant_and_newline("'\\U576F6'");
    expect_c_constant_and_newline("u'\\U1X'");
    // character too large for enclosing character literal type
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_c_constant_and_newline("'\\u7834'");
    expect_c_constant_and_newline("'\\U0001f34c'");
    expect_c_constant_and_newline("u'\\U0001f34c'");
    // hex escape sequence out of range
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_c_constant_and_newline("'\\xfff'");
    expect_c_constant_and_newline("'\\xfffffH'");
    expect_c_constant_and_newline("u'\\xfffff'");
    // octal escape sequence out of range
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_c_constant_and_newline("'\\7777'");
    // unknown escape sequence '\*'
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_c_constant_and_newline("'\\@'");
    expect_c_constant_and_newline("'\\9dd'");
    // \x used with no following hex digits
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_c_constant_and_newline("'\\xgg'");
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::END);
}

TEST_F(ScannerTest, ScanStrLiteral) {
    InitScanner("str_literals.c");
    std::unique_ptr<TokenSequence> tsp = scp_->Scan();
    auto expect_s_literal_and_newline =
        [&ts = *tsp](const std::string& token_str) {
            ExpectTokenAndNewline(ts, TokenType::STRING, token_str);
    };
    // Valid string literals
    EXPECT_EQ(tsp->Begin()->Tag(), TokenType::NEWLINE);
    expect_s_literal_and_newline("\"test string\"");
    expect_s_literal_and_newline("\"@str\"");
    expect_s_literal_and_newline("\"\\'\\\"'\"");
    expect_s_literal_and_newline("\"\\u7834\\u7834\"");
    expect_s_literal_and_newline("\"\\029x\"");
    expect_s_literal_and_newline("u8\"\\u7834\\u7834\"");
    expect_s_literal_and_newline("u8\"\\x88\\x76\\x23\"");
    expect_s_literal_and_newline("u\"\\u7834\\u7834\"");
    expect_s_literal_and_newline("u\"\\xffff\\xffffstr\"");
    expect_s_literal_and_newline("U\"\\u7834\\u7834\"");
    expect_s_literal_and_newline("U\"str\\U0001f34c\\U0001f34c\"");
    expect_s_literal_and_newline("L\"\\u7834\\u7834\"");
    expect_s_literal_and_newline("L\"str\\U0001f34c\\U0001f34c\"");
    // incomplete universal character name
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_s_literal_and_newline("\"\\u2d3str\"");
    expect_s_literal_and_newline("\"str\\U576F6\"");
    expect_s_literal_and_newline("u\"@\\U1X\"");
    // hex escape sequence out of range
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_s_literal_and_newline("\"str\\xfff\"");
    expect_s_literal_and_newline("\"\\xfffffH__\"");
    // octal escape sequence out of range
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_s_literal_and_newline("\"str\\7777\"");
    // unknown escape sequence '\*'
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_s_literal_and_newline("\"@**\\\\\\@\"");
    expect_s_literal_and_newline("\"str\\9dd\"");
    // \x used with no following hex digits
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_s_literal_and_newline("\"__\\xgg\"");
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::END);
}

TEST_F(ScannerTest, ScanIdent) {
    InitScanner("idents.c");
    std::unique_ptr<TokenSequence> tsp = scp_->Scan();
    auto expect_ident_and_newline =
        [&ts = *tsp](const std::string& token_str) {
            ExpectTokenAndNewline(ts, TokenType::IDENTIFIER, token_str);
    };
    EXPECT_EQ(tsp->Begin()->Tag(), TokenType::INT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::BOOL);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::ALIGNOF);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_ident_and_newline("_bool");
    expect_ident_and_newline("longlong");
    expect_ident_and_newline("try");
    expect_ident_and_newline("abcd");
    expect_ident_and_newline("\\u7834");
    expect_ident_and_newline("uu\\u7834");
    expect_ident_and_newline("uu\\u783");
    expect_ident_and_newline("\\U0001f34cuu");
    expect_ident_and_newline("\\U0001f34uu");
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INVALID);
    expect_ident_and_newline("ident");
    // ident$ident
    Token* tp = tsp->Next();
    EXPECT_EQ(tp->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tp->TokenStr(), "ident");
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INVALID);
    expect_ident_and_newline("ident");
    // @ident
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INVALID);
    expect_ident_and_newline("ident");
    expect_ident_and_newline("_Ident");
    expect_ident_and_newline("_i11");
    // 11aa
    tp = tsp->Next();
    EXPECT_EQ(tp->Tag(), TokenType::I_CONSTANT);
    EXPECT_EQ(tp->TokenStr(), "11aa");
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    expect_ident_and_newline("_11");
    expect_ident_and_newline("A_IO__");
    // UU\\u7834
    tp = tsp->Next();
    EXPECT_EQ(tp->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tp->TokenStr(), "UU");
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INVALID);
    expect_ident_and_newline("\\u7834");
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::END);
}

TEST_F(ScannerTest, ScanAll) {
    InitScanner("testfile2.c");
    std::unique_ptr<TokenSequence> tsp = scp_->Scan();
    EXPECT_EQ(tsp->Begin()->Tag(), TokenType::SHARP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::STRING);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SHARP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::COMMA);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::VOID);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::DSHARP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RETURN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SHARP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SHARP);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::COMMA);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::I_CONSTANT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::STATIC);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RETURN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::STATIC_ASSERT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::I_CONSTANT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::PLUS);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::I_CONSTANT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::EQ);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::I_CONSTANT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::COMMA);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::DO);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::WHILE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::I_CONSTANT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::ASGN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::I_CONSTANT);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RETURN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::LPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::INC);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::PLUS);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RPAR);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::SCLN);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::RBRACE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::NEWLINE);
    EXPECT_EQ(tsp->Next()->Tag(), TokenType::END);
}

}
