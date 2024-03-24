golexers
========

go package for lexing various languages enough to support search by word type and simpler syntax highlighting.
The lexers themselves are built using the re2c https://re2c.org/ tool.

Generates pure go library so no issues with requiring `cgo`.

Is not as sophisticated as tree-sitter but hopefully good enough for highlighting and extracting words
or other tokens for simple more text based indexing of code. No guarantees though there aren't misinterpreted
cases with some languages.

Includes some simple command line tools: `lex` to just print tokens in the input file and `format` to generate
html formatted output based on the output of this library.