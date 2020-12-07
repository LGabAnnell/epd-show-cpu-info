#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <signal.h>

static char *wholeData; 
CURL *handle;

size_t print(void *ptr, size_t size, size_t nmemb, void *_) {
  strncpy(wholeData, ptr, nmemb);
  wholeData[nmemb] = '\0';
  return nmemb;
}

void curl_init(char *url) {
  curl_global_init(CURL_GLOBAL_ALL);

  handle = curl_easy_init();
  curl_easy_setopt(handle, CURLOPT_URL, url);
  curl_easy_setopt(handle, CURLOPT_TIMEOUT_MS, 10000L);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, print);
}

char* get(char *dataHolder) {
  wholeData = dataHolder;
  CURLcode res;

  if (!handle) { printf("CURL NOT CORRECTLY INITIALIZED"); }

  res = curl_easy_perform(handle);

  if (res != CURLE_OK) {
    return "err";
  }

  return "OK";
}


