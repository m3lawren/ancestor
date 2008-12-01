#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

#include <assert.h>

#include "jobqueue.h"

size_t header_func(void* ptr, size_t size, size_t nmemb, void* stream) {
	(void)stream;

	assert(size == 1);
	
	fwrite(ptr, size, nmemb, stdout);

	return size * nmemb;
}

size_t write_func(void* ptr, size_t size, size_t nmemb, void* stream) {
	(void)ptr;
	(void)stream;
	assert(stream == NULL);
	assert(size == 1);
	printf("Received %d bytes\n", nmemb);
	return size * nmemb;
}

int main(void) {
	CURL* curl;
	CURLcode res;

	curl = curl_easy_init();

	if (!curl) {
		fprintf(stderr, "Failed to initialize curl.\n");
		exit(EXIT_FAILURE);
	}

	curl_easy_setopt(curl, CURLOPT_URL, "netinn.xsquared.ca");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_func);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_func);
	if ((res = curl_easy_perform(curl))) {
		fprintf(stderr, "CURL error: %s", curl_easy_strerror(res));
	}

	curl_easy_cleanup(curl);

	return EXIT_SUCCESS;
}
