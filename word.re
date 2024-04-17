/*!include:re2c "unicode_categories.re" */
/*!re2c
wstart				= L | Nl | "_";
nwstart				= L | Nl | Nd | "_"; // for "words" that start with numbers (not code)
wcontinue 			= wstart | "$" | Mn | Mc | Nd | Pc | [\u200D\u05F3];
word  				= wstart wcontinue*;
nword  				= nwstart wcontinue*;
wsp                 = [ \f\t\v\r\u00a0]+;
newline             = [\n];
*/