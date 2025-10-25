#pragma once

namespace Lexer
{
	enum class Tag
	{
		// Literals
		STRING3_LITERAL,// """Hello, World"""
		STRING_LITERAL, // "Hello, World!"
		CHAR_LITERAL,   // 'H'
		HEX_LITERAL,    // 0xABCD
		BIN_LITERAL,    // 0b0101
		OCT_LITERAL,	// 0o01234567
		SCI_LITERAL,    // 1e-3
		FLOAT_LITERAL,  // 1234.1234
		INT_LITERAL,    // 1234
		BOOL_LITERAL,   // True, False
		NONE_LITERAL,   // None

		// Constants
		SYMBOL,         // +=, -, *, >>, (, and, or and ect
		KEYWORD,        // if, for, def and ect

		// Other
		NEW_LINE,	     // \n
		INDENT,          // Increasement in level (\n and \t)
		DEDENT,          // Decreasement in level (\n and \t)
		IDENTIFIER,      // *any*
	};
}
