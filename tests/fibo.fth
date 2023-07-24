( Copyright (C) 2023 Mistivia <mistivia@soverin.net> )
( Licensed under AGPLv3. See LICENSE for details. )

: fibo ( recursive fibonacci )
    dup 2 <= if
        1 swap drop
    else
        dup 1 - fibo over 2 - fibo + swap drop
    then
;

8 fibo
21 = assert

depth 0 = assert
