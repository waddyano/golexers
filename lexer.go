//go:generate re2c --lang go --no-generation-date comments.re -8 -o comments.go
//go:generate re2c --lang go --no-generation-date cpp.re -8 -o cpp.go
//go:generate re2c --lang go --no-generation-date csharp.re -8 -o csharp.go
//go:generate re2c --lang go --no-generation-date java.re -8 -o java.go
//go:generate re2c --lang go --no-generation-date js.re -8 -o js.go
//go:generate re2c --lang go --no-generation-date py.re -8 -o py.go
//go:generate re2c --lang go --no-generation-date go.re -8 -o go.go
//go:generate re2c --lang go --no-generation-date perl.re -8 -o perl.go
//go:generate re2c --lang go --no-generation-date txt.re -8 -o txt.go
//go:generate re2c --lang go --no-generation-date xml.re -8 -o xml.go
//go:generate re2c --lang go --no-generation-date commented_txt.re -8 -o commented_txt.go

package golexers

import (
	"bytes"
	"fmt"
	"os"
	"path/filepath"

	"golang.org/x/text/encoding/unicode"
	"golang.org/x/text/transform"
)

type LexFunc func(input *Input) TokenType

var langMap = map[string]LexFunc{}

func Register(exts []string, lexFunc LexFunc) {
	for _, ext := range exts {
		//fmt.Printf("Register %s\n", ext)
		langMap[ext] = lexFunc
	}
}

func RegisterAlias(alias string, ext string) {
	function, present := langMap[ext]
	if !present {
		fmt.Fprintf(os.Stderr, "trying to register alias for unknown extension %s\n", ext)
		return
	}
	langMap[alias] = function
}

type Lexer struct {
	input            *Input
	lex_func         LexFunc
	tokenType        TokenType
	tokenLine        int
	tokenStart       int
	tokenEnd         int
	pendingTokenType TokenType
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

	// Convert from UTF16 to UTF8 if we recognise the BOM
	if len(input) > 2 {
		var err error
		if input[0] == 0xff && input[1] == 0xfe {
			input, _, err = transform.Bytes(unicode.UTF16(unicode.LittleEndian, unicode.IgnoreBOM).NewDecoder(), input[2:])
		} else if input[0] == 0xfe && input[1] == 0xff {
			input, _, err = transform.Bytes(unicode.UTF16(unicode.BigEndian, unicode.IgnoreBOM).NewDecoder(), input[2:])
		}
		if err != nil {
			return nil
		}
	}

	in := &Input{
		filename:        filename,
		file:            nil,
		data:            input,
		unmatched_start: -1,
		cursor:          cur,
		marker:          0,
		token:           -1,
		limit:           len(input),
		line:            1,
		state:           STATE_NORMAL,
		eof:             false,
		bolcursor:       0,
	}

	lex_func := langMap[filepath.Ext(filename)]
	return &Lexer{input: in, lex_func: lex_func, pendingTokenType: INVALID}
}

func (lexer *Lexer) Lex() TokenType {
	if lexer.pendingTokenType != INVALID {
		lexer.tokenType = lexer.pendingTokenType
		lexer.pendingTokenType = INVALID
	} else {
		tt := lexer.lex_func(lexer.input)
		//fmt.Printf("returned %s\n", TypeString(tt))
		if lexer.input.unmatched_start >= 0 {
			//fmt.Printf("got token %s us %d %d\n", lexer.input.data[lexer.input.unmatched_start:lexer.input.token], lexer.input.unmatched_start, lexer.input.token)
			lexer.pendingTokenType = tt
			lexer.tokenLine = lexer.input.line
			lexer.tokenType = lexer.input.unmatched_token
			lexer.tokenStart = lexer.input.unmatched_start
			lexer.tokenEnd = lexer.input.token
			lexer.input.unmatched_start = -1
			return lexer.tokenType
		} else {
			lexer.tokenType = tt
		}
	}
	lexer.tokenStart = lexer.input.token
	lexer.tokenEnd = lexer.input.cursor
	lexer.tokenLine = lexer.input.line
	return lexer.tokenType
}

func (lexer *Lexer) Line() int {
	return lexer.tokenLine
}

func (lexer *Lexer) TokenType() TokenType {
	return lexer.tokenType
}

func (lexer *Lexer) TokenPos() (int, int) {
	return lexer.tokenStart, lexer.tokenEnd
}

func (lexer *Lexer) Token() []byte {
	return lexer.input.data[lexer.tokenStart:lexer.tokenEnd]
}

func (lexer *Lexer) LineText() []byte {
	e := bytes.IndexByte(lexer.input.data[lexer.input.bolcursor:], '\n')
	if e == -1 {
		e = len(lexer.input.data)
	} else {
		e += lexer.input.bolcursor
	}
	return lexer.input.data[lexer.input.bolcursor:e]
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
