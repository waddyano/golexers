package golexers

import "os"

type Input struct {
	filename        string
	file            *os.File
	data            []byte
	unmatched_start int
	unmatched_token TokenType
	cursor          int
	marker          int
	token           int
	limit           int
	line            int
	state           int
	eof             bool
	bolcursor       int
	raw_str_delim   []byte
}

func fill(in *Input) int {
	return 1
}

func peek(in *Input) byte {
	if in.cursor >= in.limit {
		return 0
	}
	return in.data[in.cursor]
}
