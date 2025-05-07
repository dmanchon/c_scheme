;; C/Scheme HTTP Routing Example
;; -----------------------------
;; This file defines a minimal routing system for handling HTTP requests
;; in s7 Scheme, designed to be loaded by the C server.

;; -----------------------------
;; Route Table and Registration
;; -----------------------------

(define *routes* '())

(define (add-route method path handler)
  (set! *routes* (cons (list method path handler) *routes*)))

;; -----------------------------
;; Route Lookup
;; -----------------------------

;; Find the first element in lst for which (pred element) is true
(define (find pred lst)
  (cond
    ((null? lst) #f)
    ((pred (car lst)) (car lst))
    (else (find pred (cdr lst)))))

;; -----------------------------
;; Request Handling
;; -----------------------------

;; Main entry: called from C with (method path)
(define (handle-request method path)
  (let* ((route (find (lambda (r)
                        (and (string=? method (car r))
                             (string=? path (cadr r))))
                      *routes*))
         (handler (if route (caddr route)
                      (lambda () (respond 404 "Not Found")))))
    (if (procedure? handler)
        (handler)
        (respond 500 "Internal Server Error: Invalid route handler."))))

;; -----------------------------
;; HTTP Response Helpers
;; -----------------------------

;; Build a full HTTP response string
(define (respond code body)
  (format #f "HTTP/1.1 ~a ~a\r\nContent-Type: text/html\r\nContent-Length: ~a\r\n\r\n~a"
          code (status-text code) (string-length body) body))

;; Map status codes to reason phrases
(define (status-text code)
  (case code
    ((200) "OK")
    ((404) "Not Found")
    (else "Unknown")))


;; TODO: the previous stuff should be injected automatically, user script starts here.
(add-route "GET" "/" 
  (lambda ()
    (respond 200 "<h1>Home</h1>Hello from C/Scheme!")))

(add-route "GET" "/about"
  (lambda ()
    (respond 200 "<h1>About</h1>Hybrid C/Scheme server")))