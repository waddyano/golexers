package golexers

//import "fmt"

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

func txt_lex(in *Input) TokenType {
	for {
        //fmt.Printf("start at %d, us %d\n", in.cursor, in.unmatched_start)
		in.token = in.cursor
    /*!re2c
        wsp { continue }
		newline { in.bolcursor = in.cursor; in.line += 1; continue }

        * { if (in.unmatched_start < 0 ) { in.unmatched_start = in.token; in.unmatched_token = PUNCTUATION }; continue }
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