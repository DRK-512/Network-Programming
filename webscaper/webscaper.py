import os
import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin, urlparse
import time
import logging
from urllib.robotparser import RobotFileParser
from urllib.error import URLError

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('scraper.log'),
        logging.StreamHandler()
    ]
)

class WebScraper:
    def __init__(self, base_dir="./scraped_pages", max_depth=3, delay=1.0):
        self.base_dir = os.path.abspath(base_dir)
        self.max_depth = max_depth
        self.delay = delay
        self.visited_urls = set()
        self.session = requests.Session()
        self.session.headers.update({
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36'
        })
        
    def can_fetch(self, url):
        """Check if URL is allowed by robots.txt"""
        try:
            rp = RobotFileParser()
            robots_url = f"{urlparse(url).scheme}://{urlparse(url).netloc}/robots.txt"
            rp.set_url(robots_url)
            rp.read()
            return rp.can_fetch(self.session.headers['User-Agent'], url)
        except Exception as e:
            logging.warning(f"Error checking robots.txt for {url}: {e}")
            return True  # Allow if robots.txt check fails

    def download_page(self, url):
        """Download page content with error handling"""
        try:
            if not self.can_fetch(url):
                logging.warning(f"robots.txt disallows scraping: {url}")
                return None
                
            time.sleep(self.delay)
            response = self.session.get(url, timeout=10)
            response.raise_for_status()
            return response.text
        except requests.RequestException as e:
            logging.error(f"Failed to download {url}: {e}")
            return None

    def save_page(self, url, content):
        """Save page content to file"""
        try:
            filename = self.get_filename(url)
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            with open(filename, "w", encoding="utf-8") as f:
                f.write(content)
            logging.info(f"Saved: {url} -> {filename}")
        except Exception as e:
            logging.error(f"Failed to save {url}: {e}")

    def get_filename(self, url):
        """Generate safe filename from URL"""
        parsed_url = urlparse(url)
        path = parsed_url.path
        if not path or path == '/':
            path = '/index.html'
        elif path.endswith('/'):
            path += 'index.html'
        elif not os.path.splitext(path)[1]:
            path += '.html'
            
        # Clean filename to avoid invalid characters
        netloc = parsed_url.netloc.replace(':', '_')
        path = path.replace('?', '_').replace('&', '_').replace('=', '_')
        return os.path.join(self.base_dir, netloc + path)

    def scrape_website(self, url, depth=None):
        """Recursively scrape website"""
        if depth is None:
            depth = self.max_depth
            
        if depth < 0 or url in self.visited_urls:
            return
            
        self.visited_urls.add(url)
        logging.info(f"Scraping (depth {depth}): {url}")
        
        html_content = self.download_page(url)
        if not html_content:
            return
            
        self.save_page(url, html_content)
        
        try:
            soup = BeautifulSoup(html_content, "html.parser")
            for link in soup.find_all("a", href=True):
                next_url = urljoin(url, link["href"])
                parsed_next = urlparse(next_url)
                parsed_current = urlparse(url)
                
                # Only follow links on same domain and with http(s) scheme
                if (parsed_next.netloc == parsed_current.netloc and 
                    parsed_next.scheme in ['http', 'https']):
                    self.scrape_website(next_url, depth - 1)
        except Exception as e:
            logging.error(f"Error parsing {url}: {e}")

def main():
    starting_url = "https://docs.yoctoproject.org/"
    scraper = WebScraper(base_dir="./scraped_pages", max_depth=3, delay=1.0)
    try:
        scraper.scrape_website(starting_url)
    except KeyboardInterrupt:
        logging.info("Scraping interrupted by user")
    finally:
        scraper.session.close()

if __name__ == "__main__":
    main()
