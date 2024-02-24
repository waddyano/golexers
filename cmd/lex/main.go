package main

import (
	"flag"
	"fmt"
	"golexers"
	"io"
	"os"
	"slices"
)

func main() {
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
		for {
			tok := lex.Lex()
			if tok == golexers.END {
				break
			}
			if tok == golexers.KEYWORD || tok == golexers.IDENTIFIER || tok == golexers.COMMENTWORD || tok == golexers.STRINGWORD {
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
