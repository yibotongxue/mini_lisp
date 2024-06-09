; This a test
(define (factorial n)
  (if (= n 0)
      1
      (* n (factorial (- n 1)))))
; This a test.
(display "5! = ")
(display (factorial 5))
(newline)

(define my-list '(1 2 3 4 5))

(display "my-list is: ")
(display my-list)
(newline)

(display "my-list is null? ")
(if (null? my-list) (display "true") (display "false"))
(newline)

(display "The first element of my-list is ")
(displayln (car my-list))

(display "The element of my-list left are ")
(display (cdr my-list))
(newline)
