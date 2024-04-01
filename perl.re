package golexers

/*!include:re2c "word.re" */

import (
    "fmt"
)

const STATE_PERL_AFTER_END = STATE_CUSTOM

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
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }
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
        wsp                  { continue }

		"`" { in.state = STATE_NORMAL; return STRING }

        word        		{ return STRINGWORD }
	*/
	}
}

func perl_doc_lex(in *Input) TokenType {
	for {
        //fmt.Printf("start at %d, us %d\n", in.cursor, in.unmatched_start)
		in.token = in.cursor
    /*!re2c
        wsp { continue }
		newline { in.bolcursor = in.cursor; in.line += 1; continue }

        * { if (in.unmatched_start < 0 ) { in.unmatched_start = in.token; in.unmatched_token = COMMENT }; continue }
        $ { return END }

        word { return COMMENTWORD }
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
			t := lex_eol_comment(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_PERL_AFTER_END) {
			// reuse the .txt lexer
			t := perl_doc_lex(in)
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

        "__END__" { in.state = STATE_PERL_AFTER_END; return KEYWORD } // anything after __END__ should just be treated as plain text

		"$" { return PUNCTUATION }
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
	Register([]string{".pl", ".pm", ".t"}, perl_lex)
}