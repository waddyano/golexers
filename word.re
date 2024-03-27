/*!include:re2c "unicode_categories.re" */
/*!re2c
wstart				= L | Nl | "_";
wcontinue 			= wstart | "$" | Mn | Mc | Nd | Pc | [\u200D\u05F3];
word  				= wstart wcontinue*;
wsp                 = [ \f\t\v\r]+;
newline             = [\n];
*/