[INT_LITERAL: '123'] [HEX_LITERAL: '0xDEAD'] [BIN_LITERAL: '0b1101'] [OCT_LITERAL: '0o755'] [FLOAT_LITERAL: '3.14'] [SCI_LITERAL: '2e10'] [SCI_LITERAL: '.5e-'] [INT_LITERAL: '3'] [SCI_LITERAL: '6.022e23'] [NEW_LINE]
[STRING_LITERAL: '"normal string"'] [STRING3_LITERAL: '"""triple
string"""'] [CHAR_LITERAL: ''c''] [CHAR_LITERAL: ''\n''] [CHAR_LITERAL: ''a''] [CHAR_LITERAL: ''\t''] [NEW_LINE]
[BOOL_LITERAL: 'True'] [BOOL_LITERAL: 'False'] [NONE_LITERAL: 'None'] [NEW_LINE]
[IDENTIFIER: 'foo'] [IDENTIFIER: 'bar'] [IDENTIFIER: '_baz'] [IDENTIFIER: 'qux123'] [IDENTIFIER: 'var_name'] [NEW_LINE]
[SYMBOL: '+'] [SYMBOL: '-'] [SYMBOL: '*'] [SYMBOL: '/'] [SYMBOL: '%'] [SYMBOL: '<<'] [SYMBOL: '>>'] [SYMBOL: '|='] [SYMBOL: '&='] [SYMBOL: '^='] [SYMBOL: '~'] [SYMBOL: '=='] [SYMBOL: '!='] [SYMBOL: '>='] [SYMBOL: '<='] [SYMBOL: '->'] [SYMBOL: '::'] [SYMBOL: '...'] [SYMBOL: '++'] [SYMBOL: '+='] [SYMBOL: '--'] [SYMBOL: '-='] [SYMBOL: '*='] [SYMBOL: '/='] [NEW_LINE]
[KEYWORD: 'if'] [KEYWORD: 'elif'] [KEYWORD: 'else'] [KEYWORD: 'for'] [KEYWORD: 'while'] [KEYWORD: 'switch'] [KEYWORD: 'case'] [KEYWORD: 'default'] [KEYWORD: 'break'] [KEYWORD: 'continue'] [KEYWORD: 'label'] [KEYWORD: 'goto'] [NEW_LINE]
[KEYWORD: 'def'] [KEYWORD: 'return'] [KEYWORD: 'class'] [KEYWORD: 'const'] [KEYWORD: 'static'] [KEYWORD: 'int8'] [KEYWORD: 'uint8'] [KEYWORD: 'int16'] [KEYWORD: 'uint16'] [KEYWORD: 'int32'] [KEYWORD: 'uint32'] [KEYWORD: 'int64'] [KEYWORD: 'uint64'] [NEW_LINE]
[KEYWORD: 'float'] [KEYWORD: 'double'] [KEYWORD: 'import'] [KEYWORD: 'ptr'] [KEYWORD: 'ref'] [KEYWORD: 'dref'] [KEYWORD: 'arr'] [KEYWORD: 'enum'] [KEYWORD: 'namespace'] [KEYWORD: 'typedef'] [NEW_LINE]
[SYMBOL: '('] [IDENTIFIER: 'x'] [SYMBOL: '+'] [IDENTIFIER: 'y'] [SYMBOL: ')'] [SYMBOL: '*'] [SYMBOL: '['] [IDENTIFIER: 'z'] [SYMBOL: '-'] [INT_LITERAL: '1'] [SYMBOL: ']'] [SYMBOL: '/'] [SYMBOL: '++'] [SYMBOL: '+'] [SYMBOL: '--'] [SYMBOL: '-'] [SYMBOL: '*'] [SYMBOL: '*'] [SYMBOL: '*'] [SYMBOL: '/'] [SYMBOL: '/'] [SYMBOL: '/='] [SYMBOL: '=='] [SYMBOL: '!='] [SYMBOL: '<'] [SYMBOL: '>>'] [SYMBOL: '<='] [SYMBOL: '>>='] [SYMBOL: '...'] [SYMBOL: '('] [SYMBOL: ')'] [SYMBOL: '['] [SYMBOL: ']'] [IDENTIFIER: 'abc123__XYZ'] [NEW_LINE]
[IDENTIFIER: 'randomGarbage'] [SCI_LITERAL: '123.456e-'] [INT_LITERAL: '78'] [HEX_LITERAL: '0xABCDEF'] [BIN_LITERAL: '0b1010101'] [OCT_LITERAL: '0o1234'] [NEW_LINE]
[CHAR_LITERAL: ''a''] [CHAR_LITERAL: ''\x''] [CHAR_LITERAL: ''\n''] [NEW_LINE]
[IDENTIFIER: 'TrueFalseNone'] [NEW_LINE]
[SYMBOL: '('] [IDENTIFIER: 'foo_bar'] [SYMBOL: '+'] [HEX_LITERAL: '0X1F'] [SYMBOL: '-'] [BIN_LITERAL: '0b10'] [IDENTIFIER: '_101'] [SYMBOL: ')'] [SYMBOL: '*'] [KEYWORD: 'arr'] [SYMBOL: '['] [INT_LITERAL: '42'] [SYMBOL: ']'] [SYMBOL: '/'] [IDENTIFIER: 'some_var'] [NEW_LINE]
[SYMBOL: '.'] [SYMBOL: '.'] [SYMBOL: '.'] [SYMBOL: '.'] [SYMBOL: '...'] [SYMBOL: '.'] [SYMBOL: '.'] [SYMBOL: '++'] [SYMBOL: '--'] [SYMBOL: '=='] [SYMBOL: '!='] [SYMBOL: '<<'] [SYMBOL: '>>'] [SYMBOL: '>>='] [SYMBOL: '<<='] [NEW_LINE]
[IDENTIFIER: 'nested_identifiers_and_numbers1234567890'] [NEW_LINE]
[IDENTIFIER: 'random_symbols_'] 
At file: input.mon

At line: 9
Error: Invalid character
(x + y) * [z - 1] / {a + b} ... ??? :: -> <<=
                    ^
At line: 11
Error: Invalid character
randomGarbage$$$@@@!!!###%%%^^^&&&___
             ^
At line: 13
Error: String literal does not end at current line
"unterminated string
^
At line: 19
Error: Invalid character
random_symbols_!@#$%^&*()_+=-[]|\:;<>,.?/~`
               ^
At line: 20
Error: Invalid character
{}
^
