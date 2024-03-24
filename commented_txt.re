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

func commented_txt_lex(in *Input, commentChar byte) TokenType {
	for {
		in.token = in.cursor
		//fmt.Printf("start at %d\n", in.token)
		if (in.state == STATE_EOLCOMMENT) {
			t := lex_eol_comment(in)
			if t >= 0 {
				return t
			}
		}

	/*!re2c
		wsp { continue }
		newline { in.bolcursor = in.cursor; in.line += 1; continue }

		* { if in.data[in.cursor-1] == commentChar { in.state = STATE_EOLCOMMENT; return COMMENT }; continue }
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
	Register([]string{".yaml"}, func (in *Input) TokenType { return commented_txt_lex(in, '#') })
}