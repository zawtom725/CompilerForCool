## Compiler Construction for an Object-Oriented Language: COOl
In this repository, I implement half of a compiler that transform high-level COOL programs into LLVM IR code. Then, the LLVM infrastracture can be utilized to further compile the IR code into executable binaries.

The front-end of the compiler should consist of four parts: a lexical scanner, a parse, a semantic checker and an intermediate code generator. I implemented the lexical scanner, a LL[1] parse and the intermediate code generator. The source code is contained in this repostory. I did not implement the semantic checker. However, an usable `semant` is provided in the `bin` folder.

### How to run the code
All the executable binaries are in the `bin` folder. To run:
```
lexer validInput.cl | parser | semant | cgen > outputIR.ll
```