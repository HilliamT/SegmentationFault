#include "query_verif.h"
#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>

/**
 * HOW TO USE:
 *
 * int main(int argc, char** argv) {
 *      printf("%s", get_stackoverflow_results(argc, argv));
 * }
 *
 * 1 simple function, get_stackoverflow_results will return the JSON
 * of the stackoverflow results from the user's terminal inputs.
 *
 */


/**
 * Used to hold our API response string
 */
struct Buffer {
    char *memory;
    size_t size;
};

/**
 * Append user query to our base url to send HTTP request to
 */

char* build_url(char* query) {
  CURL* curl = curl_easy_init();
  if (curl) {
    // Encode for suitability as a URL link
    char* safe_query = curl_easy_escape(curl, query, 0);

    // We combine our base url for the API, with our safe_query
    char* url = "https://api.stackexchange.com/2.2/search/advanced?order=desc&sort=relevance&accepted=True&answers=1&site=stackoverflow&filter=!-*jbN-o8P3E5&q=";
    char* final_url = malloc(sizeof(char) * (strlen(url) + strlen(safe_query) + 3));
    strcpy(final_url, url);
    strcat(final_url, safe_query);

    curl_easy_cleanup(curl);
    free(safe_query);
    return final_url;
  }
  return NULL;
}

/**
 * Callback function for CURL as to write to a buffer
 */
static size_t write_to_buffer(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct Buffer *mem = (struct Buffer *)userp;

    // Allocate memory for ptr to response onto the heap
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(ptr == NULL) {
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    // Save data and size of bytes
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

/**
 * Set up HTTP request and send API request
 */
char* send_api_request(char* url) {
    CURL *curl_handle;
    CURLcode res;

    struct Buffer buffer;

    buffer.memory = malloc(1);
    buffer.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    // Set the HTTP headers for the API to return a non-escaped HTML format response
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Accept: text/html; charset=utf-8");
    headers = curl_slist_append(headers, "Content-Type: text/html; charsets: utf-8");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl_handle, CURLOPT_ACCEPT_ENCODING, "gzip, deflate"); // SO encodes in GZIP, so need this to decode
    curl_easy_setopt(curl_handle, CURLOPT_URL, url); // Direct our HTTP request to the URL
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_to_buffer); // A function as to manage how we
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&buffer); // write into buffer
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    // perform the request
    res = curl_easy_perform(curl_handle);

    // Check if we have successfully made the request
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        exit(0);
    }
    // Success! Our data should now be stored in buffer.memory

    // clean up CURL objects since we no longer need them
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return buffer.memory;
}

/**
 * Returns the JSON string of relevant questions from StackOverflow of the user input given
 */
char* get_stackoverflow_results(char *query) {
    char* url = build_url(query); // affix our parameters to the base URL
    char* json = send_api_request(url); // GET request to the URL, and return json data
    free(url);
    return json;
}
