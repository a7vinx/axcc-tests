// Valid character constant
'Z'
'1'
'@'
'\''
'"'
'\"'
'\\'
'\a'
'\n'
'\77'
'\x88'
'\u0041'
u'\u7834'
u'\xffff'
U'\u7834'
U'\xffffffff'
U'\U0001f34c'
L'\u7834'
L'\U0001f34c'
L'\xffffffff'
// multi-character character constant
'\578'
'AAU'
'\u78348'
// unicode character literals may not contain multiple characters
u'AB'
u'\1F'
U'\u78348'
// wchar_t character literals may not contain multiple characters
L'AB'
// incomplete universal character name
'\u2d3'
'\U576F6'
u'\U1X'
// character too large for enclosing character literal type
'\u7834'
'\U0001f34c'
u'\U0001f34c'
// hex escape sequence out of range
'\xfff'
'\xfffffH'
u'\xfffff'
// octal escape sequence out of range
'\7777'
// unknown escape sequence '\*'
'\@'
'\9dd'
// \x used with no following hex digits
'\xgg'