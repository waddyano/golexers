package main

import (
	"flag"
	"fmt"
	"io"
	"os"
	"slices"

	"github.com/waddyano/golexers"
)

func main() {
	trace := flag.Bool("t", false, "trace each token")
	flag.Parse()
	if flag.NArg() > 0 {
		filename := flag.Arg(0)
		f, err := os.Open(filename)
		if err != nil {
			fmt.Fprintf(os.Stderr, "failed to open %s: %s\n", filename, err.Error())
			os.Exit(1)
		}
		defer f.Close()
		bytes, _ := io.ReadAll(f)
		lex := golexers.NewLexer(filename, bytes)
		words := make(map[string]bool)
		curLine := -1
		for {
			tok := lex.Lex()
			if tok == golexers.END {
				break
			}
			if curLine != lex.Line() {
				curLine = lex.Line()
				fmt.Printf("%3d:  %s\n", curLine, lex.LineText())
			}
			if *trace {
				fmt.Printf("%s %t %s\n", golexers.TypeString(tok), tok.IsWord(), lex.Token())
			}
			if tok.IsWord() {
				words[string(lex.Token())] = true
			}
		}
		wordsArray := make([]string, 0)
		for word := range words {
			wordsArray = append(wordsArray, word)
		}
		slices.Sort(wordsArray)
		for _, word := range wordsArray {
			fmt.Printf("%s\n", word)
		}
	}
}
