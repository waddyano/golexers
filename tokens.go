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
	INVALID      TokenType = -2
	END                    = -1
	KEYWORD                = 1
	KEYWORD_TYPE           = 2 // a keyword (or known identifier) which is a type e.g. types in go
	IDENTIFIER             = 3
	BUILTIN                = 4 // a built in function as used in go or perl
	PUNCTUATION            = 5
	LITERAL                = 6 // literal but not string or char
	CHARLITERAL            = 7
	STRING                 = 8 // inside a string but not a word
	STRINGWORD             = 9
	COMMENT                = 10 // inside a commant but not a aord
	COMMENTWORD            = 11
)

func (tt TokenType) IsWord() bool {
	switch tt {
	case KEYWORD:
	case KEYWORD_TYPE:
	case IDENTIFIER:
	case BUILTIN:
	case STRINGWORD:
	case COMMENTWORD:
		return true
	default:
		return false
	}
	return false
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
		return "BUILT"
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
