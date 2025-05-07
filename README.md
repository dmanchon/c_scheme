# C/Scheme Hybrid HTTP Server

This project is a simple HTTP server written in C that uses the [s7 Scheme interpreter](https://s7.scheme.org/) to handle HTTP requests via Scheme scripts. The server is multithreaded and allows you to define routes and handlers in Scheme, making it easy to extend and modify the server logic without recompiling the C code.

## Features

- Written in C, using POSIX sockets and pthreads for concurrency.
- Embeds the s7 Scheme interpreter for dynamic request handling.
- HTTP routing and response logic is defined in `main.sch` (Scheme).
- Pass the Scheme file to use at runtime with the `-i` argument.

## Project Structure

```
.
├── main.sch        # Scheme script with HTTP routes and handlers
├── src/
│   ├── main.c      # C source code for the HTTP server
│   ├── s7.c        # s7 Scheme interpreter source
│   └── s7.h        # s7 Scheme interpreter header
├── xmake.lua       # Build configuration for xmake
└── .gitignore
```

## Building

This project uses [xmake](https://xmake.io/) as the build system.

1. **Install xmake** (if you don't have it):
    ```bash
    brew install xmake
    ```
    or see [xmake installation docs](https://xmake.io/#/guide/installation).

2. **Build the project:**
    ```bash
    xmake
    ```

## Running

By default, the server listens on port 8080. You can specify a different Scheme file using the `-i` argument:

```bash
./build/macosx/arm64/debug/c_scheme -i main.sch