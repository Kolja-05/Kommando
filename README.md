# Kommando
*Kommando* is a Programming language that uses german keywords. The language is formaly defined by a grammar. The compilation process takes three steps:
1. The programmcode is turned into tokens with a lexer, which instead of single letters contain the words of the source code.
2. From the tokens we construct a abstract syntax tree (ASTS), a structure we can later follow to generete the assembly code.
3. We travel the AST and for each node we generate the assembly code.
4. The generated assembly code now only needs to be assembled, i.e. to to be translated into machinecode, the language, the computer can understand. For that, we use the NASM assambler and GNU linker.
```Kommando
Kommando:
sei x = 4;
sei y = 10;
sei z = y + x;
x = z + 42;
zurueck x;

```
My goal with the *Kommando* language is to achive Turing completeness. A programming language - or rather the formal system defined by that language - is Turing complete, if it can express any computable function. To achive this, a language only needs three things: conditional execution, jumps and mutable variables. I want to implement these features step by step. I already implemented variably declaration and initializiation and assignment of another value and a jump (Springe) command to jump between lables. To make it Turingcomplete I only need Conditional execution i.e. if-statements (Wenn ... Dann).

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
## How to use?
1. Clone the repository
   ```bash
   git clone https://github.com/Kolja-05/Kommando.git

2. Compile the Programm with cmake
   ```bash
   cmake -S . -B ./build/
   cmake --build ./build/
3. Compile your Kommando file
   ```bash
   ./build/kommando example.ko
4. run your compiled output
   ```bash
   ./out
5. See the exit code of your programm
   ```bash
   echo $?
