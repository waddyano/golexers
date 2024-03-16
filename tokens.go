package golexers

type TokenType int

const (
	STATE_NORMAL = iota
	STATE_CHARLITERAL
	STATE_STRINGLITERAL
	STATE_RAWSTRINGLITERAL
	STATE_LONGSTRINGLITERAL
	STATE_EOLCOMMENT
	STATE_MLCOMMENT
	STATE_CUSTOM // keep last - first for a languag specific state
)

const (
	IS_WORD    = 1 << 8
	IS_STRING  = 1 << 9
	IS_COMMENT = 1 << 10
)
const (
	INVALID      TokenType = -2
	END                    = -1
	KEYWORD                = 1 | IS_WORD
	KEYWORD_TYPE           = 2 | IS_WORD // a keyword (or known identifier) which is a type e.g. types in go
	IDENTIFIER             = 3 | IS_WORD
	BUILTIN                = 4 | IS_WORD // a built in function as used in go or perl
	PUNCTUATION            = 5
	LITERAL                = 6 // literal but not string or char
	CHARLITERAL            = 7
	STRING                 = 8 | IS_STRING // inside a string but not a word
	STRINGWORD             = 9 | IS_WORD | IS_STRING
	COMMENT                = 10 | IS_COMMENT // inside a commant but not a aord
	COMMENTWORD            = 11 | IS_WORD | IS_COMMENT
)

func (tt TokenType) IsWord() bool {
	return (tt & IS_WORD) != 0
}

func (tt TokenType) IsComment() bool {
	return (tt & IS_COMMENT) != 0
}

func (tt TokenType) IsString() bool {
	return (tt & IS_STRING) != 0
}

func TypeString(tt TokenType) string {
	switch tt {
	case KEYWORD:
		return "KEYWORD"
	case KEYWORD_TYPE:
		return "KEYWORD_TYPE"
	case IDENTIFIER:
		return "IDENTIFIER"
	case BUILTIN:
		return "BUILTIN"
	case PUNCTUATION:
		return "PUNCTUATION"
	case LITERAL:
		return "LITERAL"
	case CHARLITERAL:
		return "CHARLITERAL"
	case STRINGWORD:
		return "STRINGWORD"
	case STRING:
		return "STRING"
	case COMMENT:
		return "COMMENT"
	case COMMENTWORD:
		return "COMMENTWORD"
	default:
		return "???"
	}
}
