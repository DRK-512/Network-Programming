import os
import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin, urlparse
import time

base_dir = "./"  # Replace this with the absolute path where you want to save the HTML files

def download_page(url):
    time.sleep(1)  # Add delay to avoid error 429
    response = requests.get(url)
    if response.status_code == 200:
        return response.text
    else:
        print(f"Failed to download {url}. Status code: {response.status_code}")
        return None

def save_page(url, content):
    filename = get_filename(url)
    os.makedirs(os.path.dirname(filename), exist_ok=True)
    with open(filename, "w", encoding="utf-8") as f:
        f.write(content)
    print(f"Saved: {url} -> {filename}")

def get_filename(url):
    parsed_url = urlparse(url)
    if parsed_url.path == '':
        filename = parsed_url.netloc + "/index.html"
    elif parsed_url.path.endswith("/"):
        filename = parsed_url.netloc + parsed_url.path + "index.html"
    else:
        filename = parsed_url.netloc + parsed_url.path
    return os.path.join(base_dir, filename)

def scrape_website(url, depth=3):
    if depth == 0:
        return

    print(f"Scraping: {url}")
    html_content = download_page(url)
    if html_content:
        save_page(url, html_content)
        soup = BeautifulSoup(html_content, "html.parser")
        for link in soup.find_all("a", href=True):
            next_url = urljoin(url, link["href"])
            if urlparse(next_url).netloc == urlparse(url).netloc:
                scrape_website(next_url, depth - 1)

# Example usage
starting_url = "https://docs.yoctoproject.org/"
scrape_website(starting_url)

