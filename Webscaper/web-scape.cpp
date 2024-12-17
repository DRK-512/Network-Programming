#include <iostream>
#include <fstream> // std::ofstream 
#include <string> // std::string
#include <vector> // std::vector
#include <curl/curl.h> // CURL & curl_easy_init
#include <filesystem> // std::filesystem
#include <regex> // std::regex
#include <set> // std::set

size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* buffer) {
    size_t tSize = size*nmemb; 
    buffer->append((char*) contents, tSize);
    return tSize;
}

std::string fetchPage(const std::string& url) {
    CURL* cl = curl_easy_init();
    std::string response;
    if(cl) {
        curl_easy_setopt(cl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(cl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(cl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(cl, CURLOPT_FOLLOWLOCATION, 1L);
        CURLcode res = curl_easy_perform(cl); 
        if(res != CURLE_OK) {
            std::cerr << "ERROR: Curl: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(cl);
    }
    return response; 
}

std::string fetchDomain(const std::string& url) {
    std::regex domainRegex(R"(https?://([^/]+))");
    std::smatch match; 

    if(std::regex_search(url, match, domainRegex)) {
        return match[1].str(); 
    } 
    return ""; 
}

std::string normalizeLink(const std::string& link, const std::string& baseUrl) {
    if (link.find("http://") != 0 && link.find("https://") != 0) {
        return link; 
    }
    if (!baseUrl.empty()) {
        if (link[0] == '/') {
            return baseUrl + link; 
        } else {
            return baseUrl + "/" + link; 
        }
    }
    return link; 
}

// get all subpages AKA links
std::vector<std::string> extractLinks(const std::string& html, const std::string& baseUrl) {
    
    std::regex linkRegex(R"((href|src)\s*=\s*['"]([^'"]+)['"])"); // check if this is right, it is weird
    std::smatch match; 
    std::vector<std::string> links;
    std::set<std::string> seenLinks; 

    auto baseDomain = fetchDomain(baseUrl); 
    auto searchStart = html.cbegin(); 

    while(std::regex_search(searchStart, html.cend(), match, linkRegex)) {
        std::string link = match[2].str();

        // handle links
        std::string fullLink = normalizeLink(link, baseUrl); 

        auto linkDomain = fetchDomain(fullLink); 
        if ((linkDomain == baseDomain) && (seenLinks.find(fullLink) == seenLinks.end())) {
            links.push_back(link); 
            seenLinks.insert(fullLink); 
        }
        searchStart = match.suffix().first;
    }
    return links; 
}

// save the links
void saveToFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if(file.is_open()) {
        file << content;
        file.close(); 
    } else {
        std::cerr << "ERROR: Could not save file " << path << std::endl; 
    }
}

// recursive page download
void downloadSite(const std::string& url, const std::string& outputDir) {
    
    static std::set<std::string> visited; 

    // if I already visited a site, I dont want to download it again, just skip it
    if(visited.find(url) != visited.end()) return; 
    visited.insert(url); 

    // fetch URL
    std::cout << "Fetching: " << url << std::endl; 
    std::string pageContent = fetchPage(url); 

    // save page locally
    std::string sanitizedURL = std::regex_replace(url, std::regex(R"([^\w.-])"), "_");
    std::filesystem::create_directories(outputDir); 
    std::string filePath = outputDir + "/" + sanitizedURL + ".html"; 
    saveToFile(filePath, pageContent); 

    // recursivly extract and download all the links
    std::vector<std::string> links = extractLinks(pageContent, url); 
    for(const std::string& link : links){
        downloadSite(link, outputDir); 
    }
}

int main(int argc, char* argv[]) {
    if ((argc < 3) || ((std::string(argv[1]) != "-u") && (std::string(argv[1]) != "--url"))) {
        std::cerr << "Usage: " << argv[0] << " -u|--url <url>" << std::endl;
        return 1;
    }
    std::string url = argv[2];
    std::string outputDir = "local_clone"; 

    downloadSite(url, outputDir);
    std::cout << "Website Cloned Successfully to directory: " << outputDir << std::endl;
    return 0;
}
