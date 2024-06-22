;; Mini Lisp Test File: test.scm

;; Test arithmetic operations
(displayln (+ 1 2)) ; Expected output: 3
(displayln (* 3 4)) ; Expected output: 12
(displayln (- 10 5)) ; Expected output: 5
(begin (display (/ 20 4)) (newline)) ; Expected output: 5

;; Test special forms
(define x 10)
(define y 20)
(displayln 
    (if (< x y) 
        "x is less than y" 
          "x is not less than y"
          )
      ) ; Expected output: "x is less than y"
(begin
 (display (and #t #f))
 (newline)
) ; Expected output: #f
(displayln (or #t #f)) ; Expected output: #t

;; Test user-defined function
(define (square x) (* x x))
(displayln (square 5)) ; Expected output: 25

;; Test list manipulation
(define my-list '(1 2 3 4 5))
(displayln (car my-list)) ; Expected output: 1
(displayln (cdr my-list)) ; Expected output: (2 3 4 5)
(displayln (length my-list)) ; Expected output: 5

;; Test reading from standard input
(displayln"Enter a number: ")
(define user-input (read))
(displayln user-input)

;; Test lambda function
(define add5 (lambda (x) (+ x 5)))
(displayln (add5 10)) ; Expected output: 15

;; Test let expression
(let ((a 2) (b 3))
  (displayln (+ a b))) ; Expected output: 5

;; Test quasiquote
(define name "Alice")
(define age 30)
(displayln
 `(My name is ,name and I am ,age years old.)) ; Expected output: "My name is Alice and I am 30 years old."

;; Add more test cases as needed
;; Mini Lisp Test File: test.scm

;; Define a function to calculate the factorial of a number
(define (factorial n)
  (if (= n 0)
      1
      (* n (factorial (- n 1)))))

;; Test the factorial function
(begin 
 (display (factorial 5))
(newline)) ; Expected output: 120

;; Define a list of numbers
(define my-list '(1 2 3 4 5))

;; Print the list
(displayln my-list) ; Expected output: (1 2 3 4 5)

;; Calculate the sum of the list
(define (sum-list lst)
  (if (null? lst)
      0
      (+ (car lst) (sum-list (cdr lst)))))

;; Test the sum-list function
(displayln 
    (sum-list my-list)) ; Expected output: 15

;; Define a rational number (3/4)
(define my-rational (make-rational 3 4))

;; Print the rational number
(displayln my-rational) ; Expected output: 3/4