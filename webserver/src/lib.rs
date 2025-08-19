use std::{sync::{mpsc, Arc, Mutex}, thread};

pub struct ThreadPool {
    workers: Vec<Worker>,
    sender: mpsc::Sender<Message>,
}

// What we will send down our channel
// Job is a type alias for a trait object that the type closer expects
type Job = Box<dyn FnOnce() + Send + 'static>;

enum Message {
    NewJob(Job),
    Terminate,
}

impl ThreadPool {
    /// Create new ThreadPool
    /// The size is the number of threads in the pool
    /// 
    /// # Panics
    /// The 'new' function will panic if the size is less than 0
    pub fn new(size: usize) -> ThreadPool {
        assert!(size > 0); // we need to have more than size of 0

        let (sender, receiver) = mpsc::channel();

        // set up a thread-safe, shared channel receiver that can be accessed by multiple threads in a concurrent program
        let receiver = Arc::new(Mutex::new(receiver));
        
        let mut workers = Vec::with_capacity(size);

        for id in 0..size {
            workers.push(Worker::new(id, Arc::clone(&receiver)));
        }
        ThreadPool { workers, sender }
    }

    // our version of Thread::spawn
    pub fn execute<F>(&self, f: F)
    where F: FnOnce() + Send + 'static
    {
        // when one channel is called, we can use the closer to send data to the workers
        let job = Box::new(f);
        if let Err(e) = self.sender.send(Message::NewJob(job)) {
            eprintln!("Failed to send job: {0}", e);
        }
    }
}

// This will ensure reqs dont automatically drop when the server goes down
impl Drop for ThreadPool {
    fn drop(&mut self) {
        println!("Sending terminate message to all workers.");

        for _ in &self.workers {
            if let Err(e) = self.sender.send(Message::Terminate) {
                eprintln!("Failed to send terminate message: {}", e);
            }
        }

        for worker in &mut self.workers {
            println!("Shutting down worker: {0}", worker.id);
            // worker.thread.join().unwrap();
            if let Some(thread) = worker.thread.take() {
                if let Err(e) = thread.join() {
                    eprintln!("Failed to join worker {}: {:?}", worker.id, e);
                }
            }
        }
    }
}
struct Worker {
    id: usize, 
    thread: Option<thread::JoinHandle<()>>
}
impl Worker {
    fn new(id: usize, receiver: Arc<Mutex<mpsc::Receiver<Message>>>) -> Worker {
        // we have to keep looping to look for threads to execute
        let thread = thread::spawn(move || loop {
            // lock to get mutex (might fail) & recv to recieve job from channel (also might fail)
            let message = match receiver.lock() {
                Ok(guard) => match guard.recv() {
                    Ok(msg) => msg,
                    Err(e) => {
                        println!("Worker {}: Channel disconnected: {}", id, e);
                        break;
                    }
                },
                Err(e) => {
                    println!("Worker {}: Failed to lock receiver: {}", id, e);
                    break;
                }
            };
            match message {
                Message::NewJob(job) => {
                    println!("Worker {} got a job; executing.", id);
                    job();
                }
                Message::Terminate => {
                    println!("Worker {} was told to terminate.", id);
                    break;
                }
            }
        });
        Worker {id, thread:Some(thread) }
    }
}

/// This test checks for the following
/// thread pool creation with valid and invalid sizes.
/// job execution to ensure tasks are processed by workers.
/// graceful shutdown via the Drop implementation.
#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::atomic::{AtomicUsize, Ordering};
    use std::time::Duration;

    #[test]
    #[should_panic(expected = "assertion failed: size > 0")]
    fn test_thread_pool_new_zero_size() {
        ThreadPool::new(0);
    }

    #[test]
    fn test_thread_pool_new_valid_size() {
        let pool = ThreadPool::new(2);
        assert_eq!(pool.workers.len(), 2);
    }

    #[test]
    fn test_thread_pool_execute() {
        let pool = ThreadPool::new(2);
        let counter = Arc::new(AtomicUsize::new(0));
        let counter_clone = Arc::clone(&counter);

        pool.execute(move || {
            counter_clone.fetch_add(1, Ordering::SeqCst);
        });

        // Give workers time to process the job
        thread::sleep(Duration::from_millis(100));
        assert_eq!(counter.load(Ordering::SeqCst), 1);
    }

    #[test]
    fn test_thread_pool_drop() {
        let pool = ThreadPool::new(2);
        let counter = Arc::new(AtomicUsize::new(0));
        let counter_clone = Arc::clone(&counter);

        pool.execute(move || {
            counter_clone.fetch_add(1, Ordering::SeqCst);
        });

        // Drop the pool to trigger shutdown
        drop(pool);
        thread::sleep(Duration::from_millis(100));
        assert_eq!(counter.load(Ordering::SeqCst), 1);
    }
}
