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
)

const (
	INVALID     TokenType = -2
	END                   = -1
	KEYWORD               = 1
	IDENTIFIER            = 2
	PUNCTUATION           = 3
	LITERAL               = 4
	CHARLITERAL           = 5
	STRING                = 6
	STRINGWORD            = 7
	COMMENT               = 8
	COMMENTWORD           = 9
)

func TypeString(l TokenType) string {
	switch l {
	case KEYWORD:
		return "KEYWORD"
	case IDENTIFIER:
		return "IDENTIFIER"
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
