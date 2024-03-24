package golexers

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

func lex_eol_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { if (in.unmatched_start < 0 ) { in.unmatched_start = in.token; in.unmatched_token = COMMENT }; continue }
		wsp                  { continue }
        "\n"                 { in.state = STATE_NORMAL; in.cursor -= 1; return NEWLINE }
        $                    { return END }
        word        		 { return COMMENTWORD }
	*/
	}
}

func lex_ml_comment(in *Input) TokenType {
	for {
		in.token = in.cursor
    /*!re2c
        *                    { if (in.unmatched_start < 0 ) { in.unmatched_start = in.token; in.unmatched_token = COMMENT }; continue }
        "\n"                 { in.bolcursor = in.cursor; in.line += 1; return NEWLINE }
        "*/"                 { in.state = STATE_NORMAL; return COMMENT }
        $                    { return END }
        word       			 { return COMMENTWORD }
	*/
	}
}
