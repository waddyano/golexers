package golexers

/*!include:re2c "word.re" */

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
        "\"\""               { return STRING }
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
        wsp                  { continue }

		"`"                  { in.state = STATE_NORMAL; return STRING }

        word        		 { return STRINGWORD }
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
			t := lex_eol_comment(in)
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

    /*!re2c
		"\\" { continue }
        wsp { continue }
		newline { in.bolcursor = in.cursor; in.line += 1; continue }

        * { fmt.Printf("%s: %d: unrecognised character %2x\n", in.filename, in.line, in.data[in.cursor-1]); continue }
        $ { return END }
        "\uFEFF" { continue } // ignore BOM in the middle
		
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

        "@"?"abstract" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"as" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"base" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"bool" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"break" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"byte" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"case" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"catch" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"char" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"checked" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"class" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"const" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"continue" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"decimal" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"default" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"delegate" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"do" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"double" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"else" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"enum" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"event" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"explicit" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"extern" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"false" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"finally" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"fixed" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"for" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"forech" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"goto" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"if" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"implicit" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"in" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"int" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"interface" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"internal" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"is" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"lock" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"long" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"namespace" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"new" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"null" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"object" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"operator" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"out" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"override" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"params" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"private" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"protected" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"public" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"readonly" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"ref" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"return" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"sbyte" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"sealed" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"short" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"sizeof" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"stackalloc" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"static" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"string" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"struct" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"switch" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"this" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"throw" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"true" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"try" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"typeof" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"uint" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"ulong" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"unchecked" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"unsafe" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"ushort" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"using" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"value" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"var" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"virtual" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"void" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"volatile" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"while" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }
        "@"?"yield" { if in.data[in.token] == '@' { in.token++; return IDENTIFIER }; return KEYWORD }

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
        "@"?id { if in.data[in.token] == '@' { in.token++ }; return IDENTIFIER }
    */
    }
}

func init() {
	Register([]string{".cs"}, csharp_lex)
}