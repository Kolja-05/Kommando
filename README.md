# Kommando
*Kommando* is a Programming language that uses german keywords. The language is formaly defined by a grammar. The compilation process takes three steps:
1. The programmcode is turned into Tokens with a lexer, which instead of single letters containt the words of the source code.
2. From the tokens we construct a abstract syntax tree (ASTS), a structure we can later follow to generete the assembly code.
3. We travel the AST and for each node we generate the assembly code.
4. The generated assembly code now only needs to be assembled, i.e. to to be translated into machinecode, the language, the computer can understand. For that, we use the NASM assambler and GNU linker.
```Kommando
Kommando:
sei x = 4;
sei y = 10;
Schleife:
Wenn x > 0 dann:
  x = x - 1;
  y = y * x;
  springe Schleife;
Ende

zurueck x;

```
My goal with the *Kommando* language is to achive Turing completeness. A programming language - or rather the formal system defined by that language - is Turing complete, if it can express any computable function. To achive this, a language only needs three things: conditional execution, jumps and mutable variables. I want to implement these features step by step.

