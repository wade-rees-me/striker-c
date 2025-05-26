#include "request.h"
#include "../xlog/xlog.h"
#include "constants.h"
#include <curl/curl.h>

// Helper function to handle the CURL write callback
static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t totalSize = size * nmemb;
    Request *request = (Request *)userp;
    strncat(request->responseString, (char *)contents, totalSize);
    return totalSize;
}

// Function to fetch JSON data from a URL and store it in the Request
void requestFetchJson(Request *request, const char *url) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl) {
        xlog_panic("curl_easy_perform() failed:");
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)request);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        xlog_panic("curl_easy_perform() failed: %s", curl_easy_strerror(res));
    }
    unescape_json(request->responseString);
    strip_quotes(request->responseString);
    request->jsonResponse = cJSON_Parse(request->responseString);
    if (request->jsonResponse == NULL) {
        xlog_panic("Error parsing JSON");
    }
    curl_easy_cleanup(curl);
}

