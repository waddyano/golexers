package golexers

/*!include:re2c "unicode_categories.re" */

import (
    "fmt"
)

const (
	STATE_VERBATIMSTRING = STATE_CUSTOM
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

func csharp_lex_str(in *Input) TokenType {
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

func csharp_lex_verbatim_str(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { return STRING }
        $                    { return -1 }
        "\""                 { in.state = STATE_NORMAL; return STRING }
        word		         { return STRINGWORD }
        "\\"                 { return STRING }
        //"\\" [0-7]{1,3}      { lex_oct(in.tok, in.cur, u); continue; }
        //"\\u" [0-9a-fA-F]{4} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\U" [0-9a-fA-F]{8} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\x" [0-9a-fA-F]+   { if (!lex_hex(in.tok, in.cur, u)) return false; continue; }	
	*/
	}
}

func csharp_lex_long_str(in *Input, start bool) TokenType {
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

func csharp_lex_eol_comment(in *Input) TokenType {
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

func csharp_lex_ml_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { return COMMENT }
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }
        "*/"                 { in.state = STATE_NORMAL; return COMMENT }
        $                    { return END }
        word		        { return COMMENTWORD }
	*/
	}
}

func csharp_lex(in *Input) TokenType {
	for {
		in.token = in.cursor
        //fmt.Printf("start at %d\n", in.token)
		if in.state == STATE_STRINGLITERAL || in.state == STATE_CHARLITERAL {
			t := csharp_lex_str(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_LONGSTRINGLITERAL) {
			t := csharp_lex_long_str(in, false)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_EOLCOMMENT) {
			t := csharp_lex_eol_comment(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_MLCOMMENT) {
			t := csharp_lex_ml_comment(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_VERBATIMSTRING) {
			t := csharp_lex_verbatim_str(in)
			if t >= 0 {
				return t
			}
		}

	    was_bol := in.bol
		in.bol = false
    /*!re2c
        scm = "//" [^\n]*;
		newline = [\n];
		"\\" { continue }
        wsp { in.bol = was_bol; continue }
		newline { in.bol = true; in.bolcursor = in.cursor; in.line += 1; continue }

        * { fmt.Printf("%s: %d: match %2x\n", in.filename, in.line, in.data[in.cursor-1]); continue }
        $ { return END }
		
		"#" [^\n]* { continue } // preprocessor
        

		decimal = [1-9][0-9]*;
		hex = "0x" [0-9a-fA-F]+;
		octal = "0" [0-7]*;

		decimal { return LITERAL }
		hex { return LITERAL }
		octal { return LITERAL }

		"@\"" { in.state = STATE_VERBATIMSTRING; return STRING } 
		"\"" { in.state = STATE_STRINGLITERAL; return STRING }
		"\'" { in.state = STATE_CHARLITERAL; return STRING }
		"`" { in.state = STATE_LONGSTRINGLITERAL; in.raw_str_delim = in.data[in.token:in.cursor]; return STRING }
		"//" { in.state = STATE_EOLCOMMENT; return COMMENT }
		"/*" { in.state = STATE_MLCOMMENT; return COMMENT }

        "abstract" { return KEYWORD }
        "as" { return KEYWORD }
        "base" { return KEYWORD }
        "bool" { return KEYWORD }
        "break" { return KEYWORD }
        "byte" { return KEYWORD }
        "case" { return KEYWORD }
        "catch" { return KEYWORD }
        "char" { return KEYWORD }
        "checked" { return KEYWORD }
        "class" { return KEYWORD }
        "const" { return KEYWORD }
        "continue" { return KEYWORD }
        "decimal" { return KEYWORD }
        "default" { return KEYWORD }
        "delegate" { return KEYWORD }
        "do" { return KEYWORD }
        "double" { return KEYWORD }
        "else" { return KEYWORD }
        "enum" { return KEYWORD }
        "event" { return KEYWORD }
        "explicit" { return KEYWORD }
        "extern" { return KEYWORD }
        "false" { return KEYWORD }
        "for" { return KEYWORD }
        "forech" { return KEYWORD }
        "goto" { return KEYWORD }
        "if" { return KEYWORD }
        "implicit" { return KEYWORD }
        "in" { return KEYWORD }
        "int" { return KEYWORD }
        "interface" { return KEYWORD }
        "internal" { return KEYWORD }
        "is" { return KEYWORD }
        "lock" { return KEYWORD }
        "long" { return KEYWORD }
        "namespace" { return KEYWORD }
        "new" { return KEYWORD }
        "null" { return KEYWORD }
        "object" { return KEYWORD }
        "operator" { return KEYWORD }
        "out" { return KEYWORD }
        "override" { return KEYWORD }
        "params" { return KEYWORD }
        "private" { return KEYWORD }
        "protected" { return KEYWORD }
        "public" { return KEYWORD }
        "readonly" { return KEYWORD }
        "ref" { return KEYWORD }
        "return" { return KEYWORD }
        "sbyte" { return KEYWORD }
        "sealed" { return KEYWORD }
        "short" { return KEYWORD }
        "sizeof" { return KEYWORD }
        "stackalloc" { return KEYWORD }
        "static" { return KEYWORD }
        "string" { return KEYWORD }
        "struct" { return KEYWORD }
        "switch" { return KEYWORD }
        "this" { return KEYWORD }
        "throw" { return KEYWORD }
        "true" { return KEYWORD }
        "try" { return KEYWORD }
        "typeof" { return KEYWORD }
        "uint" { return KEYWORD }
        "ulong" { return KEYWORD }
        "unchecked" { return KEYWORD }
        "unsafe" { return KEYWORD }
        "unshort" { return KEYWORD }
        "using" { return KEYWORD }
        "virtual" { return KEYWORD }
        "void" { return KEYWORD }
        "volatile" { return KEYWORD }
        "while" { return KEYWORD }

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
	register([]string{".cs"}, csharp_lex)
}