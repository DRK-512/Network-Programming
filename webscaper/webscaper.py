import os  # For handling file system operations
import requests  # For making HTTP requests to websites
from bs4 import BeautifulSoup  # For parsing HTML content
from urllib.parse import urljoin, urlparse  # For URL manipulation
import time  # For adding delays between requests
import logging  # For logging scraper activity
from urllib.robotparser import RobotFileParser  # For checking robots.txt rules
from urllib.error import URLError  # For handling URL-related errors

# Configure logging to output to both file and console
# This helps track scraper activity and debug issues
logging.basicConfig(
    level=logging.INFO,  # Set logging level to INFO to capture relevant events
    format='%(asctime)s - %(levelname)s - %(message)s',  # Log format with timestamp
    handlers=[
        logging.FileHandler('scraper.log'),  # Save logs to scraper.log file
        logging.StreamHandler()  # Also print logs to console
    ]
)

class WebScraper:
    # WebScraper class to encapsulate all scraping functionality
    def __init__(self, base_dir="./scraped_pages", max_depth=3, delay=1.0):
        # Initialize the scraper with configurable parameters
        self.base_dir = os.path.abspath(base_dir)  # Convert base_dir to absolute path for consistency
        self.max_depth = max_depth  # Maximum recursion depth for scraping
        self.delay = delay  # Delay between requests to avoid overwhelming servers
        self.visited_urls = set()  # Set to track visited URLs to avoid duplicates
        self.session = requests.Session()  # Create a session for persistent HTTP connections
        # Set a user-agent to identify the scraper to servers
        self.session.headers.update({
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'
        })

    def can_fetch(self, url):
        # Check if scraping the URL is allowed by the website's robots.txt
        try:
            rp = RobotFileParser()  # Create a parser for robots.txt
            # Construct the robots.txt URL for the given URL's domain
            robots_url = f"{urlparse(url).scheme}://{urlparse(url).netloc}/robots.txt"
            rp.set_url(robots_url)  # Set the URL for robots.txt
            rp.read()  # Fetch and parse the robots.txt file
            # Check if our user-agent is allowed to fetch the URL
            return rp.can_fetch(self.session.headers['User-Agent'], url)
        except Exception as e:
            # Log any errors while checking robots.txt
            logging.warning(f"Error checking robots.txt for {url}: {e}")
            return True  # Default to allowing if robots.txt check fails

    def download_page(self, url):
        # Download the HTML content of a webpage
        try:
            # First, check if robots.txt allows scraping this URL
            if not self.can_fetch(url):
                logging.warning(f"robots.txt disallows scraping: {url}")
                return None
                
            # Respect the delay to avoid overwhelming the server
            time.sleep(self.delay)
            # Make an HTTP GET request with a timeout of 10 seconds
            response = self.session.get(url, timeout=10)
            response.raise_for_status()  # Raise an exception for bad status codes
            return response.text  # Return the HTML content
        except requests.RequestException as e:
            # Log any errors during the HTTP request
            logging.error(f"Failed to download {url}: {e}")
            return None

    def save_page(self, url, content):
        # Save the downloaded HTML content to a file
        try:
            filename = self.get_filename(url)  # Generate a filename for the URL
            os.makedirs(os.path.dirname(filename), exist_ok=True)  # Create directories if needed
            # Write the content to the file with UTF-8 encoding
            with open(filename, "w", encoding="utf-8") as f:
                f.write(content)
            logging.info(f"Saved: {url} -> {filename}")  # Log successful save
        except Exception as e:
            # Log any errors during file saving
            logging.error(f"Failed to save {url}: {e}")

    def get_filename(self, url):
        # Generate a safe filename from the URL for saving the page
        parsed_url = urlparse(url)  # Parse the URL into components
        path = parsed_url.path  # Get the path component
        if not path or path == '/':
            path = '/index.html'  # Use index.html for root URLs
        elif path.endswith('/'):
            path += 'index.html'  # Append index.html for directory URLs
        elif not os.path.splitext(path)[1]:
            path += '.html'  # Add .html extension if none exists
            
        # Clean the network location (domain) to avoid invalid characters
        netloc = parsed_url.netloc.replace(':', '_')
        # Clean the path to avoid problematic characters in filenames
        path = path.replace('?', '_').replace('&', '_').replace('=', '_')
        # Combine base directory, domain, and path to create full filename
        return os.path.join(self.base_dir, netloc + path)

    def scrape_website(self, url, depth=None):
        # Recursively scrape a website up to the specified depth
        if depth is None:
            depth = self.max_depth  # Use default max_depth if not specified
            
        # Stop if depth limit reached or URL already visited
        if depth < 0 or url in self.visited_urls:
            return
            
        self.visited_urls.add(url)  # Mark URL as visited
        logging.info(f"Scraping (depth {depth}): {url}")  # Log current URL and depth
        
        html_content = self.download_page(url)  # Download the page content
        if not html_content:
            return  # Skip if download failed
            
        self.save_page(url, html_content)  # Save the downloaded content
        
        try:
            # Parse the HTML content using BeautifulSoup
            soup = BeautifulSoup(html_content, "html.parser")
            # Find all <a> tags with href attributes
            for link in soup.find_all("a", href=True):
                # Resolve relative URLs to absolute URLs
                next_url = urljoin(url, link["href"])
                parsed_next = urlparse(next_url)  # Parse the next URL
                parsed_current = urlparse(url)  # Parse the current URL
                
                # Only follow links on the same domain and with http(s) scheme
                if (parsed_next.netloc == parsed_current.netloc and 
                    parsed_next.scheme in ['http', 'https']):
                    # Recursively scrape the next URL with decremented depth
                    self.scrape_website(next_url, depth - 1)
        except Exception as e:
            # Log any errors during HTML parsing
            logging.error(f"Error parsing {url}: {e}")

def main():
    # Main function to start the scraping process
    starting_url = "https://docs.yoctoproject.org/"  # Starting URL for scraping
    # Initialize scraper with default parameters
    scraper = WebScraper(base_dir="./scraped_pages", max_depth=3, delay=1.0)
    try:
        scraper.scrape_website(starting_url)  # Start scraping
    except KeyboardInterrupt:
        # Handle user interruption (Ctrl+C)
        logging.info("Scraping interrupted by user")
    finally:
        # Ensure the HTTP session is properly closed
        scraper.session.close()

if __name__ == "__main__":
    main()  # Run the main function if script is executed directly
