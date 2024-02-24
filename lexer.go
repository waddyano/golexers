//go:generate re2c --lang go cpp.re -8 -o cpp.go
//go:generate re2c --lang go csharp.re -8 -o csharp.go
//go:generate re2c --lang go java.re -8 -o java.go
//go:generate re2c --lang go py.re -8 -o py.go
//go:generate re2c --lang go go.re -8 -o go.go

package golexers

import (
	"path/filepath"
)

type LexFunc func(input *Input) TokenType

var langMap = map[string]LexFunc{}

func register(exts []string, lexFunc LexFunc) {
	for _, ext := range exts {
		//fmt.Printf("Register %s\n", ext)
		langMap[ext] = lexFunc
	}
}

type Lexer struct {
	input     *Input
	lex_func  LexFunc
	tokenType TokenType
}

func CanLex(filename string) bool {
	ext := filepath.Ext(filename)
	_, found := langMap[ext]
	return found
}

func NewLexer(filename string, input []byte) *Lexer {
	if !CanLex(filename) {
		return nil
	}

	// Skip BOM if there but keep all byte offsets in file correct
	cur := 0
	if len(input) > 3 && input[0] == 0xef && input[1] == 0xbb && input[2] == 0xbf {
		cur = 3
	}

	in := &Input{
		filename: filename,
		file:     nil,
		data:     input,
		cursor:   cur,
		marker:   0,
		token:    -1,
		limit:    len(input),
		line:     1,
		state:    STATE_NORMAL,
		eof:      false,
		bol:      true,
	}

	lex_func := langMap[filepath.Ext(filename)]
	return &Lexer{input: in, lex_func: lex_func}
}

func (lexer *Lexer) Lex() TokenType {
	lexer.tokenType = lexer.lex_func(lexer.input)
	return lexer.tokenType
}

func (lexer *Lexer) Line() int {
	return lexer.input.line
}

func (lexer *Lexer) TokenType() TokenType {
	return lexer.tokenType
}

func (lexer *Lexer) TokenPos() (int, int) {
	return lexer.input.token, lexer.input.cursor
}

func (lexer *Lexer) Token() []byte {
	return lexer.input.data[lexer.input.token:lexer.input.cursor]
}

/*
	for {
		l := lex(in)
		//fmt.Printf("lex returns %d\n", l)
		if l < 0 {
			break
		}
		fmt.Printf("%d: next token %s %d \"%s\"\n", in.line, typeString(l), in.token, in.data[in.token:in.cursor])
	}
}
*/
