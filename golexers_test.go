package golexers

import (
	"fmt"
	"testing"
)

func TestPyString(t *testing.T) {
	code := `
import sys

def main():
	""" documentation """
	pass
`
	lexer := NewLexer(".py", []byte(code))
	for lexer.Lex() != END {
		fmt.Printf("tok %s %s\n", lexer.Token(), TypeString(lexer.TokenType()))
	}
}

func TestGoString(t *testing.T) {
	code := `
package test

func main() {
	x := ` + "`" + `string` + "`" + `
}
`
	lexer := NewLexer(".go", []byte(code))
	for lexer.Lex() != END {
		fmt.Printf("tok %s %s\n", lexer.Token(), TypeString(lexer.TokenType()))
	}
}
