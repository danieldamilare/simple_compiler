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
    
