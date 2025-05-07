# C/Scheme Hybrid HTTP Server

This project is a simple HTTP server written in C that uses the [s7 Scheme interpreter](https://s7.scheme.org/) to handle HTTP requests via Scheme scripts. The server is multithreaded and allows you to define routes and handlers in Scheme, making it easy to extend and modify the server logic without recompiling the C code.

## Features

- Written in C, using POSIX sockets and pthreads for concurrency.
- Embeds the s7 Scheme interpreter for dynamic request handling.
- HTTP routing and response logic is defined in `main.sch` (Scheme).
- Pass the Scheme file to use at runtime with the `-i` argument.

## Project Structure