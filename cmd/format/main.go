package main

import (
	"flag"
	"fmt"
	"os"
)

func main() {
	flag.Parse()
	if flag.NArg() < 1 {
		fmt.Fprintf(os.Stderr, "must specify path to format\n")
		os.Exit(1)
	}

	filename := flag.Arg(0)
	options := FormatOptions{LineNumbers: true}
	err := Format(filename, options, os.Stdout)
	if err != nil {
		fmt.Fprintf(os.Stderr, "failed: %s\n", err.Error())
	}
}
