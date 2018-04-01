// Valid string literals
"test string"
"@str"
"\'\"'"
"\u7834\u7834"
"\029x"
u8"\u7834\u7834"
u8"\x88\x76\x23"
u"\u7834\u7834"
u"\xffff\xffffstr"
U"\u7834\u7834"
U"str\U0001f34c\U0001f34c"
L"\u7834\u7834"
L"str\U0001f34c\U0001f34c"
// incomplete universal character name
"\u2d3str"
"str\U576F6"
u"@\U1X"
// hex escape sequence out of range
"str\xfff"
"\xfffffH__"
// octal escape sequence out of range
"str\7777"
// unknown escape sequence '\*'
"@**\\\@"
"str\9dd"
// \x used with no following hex digits
"__\xgg"
