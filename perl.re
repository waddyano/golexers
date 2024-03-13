package golexers

/*!include:re2c "unicode_categories.re" */

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

func perl_lex_str(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
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

func perl_lex_long_str(in *Input, start bool) TokenType {
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
        wsp = [ \f\t\v\r]+;
        wsp                  { continue }

		"`" { in.state = STATE_NORMAL; return STRING }

        word        		{ return STRINGWORD }
	*/
	}
}

func perl_lex_eol_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { continue }
        "\n"                 { in.state = STATE_NORMAL; in.cursor -= 1; return END }
        $                    { return END }
        word		         { return COMMENTWORD }
        [^a-zA-Z_0-9\n]+     { return COMMENT }
	*/
	}
}

func perl_lex(in *Input) TokenType {
	for {
		in.token = in.cursor
        //fmt.Printf("start at %d in state %d\n", in.token, in.state)
		if in.state == STATE_STRINGLITERAL || in.state == STATE_CHARLITERAL {
			t := perl_lex_str(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_LONGSTRINGLITERAL) {
			t := perl_lex_long_str(in, false)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_EOLCOMMENT) {
			t := perl_lex_eol_comment(in)
			if t >= 0 {
				return t
			}
		} 

	    was_bol := in.bol
		in.bol = false
    /*!re2c
		newline = [\n];
		"\\" { continue }
        wsp { in.bol = was_bol; continue }
		newline { in.bol = true; in.bolcursor = in.cursor; in.line += 1; continue }

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
		"#" {  in.state = STATE_EOLCOMMENT; return COMMENT }

        "and" { return KEYWORD }
        "case" { return KEYWORD }
        "cmp" { return KEYWORD }
        "continue" { return KEYWORD }
		"CORE" { return KEYWORD }
		"do" { return KEYWORD }
        "else" { return KEYWORD }
        "elsif" { return KEYWORD }
        "eq" { return KEYWORD }
        "for" { return KEYWORD }
        "foreach" { return KEYWORD }
        "ge" { return KEYWORD }
        "gt" { return KEYWORD }
		"if" { return KEYWORD }
        "last" { return KEYWORD }
        "le" { return KEYWORD }
        "lock" { return KEYWORD }
        "lt" { return KEYWORD }
        "m" { return KEYWORD }
        "map" { return KEYWORD }
        "my" { return KEYWORD }
        "ne" { return KEYWORD }
        "next" { return KEYWORD }
        "no" { return KEYWORD }
        "or" { return KEYWORD }
        "our" { return KEYWORD }
        "package" { return KEYWORD }
        "print" { return KEYWORD }
        "q" { return KEYWORD }
        "qq" { return KEYWORD }
        "qr" { return KEYWORD }
        "qw" { return KEYWORD }
        "qx" { return KEYWORD }
        "redo" { return KEYWORD }
        "require" { return KEYWORD }
        "reset" { return KEYWORD }
        "return" { return KEYWORD }
        "s" { return KEYWORD }
        "sub" { return KEYWORD }
        "then" { return KEYWORD }
        "tr" { return KEYWORD }
        "unless" { return KEYWORD }
        "until" { return KEYWORD }
        "use" { return KEYWORD }
        "while" { return KEYWORD }
        "xor" { return KEYWORD }
        "y" { return KEYWORD }

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
	Register([]string{".pl", ".pm"}, perl_lex)
}