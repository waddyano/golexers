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

func java_lex_str(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { continue }
        $                    { return -1 }
        "\""                 { if in.state != STATE_STRINGLITERAL { continue }; in.state = STATE_NORMAL; return END }
        "\'"                 { if in.state != STATE_CHARLITERAL { continue }; in.state = STATE_NORMAL; return END }
        [a-zA-Z_0-9]+        { return STRINGWORD }
        "\\a"                { continue }
        "\\b"                { continue }
        "\\f"                { continue }
        "\\n"                { continue }
        "\\r"                { continue }
        "\\t"                { continue }
        "\\v"                { continue }
        "\\\\"               { continue }
        "\\'"                { continue }
        "\\\""               { continue }
        "\\?"                { continue }
        //"\\" [0-7]{1,3}      { lex_oct(in.tok, in.cur, u); continue; }
        //"\\u" [0-9a-fA-F]{4} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\U" [0-9a-fA-F]{8} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\x" [0-9a-fA-F]+   { if (!lex_hex(in.tok, in.cur, u)) return false; continue; }	
	*/
	}
}

func java_lex_raw_str(in *Input, start bool) TokenType {
	for {
		if start {
			in.raw_str_delim = in.data[in.token + 2:in.cursor - 1]
			start = false
		}
		in.token = in.cursor
    /*!re2c
        *                    { continue }
        $                    { return -1 }
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }

		dchar = [a-zA-Z0-9_{}[\]#<>%:;.?*+-/^&|~!=,"’];
        ")" dchar* "\""      { if bytes.Compare(in.raw_str_delim, in.data[in.token+1:in.cursor-1]) != 0 { 
									//fmt.Printf("%s:%d: not end %s\n", in.filename, in.line, in.data[in.token+1:in.cursor-1]); 
									continue
								}
								in.state = STATE_NORMAL; in.raw_str_delim = nil; return END }
        [a-zA-Z_0-9]+        { return STRINGWORD }
	*/
	}
}

func java_lex_ml_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { continue }
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }
        "*/"                 { in.state = STATE_NORMAL; return END }
        $                    { return END }
        [a-zA-Z_0-9]+        { return COMMENTWORD }
	*/
	}
}

func java_lex(in *Input) TokenType {
	for {
		in.token = in.cursor
        //fmt.Printf("start at %d\n", in.token)
		if in.state == STATE_STRINGLITERAL || in.state == STATE_CHARLITERAL {
			t := java_lex_str(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_RAWSTRINGLITERAL) {
			t := java_lex_raw_str(in, false)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_EOLCOMMENT) {
			t := lex_eol_comment(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_MLCOMMENT) {
			t := java_lex_ml_comment(in)
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

		"#" [^\n]* { continue }
		"\"" { in.state = STATE_STRINGLITERAL; t := java_lex_str(in); if t >= 0 { return t }; continue }
		"\'" { in.state = STATE_CHARLITERAL; t := java_lex_str(in); if t >= 0 { return t }; continue }
		//dchar = [a-zA-Z0-9_{}[\]#<>%:;.?*+-/^&|~!=,"’];
		"R\"" dchar * "(" { in.state = STATE_RAWSTRINGLITERAL; t := java_lex_raw_str(in, true); if t >= 0 { return t }; continue }
		"//" { in.state = STATE_EOLCOMMENT; t := lex_eol_comment(in); if t >= 0 { return t }; continue }
		"/*" { in.state = STATE_MLCOMMENT; t := java_lex_ml_comment(in); if t >= 0 { return t }; continue }

        "abstract" { return KEYWORD }
        "assert" { return KEYWORD }
        "boolean" { return KEYWORD }
        "break" { return KEYWORD }
        "byte" { return KEYWORD }
        "case" { return KEYWORD }
        "catch" { return KEYWORD }
        "char" { return KEYWORD }
        "class" { return KEYWORD }
        "const" { return KEYWORD }
        "continue" { return KEYWORD }
        "default" { return KEYWORD }
        "do" { return KEYWORD }
        "double" { return KEYWORD }
        "else" { return KEYWORD }
        "enum" { return KEYWORD }
        "extends" { return KEYWORD }
        "final" { return KEYWORD }
        "finally" { return KEYWORD }
        "float" { return KEYWORD }
        "for" { return KEYWORD }
        "goto" { return KEYWORD }
        "if" { return KEYWORD }
        "implements" { return KEYWORD }
        "import" { return KEYWORD }
        "instanceof" { return KEYWORD }
        "int" { return KEYWORD }
        "long" { return KEYWORD }
        "native" { return KEYWORD }
        "new" { return KEYWORD }
        "package" { return KEYWORD }
        "private" { return KEYWORD }
        "protected" { return KEYWORD }
        "public" { return KEYWORD }
        "return" { return KEYWORD }
        "short" { return KEYWORD }
        "static" { return KEYWORD }
        "strictfp" { return KEYWORD }
        "static_cast" { return KEYWORD }
        "super" { return KEYWORD }
        "switch" { return KEYWORD }
        "synchronized" { return KEYWORD }
        "this" { return KEYWORD }
        "throw" { return KEYWORD }
        "throws" { return KEYWORD }
        "try" { return KEYWORD }
        "void" { return KEYWORD }
        "volatile" { return KEYWORD }
        "while" { return KEYWORD }

        "false" { return LITERAL }
        "true" { return LITERAL }
        "null" { return LITERAL }

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
	Register([]string{".java"}, java_lex)
}