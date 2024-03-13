package golexers

import (
	"bytes"
    "fmt"
)

/*!include:re2c "unicode_categories.re" */
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

func cpp_lex_str(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { continue }
        $                    { return -1 }
        "\""                 { if in.state != STATE_STRINGLITERAL { return STRING }; in.state = STATE_NORMAL; return STRING }
        "\'"                 { if in.state != STATE_CHARLITERAL { return STRING }; in.state = STATE_NORMAL; return STRING }
		wstart				= L | Nl | [$_];
		wcontinue 			= wstart | Mn | Mc | Nd | Pc | [\u200D\u05F3];
		word  				= wstart wcontinue*;
        word		        { return STRINGWORD }
        wsp = [ \f\t\v\r]+;
        wsp                  { continue }
        "\\b"                { return STRING }
        "\\f"                { return STRING  }
        "\\n"                { return STRING  }
        "\\r"                { return STRING  }
        "\\t"                { return STRING  }
        "\\v"                { return STRING  }
        "\\\\"               { return STRING  }
        "\\'"                { return STRING  }
        "\\\""               { return STRING  }
        "\\?"                { return STRING  }
        //"\\" [0-7]{1,3}      { lex_oct(in.tok, in.cur, u); continue; }
        //"\\u" [0-9a-fA-F]{4} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\U" [0-9a-fA-F]{8} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\x" [0-9a-fA-F]+   { if (!lex_hex(in.tok, in.cur, u)) return false; continue; }	
	*/
	}
}

func cpp_lex_raw_str(in *Input, start bool) TokenType {
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

		dchar = [a-zA-Z0-9_{}[\]#<>%:;.?*+-/^&|~!=,"’];
        ")" dchar* "\""      { if bytes.Compare(in.raw_str_delim, in.data[in.token+1:in.cursor-1]) != 0 { 
									//fmt.Printf("%s:%d: not end %s\n", in.filename, in.line, in.data[in.token+1:in.cursor-1]); 
									continue
								}
								in.state = STATE_NORMAL; in.raw_str_delim = nil; return END }
        word        		{ return STRINGWORD }
	*/
	}
}

func cpp_lex_eol_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { continue }
        "\n"                 { in.state = STATE_NORMAL; in.cursor -= 1; return END }
        $                    { return END }
        word        		 { return COMMENTWORD }
        [^a-zA-Z_0-9 \t\n]+  { return COMMENT }
	*/
	}
}

func cpp_lex_ml_comment(in *Input) TokenType {
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

func cpp_lex(in *Input) TokenType {
	for {
		in.token = in.cursor
        //fmt.Printf("start at %d\n", in.token)
		if in.state == STATE_STRINGLITERAL || in.state == STATE_CHARLITERAL {
			t := cpp_lex_str(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_RAWSTRINGLITERAL) {
			t := cpp_lex_raw_str(in, false)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_EOLCOMMENT) {
			t := cpp_lex_eol_comment(in)
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
		newline = [\n];
		"\\" { continue }
        wsp { in.bol = was_bol; continue }
		newline { in.bol = true; in.bolcursor = in.cursor; in.line += 1; continue }

        * { fmt.Printf("%s: %d: discarded match %2x\n", in.filename, in.line, in.data[in.cursor-1]); continue }
        $ { return END }

        "@" { continue } // Objective-c - ignore it for testing (e.g. on llvm)

        decimal = [1-9][0-9]*;
        hex = "0x" [0-9a-fA-F]+;
        octal = "0" [0-7]*;

        decimal { return LITERAL }
        hex { return LITERAL }
        octal { return LITERAL }

        "#" [^\n]* { continue } // preprocessor
        "\"" { in.state = STATE_STRINGLITERAL; return STRING }
		"\'" { in.state = STATE_CHARLITERAL; return STRING }
		//dchar = [a-zA-Z0-9_{}[\]#<>%:;.?*+-/^&|~!=,"’];
		"R\"" dchar * "(" { in.state = STATE_RAWSTRINGLITERAL; t := cpp_lex_raw_str(in, true); if t >= 0 { return t }; continue }
		"//" { in.state = STATE_EOLCOMMENT; return COMMENT }
		"/*" { in.state = STATE_MLCOMMENT; return COMMENT }

        "auto" { return KEYWORD }
        "bool" { return KEYWORD_TYPE }
        "break" { return KEYWORD }
        "case" { return KEYWORD }
        "catch" { return KEYWORD }
        "char" { return KEYWORD_TYPE }
        "class" { return KEYWORD }
        "const" { return KEYWORD_TYPE }
        "consteval" { return KEYWORD }
        "constexptr" { return KEYWORD }
        "const_cast" { return KEYWORD }
        "continue" { return KEYWORD }
        "decltype" { return KEYWORD }
        "delete" { return KEYWORD }
        "do" { return KEYWORD }
        "double" { return KEYWORD_TYPE }
        "dynamic_cast" { return KEYWORD }
        "else" { return KEYWORD }
        "enum" { return KEYWORD }
        "explicit" { return KEYWORD }
        "extern" { return KEYWORD }
        "false" { return KEYWORD }
        "float" { return KEYWORD_TYPE }
        "for" { return KEYWORD }
        "friend" { return KEYWORD }
        "goto" { return KEYWORD }
        "if" { return KEYWORD }
        "inline" { return KEYWORD }
        "int" { return KEYWORD }
        "long" { return KEYWORD_TYPE }
        "mutable" { return KEYWORD }
        "namespace" { return KEYWORD }
        "new" { return KEYWORD }
        "nullptr" { return KEYWORD }
        "operator" { return KEYWORD }
        "private" { return KEYWORD }
        "protected" { return KEYWORD }
        "public" { return KEYWORD }
        "reinterpret_cast" { return KEYWORD }
        "return" { return KEYWORD }
        "short" { return KEYWORD_TYPE }
        "signed" { return KEYWORD_TYPE }
        "sizeof" { return KEYWORD }
        "static" { return KEYWORD }
        "static_assert" { return KEYWORD }
        "static_cast" { return KEYWORD }
        "struct" { return KEYWORD }
        "switch" { return KEYWORD }
        "template" { return KEYWORD }
        "this" { return KEYWORD }
        "thread_local" { return KEYWORD }
        "throw" { return KEYWORD }
        "true" { return KEYWORD }
        "try" { return KEYWORD }
        "typedef" { return KEYWORD }
        "typeid" { return KEYWORD }
        "typename" { return KEYWORD }
        "union" { return KEYWORD }
        "unsigned" { return KEYWORD_TYPE }
        "unsing" { return KEYWORD }
        "virtual" { return KEYWORD }
        "void" { return KEYWORD_TYPE }
        "volatile" { return KEYWORD_TYPE }
        "wchar_t" { return KEYWORD_TYPE }
        "while" { return KEYWORD }

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
		"+=" { return PUNCTUATION }
		"-=" { return PUNCTUATION }
		"/=" { return PUNCTUATION }
		"*=" { return PUNCTUATION }
		"%=" { return PUNCTUATION }
		"&=" { return PUNCTUATION }
		"|=" { return PUNCTUATION }
		"==" { return PUNCTUATION }
		"!=" { return PUNCTUATION }
		"^=" { return PUNCTUATION }
		"<<=" { return PUNCTUATION }
		">>=" { return PUNCTUATION }
		"<=>" { return PUNCTUATION }
		"<" { return PUNCTUATION }
		">" { return PUNCTUATION }
		"<=" { return PUNCTUATION }
		">=" { return PUNCTUATION }
		"->" { return PUNCTUATION }
		"->*" { return PUNCTUATION }
		"?" { return PUNCTUATION }
		":" { return PUNCTUATION }
		"::" { return PUNCTUATION }

		id_start    = L | Nl | [$_];
		id_continue = id_start | Mn | Mc | Nd | Pc | [\u200D\u05F3];
		id          = id_start id_continue*;
        id { return IDENTIFIER }
    */
    }
}

func init() {
	Register([]string{".c", ".h", ".cc", ".hh", ".cxx", ".hxx", ".cpp", ".hpp"}, cpp_lex)
}