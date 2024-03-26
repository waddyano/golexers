package golexers

/*!include:re2c "word.re" */

import (
    "fmt"
)

/*!re2c
	re2c:eof = 0;
	re2c:define:YYCTYPE    = byte;
	re2c:define:YYPEEK     = "peek(in)";
	re2c:define:YYSKIP     = "in.cursor += 1";
	re2c:define:YYBACKUP   = "in.marker = in.cursor";
	re2c:define:YYRESTORE  = "in.cursor = in.marker";
	re2c:define:YYLESSTHAN = "in.limit <= in.cursor + @@{len}";
	re2c:define:YYFILL     = "fill(in) == 0";
*/

func js_lex_str(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { return STRING }
        $                    { return -1 }
        "\""                 { if in.state == STATE_STRINGLITERAL { in.state = STATE_NORMAL }; return STRING }
        "\'"                 { if in.state == STATE_CHARLITERAL { in.state = STATE_NORMAL }; return STRING }
        [a-zA-Z_0-9]+        { return STRINGWORD }
        "\\a"                { return STRING }
        "\\b"                { return STRING }
        "\\f"                { return STRING }
        "\\n"                { return STRING }
        "\\r"                { return STRING }
        "\\t"                { return STRING }
        "\\v"                { return STRING }
        "\\\\"               { return STRING }
        "\\'"                { return STRING }
        "\\\""               { return STRING }
        "\\?"                { return STRING }
        "\\" [0-7]{1,3}      { return STRING }
        //"\\u" [0-9a-fA-F]{4} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\U" [0-9a-fA-F]{8} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\x" [0-9a-fA-F]+   { if (!lex_hex(in.tok, in.cur, u)) return false; continue; }	
	*/
	}
}

func js_lex_long_str(in *Input, start bool) TokenType {
	for {
		if start {
			in.raw_str_delim = in.data[in.token + 2:in.cursor - 1]
			start = false
		}
		in.token = in.cursor
    /*!re2c
        *                    { return STRING }
        $                    { return -1 }
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }
        wsp                  { continue }

		"`"                  { in.state = STATE_NORMAL; return STRING }

        [a-zA-Z_0-9]+        { return STRINGWORD }
	*/
	}
}

func js_lex_ml_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { return COMMENT }
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }
        "*/"                 { in.state = STATE_NORMAL; return COMMENT }
        $                    { return END }
        word       			 { return COMMENTWORD }
	*/
	}
}

func js_lex(in *Input) TokenType {
	for {
		in.token = in.cursor
        //fmt.Printf("start at %d\n", in.token)
		if in.state == STATE_STRINGLITERAL || in.state == STATE_CHARLITERAL {
			t := js_lex_str(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_LONGSTRINGLITERAL) {
			t := js_lex_long_str(in, false)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_MLCOMMENT) {
			t := js_lex_ml_comment(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_EOLCOMMENT) {
			t := lex_eol_comment(in)
			if t >= 0 {
				return t
			}
		}

    /*!re2c
		"\\" { continue }
        wsp { continue }
		newline { in.bolcursor = in.cursor; in.line += 1; continue }

        * { fmt.Printf("%s: %d: match %2x\n", in.filename, in.line, in.data[in.cursor-1]); continue }
        $ { return END }

        "@" { continue } // Objective-c

		decimal = [1-9][0-9]*;
		hex = "0x" [0-9a-fA-F]+;
		octal = "0" [0-7]*;

		decimal { return LITERAL }
		hex { return LITERAL }
		octal { return LITERAL }

		"\"" { in.state = STATE_STRINGLITERAL; return STRING }
		"\'" { in.state = STATE_CHARLITERAL; return STRING }
		"`" { in.state = STATE_LONGSTRINGLITERAL; return STRING }
		"//" { in.state = STATE_EOLCOMMENT; return COMMENT }
		"/*" { in.state = STATE_MLCOMMENT; return COMMENT }

        "await" { return KEYWORD }
        "break" { return KEYWORD }
        "case" { return KEYWORD }
        "catch" { return KEYWORD }
        "class" { return KEYWORD }
        "const" { return KEYWORD }
        "continue" { return KEYWORD }
        "debugger" { return KEYWORD }
        "default" { return KEYWORD }
        "delete" { return KEYWORD }
        "do" { return KEYWORD }
        "else" { return KEYWORD }
        "enum" { return KEYWORD }
        "export" { return KEYWORD }
        "extends" { return KEYWORD }
        "false" { return KEYWORD }
        "finally" { return KEYWORD }
        "for" { return KEYWORD }
        "function" { return KEYWORD }
        "if" { return KEYWORD }
        "import" { return KEYWORD }
        "in" { return KEYWORD }
        "instanceof" { return KEYWORD }
        "lambda" { return KEYWORD }
        "let" { return KEYWORD }
        "new" { return KEYWORD }
        "null" { return KEYWORD }
        "return" { return KEYWORD }
        "static" { return KEYWORD }
        "super" { return KEYWORD }
        "switch" { return KEYWORD }
        "this" { return KEYWORD }
        "throw" { return KEYWORD }
        "true" { return KEYWORD }
        "try" { return KEYWORD }
        "typeof" { return KEYWORD }
        "var" { return KEYWORD }
        "void" { return KEYWORD }
        "while" { return KEYWORD }
        "with" { return KEYWORD }
        "yield" { return KEYWORD }

		"+" { return PUNCTUATION }
		"-" { return PUNCTUATION }
		"/" { return PUNCTUATION }
		"*" { return PUNCTUATION }
		"%" { return PUNCTUATION }
		"&" { return PUNCTUATION }
		"|" { return PUNCTUATION }
		"&&" { return PUNCTUATION }
		"||" { return PUNCTUATION }
		"!" { return PUNCTUATION }
		"^" { return PUNCTUATION }
		"~" { return PUNCTUATION }
		";" { return PUNCTUATION }
		"." { return PUNCTUATION }
		"," { return PUNCTUATION }
		"(" { return PUNCTUATION }
		")" { return PUNCTUATION }
		"{" { return PUNCTUATION }
		"}" { return PUNCTUATION }
		"[" { return PUNCTUATION }
		"]" { return PUNCTUATION }
		"=" { return PUNCTUATION }
		"==" { return PUNCTUATION }
		"!=" { return PUNCTUATION }
		"<" { return PUNCTUATION }
		">" { return PUNCTUATION }
		"<=" { return PUNCTUATION }
		">=" { return PUNCTUATION }
		"->" { return PUNCTUATION }
		"?" { return PUNCTUATION }
		":" { return PUNCTUATION }
		"::" { return PUNCTUATION }

        [a-zA-Z_$][a-zA-Z_0-9$]* { return IDENTIFIER }
    */
    }
}

func init() {
	Register([]string{".js"}, js_lex)
}