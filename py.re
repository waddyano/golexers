package golexers

/*!include:re2c "word.re" */

import (
	"bytes"
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

func py_lex_str(in *Input) TokenType {
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
        //"\\" [0-7]{1,3}      { lex_oct(in.tok, in.cur, u); continue; }
        //"\\u" [0-9a-fA-F]{4} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\U" [0-9a-fA-F]{8} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\x" [0-9a-fA-F]+   { if (!lex_hex(in.tok, in.cur, u)) return false; continue; }	
	*/
	}
}

func py_lex_long_str(in *Input, start bool) TokenType {
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

		"\"\"\"" { if bytes.Equal(in.raw_str_delim, in.data[in.token:in.cursor]) { in.state = STATE_NORMAL; in.raw_str_delim = nil }; return STRING }
		"\'\'\'" { if bytes.Equal(in.raw_str_delim, in.data[in.token:in.cursor]) { in.state = STATE_NORMAL; in.raw_str_delim = nil }; return STRING }

        [a-zA-Z_0-9]+        { return STRINGWORD }
	*/
	}
}

func py_lex(in *Input) TokenType {
	for {
		in.token = in.cursor
        //fmt.Printf("start at %d\n", in.token)
		if in.state == STATE_STRINGLITERAL || in.state == STATE_CHARLITERAL {
			t := py_lex_str(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_LONGSTRINGLITERAL) {
			t := py_lex_long_str(in, false)
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

        * { fmt.Printf("%s: %d: unrecognised character %2x\n", in.filename, in.line, in.data[in.cursor-1]); continue }
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
		"\"\"\"" { in.state = STATE_LONGSTRINGLITERAL; in.raw_str_delim = in.data[in.token:in.cursor]; return STRING }
		"\'\'\'" { in.state = STATE_LONGSTRINGLITERAL; in.raw_str_delim = in.data[in.token:in.cursor]; return STRING }
		"#" { in.state = STATE_EOLCOMMENT; return COMMENT }

        "and" { return KEYWORD }
        "as" { return KEYWORD }
        "assert" { return KEYWORD }
        "class" { return KEYWORD }
        "continue" { return KEYWORD }
        "def" { return KEYWORD }
        "del" { return KEYWORD }
        "elif" { return KEYWORD }
        "else" { return KEYWORD }
        "except" { return KEYWORD }
        "False" { return KEYWORD }
        "finally" { return KEYWORD }
        "for" { return KEYWORD }
        "from" { return KEYWORD }
        "global" { return KEYWORD }
        "if" { return KEYWORD }
        "import" { return KEYWORD }
        "in" { return KEYWORD }
        "is" { return KEYWORD }
        "lambda" { return KEYWORD }
        "None" { return KEYWORD }
        "nonlocal" { return KEYWORD }
        "not" { return KEYWORD }
        "or" { return KEYWORD }
        "pass" { return KEYWORD }
        "raise" { return KEYWORD }
        "return" { return KEYWORD }
        "True" { return KEYWORD }
        "try" { return KEYWORD }
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

        word { return IDENTIFIER }
    */
    }
}

func init() {
	Register([]string{".py"}, py_lex)
}