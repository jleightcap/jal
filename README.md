# JAL (JAL isn't A Lisp)

JAL is a typed, imperative, systems-oriented programming language.
The aim is to express C-like programs with a simpler, functional syntax; including some quality of life improvements.

*JAL is in very early stages of development and currently only supports emitting Z80 assembly.*

# Examples

- Hello World
```
(include "lcd.lbl")
(defun (main `int)
    (print "hello, world!")
    (ret 0)
)
```

- Fibonacci
```
# calculate the nnth fibonacci number with recursion
(defun (fib `int (nn `int))
    (ret
        (? (<= nn 1)
            nn
            (+ (fib (- nn 1)) (fib (- nn 2)))
        )
    )
)
(defun (main `int)
    (ret (fib 20))
)
```
