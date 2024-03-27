package golexers

//import "fmt"

const (
    STATE_STARTTAG = STATE_CUSTOM
    STATE_ATTRSTRING = STATE_CUSTOM + 1
)

/*!include:re2c "word.re" */
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

func xml_lex_ml_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { if (in.unmatched_start < 0 ) { in.unmatched_start = in.token; in.unmatched_token = COMMENT }; continue }
        wsp                  { continue }
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }
        "-->"                { in.state = STATE_NORMAL; return COMMENT }
        $                    { return END }
        word       			 { return COMMENTWORD }
	*/
	}
}

func xml_lex_start_tag(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { if (in.unmatched_start < 0 ) { in.unmatched_start = in.token; in.unmatched_token = PUNCTUATION }; continue }
        wsp                  { continue }
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }
        ">"                  { in.state = STATE_NORMAL; return PUNCTUATION }
        "="                  { return PUNCTUATION }
        "\""                 { code := classify_string(in); 
                               if code == STRING_PROCESS {
                                  in.state = STATE_ATTRSTRING
                                  return PUNCTUATION
                               }
                               return STRING
                             }
        $                    { return END }
        word       			 { return IDENTIFIER }
	*/
	}
}

func xml_lex_attr_string(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { if (in.unmatched_start < 0 ) { in.unmatched_start = in.token; in.unmatched_token = PUNCTUATION }; continue }
        wsp                  { continue }
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }
        "\""                 { in.state = STATE_STARTTAG; return PUNCTUATION }
        $                    { return END }
        word       			 { return IDENTIFIER }
	*/
	}
}


func xml_lex_str(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { if (in.unmatched_start < 0 ) { in.unmatched_start = in.token; in.unmatched_token = STRING }; continue }
        wsp                  { continue }
        $                    { return -1 }
        "\""                 { if in.state != STATE_STRINGLITERAL { return STRING }; in.state = STATE_NORMAL; return STRING }
        word		         { return STRINGWORD }
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
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; continue }
        //"\\" [0-7]{1,3}      { lex_oct(in.tok, in.cur, u); continue; }
        //"\\u" [0-9a-fA-F]{4} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\U" [0-9a-fA-F]{8} { lex_hex(in.tok, in.cur, u); continue; }
        //"\\x" [0-9a-fA-F]+   { if (!lex_hex(in.tok, in.cur, u)) return false; continue; }	
	*/
	}
}

func xml_lex(in *Input) TokenType {
	for {
        //fmt.Printf("start at %d, us %d\n", in.cursor, in.unmatched_start)
		in.token = in.cursor
		if (in.state == STATE_STRINGLITERAL) {
			t := xml_lex_str(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_MLCOMMENT) {
			t := xml_lex_ml_comment(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_STARTTAG) {
			t := xml_lex_start_tag(in)
			if t >= 0 {
				return t
			}
		} else if (in.state == STATE_ATTRSTRING) {
			t := xml_lex_attr_string(in)
			if t >= 0 {
				return t
			}
		}

    /*!re2c
        wsp { continue }
		newline { in.bolcursor = in.cursor; in.line += 1; continue }

        * 		{ if (in.unmatched_start < 0 ) { in.unmatched_start = in.token; in.unmatched_token = PUNCTUATION }; continue }
		"<!--" 	{ in.state = STATE_MLCOMMENT; return COMMENT }
		"<" 	{ in.state = STATE_STARTTAG; return PUNCTUATION }
		"\"" 	{ in.state = STATE_STRINGLITERAL; return STRING }
        $ { return END }

        word { return IDENTIFIER }
    */
    }
}

func init() {
	Register([]string{".xml"}, xml_lex)
}