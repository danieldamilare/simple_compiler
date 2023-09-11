## Simple Compiler
A compiler that compiles a small dialect of BASIC to C.
It supports:
- Numerical Variables
- Basic Arithmetic
- If statements
- While loops
- Print text and numbers
- Input numbers
- Label and goto
- Comments

Example code

```
/* simple sl program for fibonacci */
PRINT "ENTER NUMBER"
INPUT a
LET B = 0
LET C = 1
WHILE a > 0 REPEAT
    LET B = B + C
    PRINT B
    LET C = B + C
    PRINT C
    LET a =  a - 2
ENDWHILE
 
```
        

Based on Let's make a Teeny Tiny compiler tutorial series by [Austin Henley](https://https://austinhenley.com/) 
