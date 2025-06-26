# Kommando
*Kommando* is a Programming language that uses german keywords. The language is formaly defined by a grammar with the following production rules, where nonterminal symbols are written in square braces, terminal symbols written normally and $return$ is the starting symbol. All words, that can be derived by these production rules are valid syntax in the Kommando programming language. The grammar is contextfree, which means, that for each production rule the left side is a single nonterminal symbol and the right side a combination of nonterminal and terminal symbols. This property of the grammar makes it a lot easier to define a parser for a compiler.
```bnf
<program>    ::= "Kommando:" <stmt_list>

<stmt_list>  ::= <stmt_elem> <stmt_list> | ε

<stmt_elem>  ::= <label> <stmt> | <stmt>

<label>      ::= <identifier> ":"

<stmt>       ::= <return>
              | <assign>
              | <goto_stmt>
              | <if_stmt>

<assign>     ::= "sei" <var> "=" <expr> ";"

<var>        ::= <letter> <var_tail>
<var_tail>   ::= <alpha_num> <var_tail> | ε

<if_stmt>    ::= "wenn" <condition> "dann" <stmt>

<condition>  ::= <expr> <comp_op> <expr>

<comp_op>    ::= "==" | "!=" | "<" | ">" | "<=" | ">="

<goto_stmt>  ::= "springe" <identifier> ";"

<return>     ::= "zurueck" <expr> ";"

<expr>       ::= <int_lit>
              | <int_lit> <arit_op> <int_lit>

<int_lit>    ::= <digit> <int_lit> | <digit>

<arit_op>    ::= "+" | "-" | "*" | "/"

<letter>     ::= "a" | "b" | "c" | ... | "z"
              | "A" | "B" | "C" | ... | "Z"

<digit>      ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"

<alpha_num>  ::= <letter> | <digit>

```
My goal with the *Kommando* language is to achive Turing completeness. A programming language - or rather the formal system defined by that language - is Turing complete, if it can express any computable function. To achive this, a language only needs three things: conditional execution. jumps and mutable variables. I want to implement these features step by step.

