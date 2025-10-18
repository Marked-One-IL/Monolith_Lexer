[KEYWORD: 'if'] [SYMBOL: '('] [SYMBOL: '('] [IDENTIFIER: 'x'] [SYMBOL: '<'] [INT_LITERAL: '7'] [SYMBOL: ')'] [SYMBOL: 'and'] [NEW_LINE]
[SYMBOL: '('] [IDENTIFIER: 'y'] [SYMBOL: '>'] [INT_LITERAL: '4'] [SYMBOL: ')'] [SYMBOL: ')'] [SYMBOL: ':'] [NEW_LINE]
	[INDENT]
	[IDENTIFIER: 'print'] [SYMBOL: '('] [STRING_LITERAL: '"Hello, World!"'] [SYMBOL: ')'] [NEW_LINE]
	[NEW_LINE]
	[KEYWORD: 'if'] [SYMBOL: '('] [SYMBOL: '('] [IDENTIFIER: 'x'] [SYMBOL: '<'] [INT_LITERAL: '7'] [SYMBOL: ')'] [SYMBOL: 'and'] [SYMBOL: '('] [IDENTIFIER: 'y'] [SYMBOL: '>'] [INT_LITERAL: '4'] [SYMBOL: ')'] [SYMBOL: ')'] [SYMBOL: ':'] [NEW_LINE]
		[INDENT]
		[IDENTIFIER: 'print'] [SYMBOL: '('] [STRING_LITERAL: '"dsafdfsjsdjkhfh"'] [SYMBOL: ')'] [NEW_LINE]
		[KEYWORD: 'if'] [SYMBOL: '('] [SYMBOL: '('] [IDENTIFIER: 'x'] [SYMBOL: '<'] [INT_LITERAL: '7'] [SYMBOL: ')'] [SYMBOL: 'and'] [SYMBOL: '('] [IDENTIFIER: 'y'] [SYMBOL: '>'] [INT_LITERAL: '4'] [SYMBOL: ')'] [SYMBOL: ')'] [SYMBOL: ':'] [NEW_LINE]
		[NEW_LINE]
	[DEDENT]
	[IDENTIFIER: 'print'] [SYMBOL: '('] [STRING_LITERAL: '"dsadas"'] [SYMBOL: ')'] [NEW_LINE]
	[NEW_LINE]
	[KEYWORD: 'def'] [IDENTIFIER: 'main'] [SYMBOL: '('] [SYMBOL: ')'] [SYMBOL: ':'] [NEW_LINE]
		[INDENT]
		[IDENTIFIER: 'myFloat'] [SYMBOL: ':'] [KEYWORD: 'float'] [SYMBOL: '='] 
At file: input.mon

At line: 8
Error: Indent (spacing) doesn't match previous indents
print("dsafdfsjsdjkhfh")
^
At line: 14
Error: Invalid scientific notation literal
myFloat: float = 1e+#
                    ^
