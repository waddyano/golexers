golexers
========

go package for lexing various languages enough to support search by word type and simpler syntax highlighting.
The lexers themselves are built using the re2c https://re2c.org/ tool.

Includes some simple command line tools: `lex` to just print tokens in the input file and `format` to generate
html formatted output based on the output of this library.