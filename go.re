package golexers

/*!include:re2c "unicode_categories.re" */

import (
    "fmt"
)

func go_lex_str(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
		re2c:eof = 0;
		re2c:define:YYCTYPE    = byte;
		re2c:define:YYPEEK     = "peek(in)";
		re2c:define:YYSKIP     = "in.cursor += 1";
		re2c:define:YYBACKUP   = "in.marker = in.cursor";
		re2c:define:YYRESTORE  = "in.cursor = in.marker";
		re2c:define:YYLESSTHAN = "in.limit <= in.cursor + @@{len}";
		re2c:define:YYFILL     = "fill(in) == 0";
        *                    { return STRING }
        $                    { return -1 }
        "\""                 { if in.state == STATE_STRINGLITERAL { in.state = STATE_NORMAL }; return STRING }
        "\'"                 { if in.state == STATE_CHARLITERAL { in.state = STATE_NORMAL }; return STRING }
		wstart				= L | Nl | [$_];
		wcontinue 			= wstart | Mn | Mc | Nd | Pc | [\u200D\u05F3];
		word  				= wstart wcontinue*;
        word		         { return STRINGWORD }
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

func go_lex_long_str(in *Input, start bool) TokenType {
	for {
		if start {
			in.raw_str_delim = in.data[in.token + 2:in.cursor - 1]
			start = false
		}
		in.token = in.cursor
    /*!re2c
		re2c:eof = 0;
		re2c:define:YYCTYPE    = byte;
		re2c:define:YYPEEK     = "peek(in)";
		re2c:define:YYSKIP     = "in.cursor += 1";
		re2c:define:YYBACKUP   = "in.marker = in.cursor";
		re2c:define:YYRESTORE  = "in.cursor = in.marker";
		re2c:define:YYLESSTHAN = "in.limit <= in.cursor + @@{len}";
		re2c:define:YYFILL     = "fill(in) == 0";
        *                    { return STRING }
        $                    { return -1 }
        "\n"                 { in.line += 1; continue }
        wsp = [ \f\t\v\r]+;
        wsp                  { continue }

		"`" { in.state = STATE_NORMAL; return STRING }

        word        		{ return STRINGWORD }
	*/
	}
}

func go_lex_eol_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
		re2c:eof = 0;
		re2c:define:YYCTYPE    = byte;
		re2c:define:YYPEEK     = "peek(in)";
		re2c:define:YYSKIP     = "in.cursor += 1";
		re2c:define:YYBACKUP   = "in.marker = in.cursor";
		re2c:define:YYRESTORE  = "in.cursor = in.marker";
		re2c:define:YYLESSTHAN = "in.limit <= in.cursor + @@{len}";
		re2c:define:YYFILL     = "fill(in) == 0";
        *                    { continue }
        "\n"                 { in.state = STATE_NORMAL; in.cursor -= 1; return END }
        $                    { return END }
        word		         { return COMMENTWORD }
        [^a-zA-Z_0-9\n]+     { return COMMENT }
	*/
	}
}

func go_lex_ml_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
		re2c:eof = 0;
		re2c:define:YYCTYPE    = byte;
		re2c:define:YYPEEK     = "peek(in)";
		re2c:define:YYSKIP     = "in.cursor += 1";
		re2c:define:YYBACKUP   = "in.marker = in.cursor";
		re2c:define:YYRESTORE  = "in.cursor = in.marker";
		re2c:define:YYLESSTHAN = "in.limit <= in.cursor + @@{len}";
		re2c:define:YYFILL     = "fill(in) == 0";
        *                    { return COMMENT }
        "\n"                 { in.line += 1; continue }
        "*/"                 { in.state = STATE_NORMAL; return COMMENT }
        $                    { return END }
        word		        { return COMMENTWORD }
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
			t := go_lex_eol_comment(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_MLCOMMENT) {
			t := cpp_lex_ml_comment(in)
			if t >= 0 {
				return t
			}
		}

	    was_bol := in.bol
		in.bol = false
    /*!re2c
		re2c:eof = 0;
		re2c:define:YYCTYPE    = byte;
		re2c:define:YYPEEK     = "peek(in)";
		re2c:define:YYSKIP     = "in.cursor += 1";
		re2c:define:YYBACKUP   = "in.marker = in.cursor";
		re2c:define:YYRESTORE  = "in.cursor = in.marker";
		re2c:define:YYLESSTHAN = "in.limit <= in.cursor + @@{len}";
		re2c:define:YYFILL     = "fill(in) == 0";

        scm = "//" [^\n]*;
		newline = [\n];
		"\\" { continue }
        wsp { in.bol = was_bol; continue }
		newline { in.bol = true; in.line += 1; continue }

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
		"`" { in.state = STATE_LONGSTRINGLITERAL; in.raw_str_delim = in.data[in.token:in.cursor]; return STRING }
		"//" { in.state = STATE_EOLCOMMENT; return COMMENT }
		"/*" { in.state = STATE_MLCOMMENT; return COMMENT }

        "break" { return KEYWORD }
        "case" { return KEYWORD }
        "chan" { return KEYWORD }
        "const" { return KEYWORD }
        "continue" { return KEYWORD }
        "default" { return KEYWORD }
        "defer" { return KEYWORD }
        "else" { return KEYWORD }
        "fallthrough" { return KEYWORD }
        "false" { return KEYWORD } // really not keyword
        "for" { return KEYWORD }
        "func" { return KEYWORD }
        "go" { return KEYWORD }
        "goto" { return KEYWORD }
        "if" { return KEYWORD }
        "import" { return KEYWORD }
        "interface" { return KEYWORD }
        "map" { return KEYWORD }
        "nil" { return KEYWORD } // really not keyword
        "package" { return KEYWORD }
        "range" { return KEYWORD }
        "return" { return KEYWORD }
        "select" { return KEYWORD }
        "switch" { return KEYWORD }
        "true" { return KEYWORD } // really not keyword
        "type" { return KEYWORD }
        "var" { return KEYWORD }

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

		id_start    = L | Nl | [$_];
		id_continue = id_start | Mn | Mc | Nd | Pc | [\u200D\u05F3];
        id = id_start id_continue *;
        id { return IDENTIFIER }
    */
    }
}

func init() {
	register([]string{".go"}, go_lex)
}