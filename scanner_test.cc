#include <memory>

#include "scanner.hh"
#include "gtest/gtest.h"

namespace axcc {

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
    char LookAhead() const { return scp_->LookAhead(); }
    char LookAheadN(int n) const { return scp_->LookAheadN(n); }
    bool NextIs(char c) const { return scp_->NextIs(c); }
    bool Try(char c) { return scp_->Try(c); }
    bool Try(const std::string& chars) { return scp_->Try(chars); }
    unsigned int FindNext(char c) { return scp_->FindNext(c); }
    void MakeTokenInTS(const TokenType& tag, const std::string& token_str) {
        return scp_->MakeTokenInTS(tag, token_str);
    }
    void MakeTokenInTS(const TokenType& tag) { return scp_->MakeTokenInTS(tag); }

    std::string fname_{};
    std::string fcontent_{};
    std::unique_ptr<Scanner> scp_{};
};

TEST_F(ScannerTest, FindNext) {
    InitScanner("testfile1.c");
    EXPECT_EQ(FindNext('\n'), 12);
    EXPECT_EQ(FindNext('9'), 0);
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

TEST_F(ScannerTest, MakeTokenInTS) {
    InitScanner("testfile1.c");
    MakeTokenInTS(TokenType::INT);
    for (int i = 0; i < 4; i++)
        Next();
    MakeTokenInTS(TokenType::IDENTIFIER, "main");

    // Check the first token
    Token* tp = TS()->Begin();
    EXPECT_EQ(tp->Tag(), TokenType::INT);
    EXPECT_TRUE(tp->TokenStr().empty());

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

}
