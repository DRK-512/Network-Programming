use std::net::TcpListener; 
use std::net::TcpStream;
use std::io::prelude::*; // To read the tcp stream
use std::fs;             // To access fs to fetch index.html
use std::thread;         // thread::sleep
use std::time::Duration; // Duration::from_secs(5)

use webserver::ThreadPool; 

fn main() {
    // 7878 spells out rust on a phone
    let ip_port: String = "127.0.0.1:7878".to_string();
    let pool = ThreadPool::new(4); // Use thread pool so we dont have infinite

    // Listen for connections
    let listener = match TcpListener::bind(&ip_port) {
        Ok(listener) => listener, 
        Err(e) => {
            eprintln!("Failed to bind to {0}: {1}", ip_port, e);
            std::process::exit(1);
        }
    };

    // wait for messages which will either be a tcp stream or an error
    for stream in listener.incoming().take(2) {
        // NOTE: we call unwrap a bit in this code because it panics if it errors
        match stream {
            Ok(stream) => {
                // when we execute the pool, we do have a thread max
                pool.execute(|| { handler(stream); });
            }
            Err(e) => {
                eprintln!("Error accepting connection: {}", e);
            }
        }
    }

    println!("Shutting Down");
}

// This will handle /read the data from the tcp stream 
fn handler(mut stream: TcpStream) {
    // NOTE: b gives us a byte array of the string
    let get: &[u8; 16] = b"GET / HTTP/1.1\r\n";
    let sleep: &[u8; 21] = b"GET /sleep HTTP/1.1\r\n"; // if a req takes too long, we go here

    // TODO: change 1024 to an arbitrary size
    let mut buffer: [u8; 1024] = [0; 1024];
    if let Err(e) = stream.read(&mut buffer) {
        eprintln!("Failed to read from stream: {}", e);
        return;
    }
    //println!(
    //    "Request: {}",
    //    String::from_utf8_lossy(&buffer[..])
    //);
    // Now check if buffer starts with the expected byte array AKA the GET call
    let (status_line, filename) = 
    if buffer.starts_with(get) {
        ("HTTP/1.1 200 OK", "static/index.html")
    } else if buffer.starts_with(sleep) {
        thread::sleep(Duration::from_secs(5));
        ("HTTP/1.1 200 OK", "static/index.html")
    } else {
        ("HTTP/1.1 404 NOT FOUND", "static/404.html")
    };

    let contents = match fs::read_to_string(filename) {
        Ok(contents) => contents,
        Err(e) => {
            eprintln!("Failed to read {}: {}", filename, e);
            let error_response = "HTTP/1.1 500 INTERNAL SERVER ERROR\r\n\r\nServer Error";
            stream.write(error_response.as_bytes()).unwrap_or_else(|e| {
                eprintln!("Failed to write error response: {}", e);
                0 // Return 0 to indicate no bytes were written
            });
            return;
        }
    };

    let response = format!(
        "{}\r\nContent-Length: {}\r\n\r\n{}",
        status_line, contents.len(), contents
    );

    if let Err(e) = stream.write(response.as_bytes()) {
        eprintln!("Failed to write response: {}", e);
    }
    if let Err(e) = stream.flush() {
        eprintln!("Failed to flush stream: {}", e);
    }
}
