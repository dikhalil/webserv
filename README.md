<div align="center">

  <h1>🌐 webserv</h1>
  <p><strong>A custom HTTP/1.1 web server built from scratch in C++98</strong></p>

  <p>
    <img alt="C++" src="https://img.shields.io/badge/C++-98-blue.svg?style=flat-square&logo=c%2B%2B" />
    <img alt="Platform" src="https://img.shields.io/badge/Platform-Linux-lightgrey.svg?style=flat-square&logo=linux" />
    <img alt="Status" src="https://img.shields.io/badge/Status-Completed-success.svg?style=flat-square" />
  </p>
</div>

<br />

## 📖 Table of Contents
- [About The Project](#-about-the-project)
- [Key Features](#-key-features)
- [Architecture](#-architecture)
- [Getting Started](#-getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#-usage)
  - [Running the Server](#running-the-server)
  - [Quick Examples](#quick-examples)
- [Configuration](#-configuration)
  - [Overview](#overview)
  - [Example File](#example-file)
- [Resources](#-resources)
- [Team](#-team)

---

## 🚀 About The Project

`webserv` is an educational project created as part of the **42 school curriculum**. The goal is to deeply understand the internal mechanics of a real-world web server like NGINX or Apache by building one entirely from scratch.

This HTTP/1.1 compliant server handles everything from low-level socket management and non-blocking I/O multiplexing to high-level request parsing, dynamic CGI execution, and response generation, all driven by a highly customizable, NGINX-inspired configuration file.

---

## ✨ Key Features

- **HTTP/1.1 Compliant**: Supports core methods (`GET`, `POST`, `DELETE`).
- **Non-blocking I/O**: Robust concurrent connection handling using `poll()`.
- **Nginx-style Configuration**: Highly flexible `.conf` file parsing for granular control over server behavior.
- **Virtual Hosting**: Run multiple virtual servers on different ports or hostnames (`server_name`) simultaneously.
- **Dynamic Content (CGI)**: Execute scripts (e.g., Python) via the Common Gateway Interface (RFC 3875).
- **File Management**: Built-in support for static file serving, secure file uploads, and resource deletion.
- **Directory Listing**: Automatically generate HTML directory listings (`autoindex on/off`).
- **Routing & Redirection**: Seamless URL redirections (`return 300/301/302`).
- **Custom Error Pages**: Define custom HTML templates for specific HTTP status codes (e.g., `404`, `50x`).
- **Security & Limits**: Prevent buffer overflows and control resource usage with `client_max_body_size`.

---

## 🏗 Architecture

The server is built around a single-threaded event loop. It uses `poll()` to multiplex I/O operations, ensuring that the server never blocks while waiting for slow clients.

1. **Parser**: Reads the `.conf` file, tokenizes it, and constructs a hierarchical configuration object (`http` -> `server` -> `location`).
2. **Server/Reactor**: Initializes listening sockets and registers them with `poll()`.
3. **Connection Handling**: Accepts incoming connections and registers new client sockets.
4. **Request Lifecycle**:
   - Parses the incoming HTTP request stream asynchronously.
   - Matches the request URL to the appropriate `server` and `location` block.
   - Validates methods, body size, and permissions.
   - Dispatches to the correct handler (Static File, CGI, Autoindex, Upload, Redirect).
   - Generates and buffers the HTTP response, sending it back to the client as socket write readiness allows.

---

## 🛠 Getting Started

### Prerequisites

To compile and run this project, you will need a Linux-based environment with:
- `c++` compiler (clang++ or g++) with **C++98** support.
- `make` utility.

### Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/webserv.git
   cd webserv
   ```

2. **Build the executable:**
   ```bash
   make
   ```
   *Useful Makefile targets:*
   - `make` - Compiles the project.
   - `make clean` - Removes `.o` object files.
   - `make fclean` - Removes object files and the `webserv` binary.
   - `make re` - Recompiles the entire project from scratch.

---

## 💻 Usage

### Running the Server

Start the server by providing a configuration file.

```bash
# Run with the default test configuration
./webserv configs/webserv.conf
```

### Quick Examples

Once the server is running (assuming the provided `configs/webserv.conf` is loaded), you can interact with it:

- **Access the main page:**
  Open `http://127.0.0.1:8080/` in your browser.

- **Test Autoindex:**
  Visit `http://127.0.0.1:8080/browse`

- **Test Redirection:**
  Visit `http://127.0.0.1:8081/old-page` (Will redirect to the new page).

- **Upload a file:**
  ```bash
  curl -X POST --data-binary @my_file.txt http://127.0.0.1:8081/upload/my_file.txt
  ```

- **Delete a file:**
  ```bash
  curl -X DELETE http://127.0.0.1:8081/upload/my_file.txt
  ```

---

## ⚙️ Configuration

### Overview

The configuration language is heavily inspired by NGINX. It uses a nested block structure with three main contexts:
1. `http`: Global settings.
2. `server`: Virtual host definitions.
3. `location`: Route-specific rules.

**Directives supported:**
- `listen`: Port and/or host to bind to.
- `server_name`: Domain name matching.
- `root`: Base directory for requests.
- `index`: Default file to serve.
- `client_max_body_size`: Max upload size (supports `K`, `M`, `G` suffixes).
- `autoindex`: Directory listing (`on` / `off`).
- `error_page`: Custom error HTML paths.
- `methods`: Allowed HTTP methods (e.g., `GET POST`).
- `return`: HTTP redirection status and URL.
- `upload` / `upload_path`: Enable/disable uploads and set the destination directory.
- `cgi` / `cgi_ext` / `cgi_bin_path`: CGI execution parameters.

### Example File

```nginx
http {
    # Global settings
    client_max_body_size 2M;
    error_page 404 /html/error_pages/404.html;

    server {
        listen 127.0.0.1:8081;
        server_name localhost;
        root ./www;
        index /html/index.html;

        # Upload configuration
        location / {
            methods GET POST DELETE;
            upload on;
            upload_path /upload;
        }

        # CGI execution
        location /cgi_bin {
            cgi on;
            cgi_ext .py;
            methods GET POST;
            cgi_bin_path ./www/cgi_bin;
        }

        # Redirection
        location /old-page {
            return 301 /html/new-page.html;
        }
    }
}
```

---

## 📚 Resources

These resources were invaluable during the development of this project:
- **HTTP RFCs**: [RFC 7230 (Syntax & Routing)](https://datatracker.ietf.org/doc/html/rfc7230) | [RFC 7231 (Semantics)](https://datatracker.ietf.org/doc/html/rfc7231)
- **CGI Specification**: [RFC 3875 (CGI 1.1)](https://datatracker.ietf.org/doc/html/rfc3875)
- **Network Programming**: [Beej’s Guide to Network Programming](https://beej.us/guide/bgnet/)
- **Linux Manual Pages**: `socket(2)`, `bind(2)`, `listen(2)`, `accept(2)`, `poll(2)`, `execve(2)`

> **AI Usage Disclosure:** AI tools were used for drafting documentation content (including this README), generating test configuration scenarios, and reviewing edge cases during parser validation. The core implementation, integration, and manual validation were executed independently by the team.

---

## 👥 Team

Built with ❤️ by the 42 students:
- [dikhalil](https://profile.intra.42.fr/users/dikhalil)
- [kabu-zee](https://profile.intra.42.fr/users/kabu-zee)
- [rsrour](https://profile.intra.42.fr/users/rsrour)
