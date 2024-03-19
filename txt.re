package golexers

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

func txt_lex(in *Input) TokenType {
	for {
		in.token = in.cursor
        //fmt.Printf("start at %d\n", in.token)

	    was_bol := in.bol
		in.bol = false
    /*!re2c
		wstart				= L | Nl | "_";
		wcontinue 			= wstart | "$" | Mn | Mc | Nd | Pc | [\u200D\u05F3];
		word  				= wstart wcontinue*;
        wsp = [ \f\t\v\r]+;

		newline = [\n];
        wsp { in.bol = was_bol; continue }
		newline { in.bol = true; in.bolcursor = in.cursor; in.line += 1; continue }

        * { continue }
        $ { return END }

		decimal = [1-9][0-9]*;
		hex = "0x" [0-9a-fA-F]+;
		octal = "0" [0-7]*;

		decimal { return LITERAL }
		hex { return LITERAL }
		octal { return LITERAL }

        word { return IDENTIFIER }
    */
    }
}

func init() {
	Register([]string{".txt"}, txt_lex)
}