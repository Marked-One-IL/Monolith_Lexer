[IDENTIFIER: 'x'] [SYMBOL: ':'] [KEYWORD: 'int32'] [SYMBOL: '='] [INT_LITERAL: '10'] [NEW_LINE]
[IDENTIFIER: 'y'] [SYMBOL: ':'] [KEYWORD: 'int32'] [SYMBOL: '='] [INT_LITERAL: '10'] [NEW_LINE]
[IDENTIFIER: 'myFloat'] [SYMBOL: ':'] [KEYWORD: 'float'] [SYMBOL: '='] [NEW_LINE]
[HEX_LITERAL: '0xab1'] [NEW_LINE]
[NEW_LINE]
	[INDENT]
	[NEW_LINE]
	[SCI_LITERAL: '1e+556'] [NEW_LINE]
	[NEW_LINE]
	[NEW_LINE]
	[KEYWORD: 'def'] [IDENTIFIER: 'main'] [SYMBOL: '('] [IDENTIFIER: 'argc'] [SYMBOL: ':'] [KEYWORD: 'int32'] [SYMBOL: ','] [IDENTIFIER: 'argv'] [SYMBOL: ':'] [KEYWORD: 'ptr'] [SYMBOL: '['] [KEYWORD: 'int8'] [SYMBOL: ','] [INT_LITERAL: '2'] [SYMBOL: ']'] [SYMBOL: ')'] [SYMBOL: ':'] [NEW_LINE]
	[IDENTIFIER: 'print'] [SYMBOL: '('] [STRING_LITERAL: '"Hello, World\n"'] [SYMBOL: ')'] [NEW_LINE]
	[NEW_LINE]
	[KEYWORD: 'if'] [SYMBOL: '('] [IDENTIFIER: 'x'] [SYMBOL: '<'] [INT_LITERAL: '3'] [SYMBOL: 'and'] [NEW_LINE]
	[IDENTIFIER: 'y'] [SYMBOL: '>'] [INT_LITERAL: '4'] [SYMBOL: ')'] [SYMBOL: ':'] [NEW_LINE]
		[INDENT]
		[KEYWORD: 'if'] [SYMBOL: '('] [NEW_LINE]
		[IDENTIFIER: 'idk'] [SYMBOL: ')'] [SYMBOL: ':'] [NEW_LINE]
			[INDENT]
			[IDENTIFIER: 'print'] [SYMBOL: '('] [SYMBOL: ')'] [NEW_LINE]
			[NEW_LINE]
		[DEDENT]
	[DEDENT]
	[KEYWORD: 'return'] [INT_LITERAL: '0'] [NEW_LINE]

At file: input.mon

At line: 2
Error: Invalid float literal
myFloat : float = 10.1a
                      ^
At line: 6
Error: String literal does not end at current line
"1234 
^
