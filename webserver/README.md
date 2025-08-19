# Rust Webserver
A simple HTTP webserver built in Rust, serving a welcome page (index.html) and a 404 error page (404.html). It uses a thread pool to handle concurrent requests efficiently.

## Features
Handles GET / and GET /sleep requests, serving index.html.
Returns a 404 response for invalid routes using 404.html.
Thread pool for concurrent request processing.
Basic error handling and logging.
Unit tests for thread pool and request handling.

# Usage
Run the project with: 
```bash
cargo run
```
Then access the server
http://127.0.0.1:7878/ for the welcome page.
http://127.0.0.1:7878/sleep for a delayed response.
Any other path returns the 404 page.

# Testing
Run unit tests for the thread pool and request handler:
```bash
cargo test
```
You can also run curl on the server for single GET calls
```bash
curl http://127.0.0.1:7878 # (should serve index.html).
curl http://127.0.0.1:7878/sleep # (should serve index.html after a 5-second delay).
curl http://127.0.0.1:7878/other # (should serve 404.html).
```

# Project Structure
- main.rs: Server logic, TCP handling, and request processing.
- lib.rs: Thread pool implementation for concurrent task execution.
- index.html: Welcome page with Tailwind CSS styling.
- 404.html: 404 error page with consistent styling.

# Notes
The server listens on 127.0.0.1:7878 with 4 worker threads by default.
Logs are output to the console with RUST_LOG=info.
