package golexers

import (
	"bytes"
)

const (
	STRING_PROCESS = iota
	STRING_IGNORE
)

// Returns first a code as to how to handle the string - if it is STRING_IGNORE in.cursor is updated
func classify_string(in *Input) int {
	//  Simple look ahead for ending " (may need to handle escapes eventually)
	end := bytes.IndexByte(in.data[in.cursor:], '"')
	if end < 0 {
		return STRING_PROCESS
	}

	// Now look to see if the whole strings has a space in it or not. If it does we process it as words.
	space := bytes.IndexByte(in.data[in.cursor:in.cursor+end], ' ')
	if space < 0 {
		if end > 64 {
			in.cursor += end + 1 // after the ending quote
			return STRING_IGNORE
		}
	}
	return STRING_PROCESS
}
