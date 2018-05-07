#include "token.hh"
#include "gtest/gtest.h"

namespace axcc {

TEST(LocStrTest, LocationString) {
    const std::string fname{"testname"};
    const std::string fcontent{
        "testcontent line1\ntestcontent line2\n\t testcontent line3"};
    SourceLocation loc1{&fname, 1, 1, fcontent.cbegin(), 17};
    EXPECT_EQ(LocStr(loc1),
            "testname:1:1: testcontent line1\n              ^\n");
    SourceLocation loc2{&fname, 2, 13, std::next(fcontent.cbegin(), 18), 17};
    EXPECT_EQ(LocStr(loc2),
            "testname:2:13: testcontent line2\n                           ^\n");
    SourceLocation loc3{&fname, 3, 15, std::next(fcontent.cbegin(), 36), 19};
    EXPECT_EQ(LocStr(loc3),
            "testname:3:15: testcontent line3\n                           ^\n");
}

class TokenSequenceTest : public ::testing::Test {
protected:
    TokenSequenceTest() : fname_{"testname"},
                          fcontent_{"ident1 ident2 ident3"} {
        ts_.EmplaceBack(TokenType::IDENTIFIER, "ident1",
                        std::make_shared<const SourceLocation>(
                            SourceLocation{&fname_, 1, 1,
                                           fcontent_.cbegin(), 20}));
        ts_.EmplaceBack(TokenType::IDENTIFIER, "ident2");
        ts_.EmplaceBack(TokenType::IDENTIFIER, "ident3");
    }

    const std::string fname_;
    const std::string fcontent_;
    TokenSequence ts_{};
};

TEST_F(TokenSequenceTest, Construction) {
    ts_.EmplaceBack(TokenType::IDENTIFIER, "testcontent1");
    ts_.PushBack({TokenType::IDENTIFIER, "testcontent2"});
    Token* tp = ts_.Begin();
    EXPECT_EQ(tp->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tp->TokenStr(), "ident1");
    EXPECT_EQ(*(tp->LocPtr()->fnamep), "testname");
    EXPECT_EQ(*(tp->LocPtr()->linep), 'i');
}

TEST_F(TokenSequenceTest, Iteration) {
    // Begin()
    Token* tp = ts_.Begin();
    EXPECT_EQ(tp->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tp->TokenStr(), "ident1");
    // LookAheadN()
    tp = ts_.LookAhead();
    EXPECT_EQ(tp->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tp->TokenStr(), "ident2");
    tp = ts_.LookAheadN(2);
    EXPECT_EQ(tp->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tp->TokenStr(), "ident3");
    // Next()
    tp = ts_.Next();
    EXPECT_EQ(tp->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tp->TokenStr(), "ident2");
    tp = ts_.Next();
    EXPECT_EQ(tp->Tag(), TokenType::IDENTIFIER);
    EXPECT_EQ(tp->TokenStr(), "ident3");
    EXPECT_EQ(ts_.Next()->Tag(), TokenType::END);
}

TEST_F(TokenSequenceTest, Erase) {
    EXPECT_EQ(ts_.Begin()->TokenStr(), "ident1");
    ts_.ErasePrevN(1);
    EXPECT_EQ(ts_.Begin()->TokenStr(), "ident2");
    EXPECT_EQ(ts_.Next()->TokenStr(), "ident3");
    EXPECT_EQ(ts_.Next()->Tag(), TokenType::END);
    ts_.ErasePrevN(2);
    EXPECT_EQ(ts_.Begin()->Tag(), TokenType::END);
}

TEST_F(TokenSequenceTest, CopyConstruct) {
    // Here the reference count should be 2 because LocPtr() has returned
    // a copy.
    EXPECT_EQ(ts_.Begin()->LocPtr().use_count(), 2);
    TokenSequence ts_copy{ts_};
    EXPECT_EQ(ts_.Begin()->LocPtr().use_count(), 3);
    ts_.Next();
    ts_.Next();
    ts_.ErasePrevN(3);
    Token* tp = ts_copy.Begin();
    EXPECT_EQ(tp->TokenStr(), "ident1");
    EXPECT_EQ(tp->LocPtr().use_count(), 2);
    EXPECT_EQ(ts_copy.Next()->TokenStr(), "ident2");
    EXPECT_EQ(ts_copy.Next()->TokenStr(), "ident3");
    EXPECT_EQ(ts_copy.Next()->Tag(), TokenType::END);
}

TEST_F(TokenSequenceTest, MoveConstruct) {
    EXPECT_EQ(ts_.Begin()->LocPtr().use_count(), 2);
    TokenSequence ts_move{std::move(ts_)};
    // Check the origin object.
    EXPECT_EQ(ts_.Begin()->Tag(), TokenType::END);
    // Check the new one.
    Token* tp = ts_move.Begin();
    EXPECT_EQ(tp->TokenStr(), "ident1");
    EXPECT_EQ(tp->LocPtr().use_count(), 2);
    EXPECT_EQ(ts_move.Next()->TokenStr(), "ident2");
    EXPECT_EQ(ts_move.Next()->TokenStr(), "ident3");
    EXPECT_EQ(ts_move.Next()->Tag(), TokenType::END);
}

TEST_F(TokenSequenceTest, Replace) {
    SourceLocation loc{&fname_, 2, 2, fcontent_.cbegin(), 20};
    TokenSequence ts_copy{ts_};
    std::vector<Token> tv{{TokenType::IDENTIFIER, "ident4"},
                          {TokenType::IDENTIFIER, "ident5"}};
    EXPECT_EQ(ts_.Begin()->TokenStr(), "ident1");
    EXPECT_EQ(ts_.Next()->TokenStr(), "ident2");
    // void ReplacePrevN(int n, TokenSequence&& ts);
    ts_.ReplacePrevN(1, std::move(ts_copy));
    EXPECT_EQ(ts_.Begin()->TokenStr(), "ident1");
    EXPECT_EQ(ts_.Next()->TokenStr(), "ident1");
    EXPECT_EQ(ts_.Next()->TokenStr(), "ident2");
    EXPECT_EQ(ts_.Next()->TokenStr(), "ident3");
    EXPECT_EQ(ts_.Next()->TokenStr(), "ident3");
    // void ReplacePrevN(int n, const std::vector<Token>& tv);
    ts_.ReplacePrevN(5, tv);
    EXPECT_EQ(ts_.Begin()->TokenStr(), "ident4");
    EXPECT_EQ(ts_.Next()->TokenStr(), "ident5");
    EXPECT_EQ(ts_.Next()->Tag(), TokenType::END);
}

}
