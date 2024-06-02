(define l '(12 71 2 15 29 82 87 8 18 66 81 25 63 97 40 3 93 58 53 31 47))
(define (insert x lst) (if (null? lst) (list x) (if (< x (car lst)) (cons x lst) (cons (car lst) (insert x (cdr lst))))))
(define (sort lst) (if (null? lst) '() (insert (car lst) (sort (cdr lst)))))
(print (sort l))
