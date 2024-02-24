package main

import (
	"fmt"
	"golexers"
	"html"
	"io"
	"os"
	"unicode/utf8"
)

type FormatOptions struct {
	LineNumbers bool
}

func Format(filename string, options FormatOptions, w io.Writer) error {
	r, err := os.Open(filename)

	if err != nil {
		fmt.Fprintf(os.Stderr, "can not open %s %s\n", filename, err)
		return err
	}
	defer r.Close()
	input, _ := io.ReadAll(r)
	lex := golexers.NewLexer(filename, input)
	//fmt.Fprintf(w, ("do analyze\n")
	if lex == nil {
		fmt.Fprintf(os.Stderr, "no lexer for %s\n", filename)
		return nil
	}

	fmt.Fprintf(w, `
<html><head>
<style>
.file { font-family: courier; border-collapse: collapse; }
.ln { background-color: lightgray; text-align: right; }
.KEYWORD { color: blue; }
.KEYWORD_TYPE { color: blue; }
.COMMENTWORD { color: green; }
.COMMENT { color: green; }
.STRING { color: brown; }
.STRINGWORD { color: brown; }
</style>
</head><body>`)
	fmt.Fprintf(w, "<table class=\"file\">\n")
	lastEnd := 0
	line := 1
	fmt.Fprintf(w, "<tr>")
	if options.LineNumbers {
		fmt.Fprintf(w, "<td class=\"ln\">%d</td>", line)
	}
	fmt.Fprintf(w, "<td class=\"ltxt\" id=\"%d\">", line)
	col := 0
	for {
		tk := lex.Lex()
		//fmt.Fprintf(w, ("lex returns %d\n", l)
		if tk == golexers.END {
			break
		}

		pos, end := lex.TokenPos()
		for pos > lastEnd {
			r, size := utf8.DecodeRune(input[lastEnd:pos])
			switch r {
			case ' ':
				fmt.Fprintf(w, "&nbsp;")
			case '<':
				fmt.Fprintf(w, "&lt;")
			case '>':
				fmt.Fprintf(w, "&gt;")
			case '&':
				fmt.Fprintf(w, "&amp;")
			case '\t':
				fmt.Fprintf(w, "<span class=\"TAB\">")
				fmt.Fprintf(w, "&nbsp;")
				col++
				for col%4 != 0 {
					fmt.Fprintf(w, "&nbsp;")
					col++
				}
				col--
				fmt.Fprintf(w, "</span>")
			case '\n':
				col = 0
				line++
				fmt.Fprintf(w, "</td></tr>\n<tr>")
				if options.LineNumbers {
					fmt.Fprintf(w, "<td class=\"ln\">%d</td>", line)
				}
				fmt.Fprintf(w, "<td class=\"ltxt\" id=\"%d\">", line)
			default:
				fmt.Fprintf(w, "%c", r)
			}
			col++
			lastEnd += size
		}
		s := string(input[pos:end])
		col += len(s)
		fmt.Fprintf(w, "<span class=\"%s\">%s</span>", golexers.TypeString(tk), html.EscapeString(s))
		lastEnd = end
		//fmt.Fprintf(w, ("%d: next token %s line %d %d-%d \"%s\"\n", lex.Line(), lexers.TypeString(tk), lex.Line(), pos, end, lex.Token())
	}

	fmt.Fprintf(w, "</table></body></html>\n")
	return nil
}
