# Kommando
*Kommando* is a Programming language that uses german keywords. The language is formaly defined by a grammar with the following production rules, where nonterminal symbols are written in square braces, terminal symbols written normally and $return$ is the starting symbol. All words, that can be derived by these production rules are valid syntax in the Kommando programming language. The grammar is contextfree, which means, that for each production rule the left side is a single nonterminal symbol and the right side a combination of nonterminal and terminal symbols. This property of the grammar makes it a lot easier to define a parser for a compiler.
```bnf
<return>   ::= "zurueck" <expr> ";"

<expr>     ::= <int_lit>
             | <int_lit> <arit_op> <int_lit>

<int_lit>  ::= <digit> <int_lit>
             | <digit>
<arit_op>  ::= "+"
             | "-"
             | "*"
             | "/"



