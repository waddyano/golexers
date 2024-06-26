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

func go_lex_str(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { return STRING }
        $                    { return -1 }
        "\""                 { if in.state == STATE_STRINGLITERAL { in.state = STATE_NORMAL }; return STRING }
        "\'"                 { if in.state == STATE_CHARLITERAL { in.state = STATE_NORMAL }; return STRING }
        nword		         { return STRINGWORD }
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
	*/
	}
}

func go_lex_long_str(in *Input, start bool) TokenType {
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

		"`" { in.state = STATE_NORMAL; return STRING }

        nword                 { return STRINGWORD }
	*/
	}
}

func go_lex_ml_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { return COMMENT }
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }
        "*/"                 { in.state = STATE_NORMAL; return COMMENT }
        $                    { return END }
        nword		     	 { return COMMENTWORD }
	*/
	}
}

func go_lex(in *Input) TokenType {
	for {
		in.token = in.cursor
        //fmt.Printf("start at %d\n", in.token)
		if in.state == STATE_STRINGLITERAL || in.state == STATE_CHARLITERAL {
			t := go_lex_str(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_LONGSTRINGLITERAL) {
			t := go_lex_long_str(in, false)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_EOLCOMMENT) {
			t := lex_eol_comment(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_MLCOMMENT) {
			t := cpp_lex_ml_comment(in)
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
		"`" { in.state = STATE_LONGSTRINGLITERAL; in.raw_str_delim = in.data[in.token:in.cursor]; return STRING }
		"//" { in.state = STATE_EOLCOMMENT; return COMMENT }
		"/*" { in.state = STATE_MLCOMMENT; return COMMENT }

        "break" { return KEYWORD }
        "byte" { return KEYWORD_TYPE }
        "case" { return KEYWORD }
        "chan" { return KEYWORD }
		"complex64" { return KEYWORD_TYPE }
		"complex128" { return KEYWORD_TYPE }
        "const" { return KEYWORD }
        "continue" { return KEYWORD }
        "default" { return KEYWORD }
        "defer" { return KEYWORD }
        "else" { return KEYWORD }
        "fallthrough" { return KEYWORD }
        "false" { return KEYWORD } // really not keyword
		"float32" { return KEYWORD_TYPE }
		"float64" { return KEYWORD_TYPE }
        "for" { return KEYWORD }
        "func" { return KEYWORD }
        "go" { return KEYWORD }
        "goto" { return KEYWORD }
        "if" { return KEYWORD }
        "import" { return KEYWORD }
        "int" { return KEYWORD_TYPE }
        "int16" { return KEYWORD_TYPE }
        "int32" { return KEYWORD_TYPE }
        "int64" { return KEYWORD_TYPE }
        "int8" { return KEYWORD_TYPE }
        "interface" { return KEYWORD }
        "map" { return KEYWORD }
        "nil" { return KEYWORD } // really not keyword
        "package" { return KEYWORD }
        "range" { return KEYWORD }
        "return" { return KEYWORD }
        "rune" { return KEYWORD_TYPE }
        "select" { return KEYWORD }
        "string" { return KEYWORD_TYPE }
        "struct" { return KEYWORD }
        "switch" { return KEYWORD }
        "true" { return KEYWORD } // really not keyword
        "type" { return KEYWORD }
        "uint" { return KEYWORD_TYPE }
        "uint16" { return KEYWORD_TYPE }
        "uint32" { return KEYWORD_TYPE }
        "uint64" { return KEYWORD_TYPE }
        "uint8" { return KEYWORD_TYPE }
        "uintptr" { return KEYWORD_TYPE }
        "var" { return KEYWORD }

		"+" { return PUNCTUATION }
		"-" { return PUNCTUATION }
		"++" { return PUNCTUATION }
		"--" { return PUNCTUATION }
		"/" { return PUNCTUATION }
		"*" { return PUNCTUATION }
		"%" { return PUNCTUATION }
		"&" { return PUNCTUATION }
		"|" { return PUNCTUATION }
		"&&" { return PUNCTUATION }
		"||" { return PUNCTUATION }
		"!" { return PUNCTUATION }
		"^" { return PUNCTUATION }
		"<<" { return PUNCTUATION }
		">>" { return PUNCTUATION }
		"&^" { return PUNCTUATION }
		"~" { return PUNCTUATION }
		";" { return PUNCTUATION }
		"." { return PUNCTUATION }
		"..." { return PUNCTUATION }
		"," { return PUNCTUATION }
		"(" { return PUNCTUATION }
		")" { return PUNCTUATION }
		"{" { return PUNCTUATION }
		"}" { return PUNCTUATION }
		"[" { return PUNCTUATION }
		"]" { return PUNCTUATION }
		"=" { return PUNCTUATION }
		"+=" { return PUNCTUATION }
		"-=" { return PUNCTUATION }
		"*=" { return PUNCTUATION }
		"/=" { return PUNCTUATION }
		"%=" { return PUNCTUATION }
		"&=" { return PUNCTUATION }
		"|=" { return PUNCTUATION }
		"^=" { return PUNCTUATION }
		"==" { return PUNCTUATION }
		"!=" { return PUNCTUATION }
		"<" { return PUNCTUATION }
		">" { return PUNCTUATION }
		"<=" { return PUNCTUATION }
		">=" { return PUNCTUATION }
		">>=" { return PUNCTUATION }
		"<<=" { return PUNCTUATION }
		"&^=" { return PUNCTUATION }
		"<-" { return PUNCTUATION }
		"?" { return PUNCTUATION }
		":" { return PUNCTUATION }
		":=" { return PUNCTUATION }

		id_start    = L | Nl | [$_];
		id_continue = id_start | Mn | Mc | Nd | Pc | [\u200D\u05F3];
        id = id_start id_continue *;
        id { return IDENTIFIER }
    */
    }
}

func init() {
	Register([]string{".go"}, go_lex)
}