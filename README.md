# C-Compiler
C Compiler project for Imperial College Instruction Architecture and Compiler (IAC) Year 2 Course, compiled for ANSI C (C90) grammar on the RV32IMFD architecture with C++, Yacc, Lex.
Received a grade of 85%, passing 167/201 test cases.

## Features
Fully Implemented:
1. `float`, `int`, `char`, `double`
2. `if`, `while`, `for`, `switch`, `enum`, `break`, `continue`, ternary logic
3. arithmetic and logical operations
4. dereferencing and addressing pointers
5. arrays
6. character literals
7. block, functional and global scoping
8. functions taking up more than 8 inputs
9. strings
10. sizeof

## To Run
In root directory,
```bash
./test.sh
```

## 🏗️ Architecture

The compiler is structured in distinct phases:
1. **Lexical Analysis:** Converts source code into tokens.
2. **Parsing:** Uses Yacc and Flex to build an Abstract Syntax Tree (AST).
3. **Code Generation:** Traverses the AST and emits RISC-V assembly code.

System flow:
```
Source File → Lexer → Parser → AST → Code Generator → RISC-V Assembly
```

## 🔗 Useful References

- [ANSI C Grammar](https://www.lysator.liu.se/c/ANSI-C-grammar-l.html)
- [RISC-V Reference Manual](https://cs.sfu.ca/~ashriram/Courses/CS295/assets/notebooks/RISCV/RISCV_CARD.pdf)
- [Godbolt RISC-V Compiler Explorer](https://godbolt.org/z/vMMnWbsff)

## 🌟 Acknowledgements
Completed as pair work with [@parthak314](https://github.com/parthak314). 
This project is based on coursework and resources from Imperial College London, under the supervision of [John Wickerson](https://johnwickerson.github.io/).  
