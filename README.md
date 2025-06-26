# Logik
*Logik* is a Programming language that uses german keywords. The language is formaly defined by a grammar with the following production rules, where nonterminal symbols are written in square braces, terminal symbols written normally and $return$ is the starting symbol. All words, that can be derived by these production rules are valid syntax in the Logik programming language. The grammar is contextfree, which means, that for each production rule the left side is a single nonterminal symbol and the right side a combination of nonterminal and terminal symbols. This property of the grammar makes it a lot easier to define a parser for a compiler.
$$
\begin{align}
[return] \: &\to zurueck\: [expr] ;\\
[expr] \: &\to [int\_lit] \: \big| \: [int\_lit] [arithmetic\_operator] [int\_lit]\\
[int\_lit] \: &\to [digit] [int\_lit] \: \big| \: [digit]


\end{align}
$$

