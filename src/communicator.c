#include <curl/curl.h>
#include <myhtml/api.h>
#include "communicator.h"
#include "utils.h"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

static void CURLCleaner(CURLResponse* c) {
  free(c->text);
}

static CURLResponse pageGET(char* url) {

  CURL *curl_handle;
  CURLcode res;
                                                                                 
  MemoryStruct chunk;
                                                                                 
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  chunk.size = 0;    /* no data at this point */ 
                                                                                 
  curl_global_init(CURL_GLOBAL_ALL);
                                                                                 
  curl_handle = curl_easy_init();
                                                                                 
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, COOKIE_FILE);
  curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR, COOKIE_FILE);                 
  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L); 
  
  /* we pass our 'chunk' struct to the callback function */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);
                                                                                 
  res = curl_easy_perform(curl_handle);
  CURLResponse response = {0};                                                                             

  if (res != CURLE_OK) {

    fprintf(stderr, "curl_easy_perform() failed: %s\n",
    curl_easy_strerror(res));

  }

  else {

    printf("%lu bytes retrieved\n", (unsigned long)chunk.size);

    response.text = chunk.memory;
    response.code = 0;
    res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response.code);

  }

  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();
  return response;

}

static CURLResponse pagePOST(char* url, Credentials* cred) {

  CURL *curl_handle;
  CURLcode res;
                                                                                 
  MemoryStruct chunk;
                                                                                 
  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  chunk.size = 0;    /* no data at this point */ 
                                                                                 
  curl_global_init(CURL_GLOBAL_ALL);
                                                                                 
  curl_handle = curl_easy_init();
                                                                                 
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl_handle, CURLOPT_COOKIEFILE, COOKIE_FILE);
  curl_easy_setopt(curl_handle, CURLOPT_COOKIEJAR, COOKIE_FILE);                  
  curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L); /* TODO: find a way to use system CA bundle! */


  char* a = concat("_token=", cred->secret);
  char* b = concat("&email=", cred->username);
  char* c = concat("&password=", cred->password);
  char* d = concat(a,b);
  char* e = concat(d,c);

  free(a);
  free(b);
  free(c);
  free(d);

  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, e);


  /* we pass our 'chunk' struct to the callback function */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
                                                                                 
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);
                                                                                 
  res = curl_easy_perform(curl_handle);
  CURLResponse response = {0};

  if (res != CURLE_OK) {
                                                        
    fprintf(stderr, "curl_easy_perform() failed: %s\n",
    curl_easy_strerror(res));
                                                        
  }

  else {                                                                           
    printf("%lu bytes retrieved\n", (unsigned long) chunk.size);
    
    response.text = chunk.memory;
    response.code = 0;
    res = curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response.code);
                                                                                     
  }

  free(e);
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();
  return response;
  
}

void getSecret(Credentials* cred) {
   printf("Getting CSRF token...");

   CURLResponse res = pageGET(CAPTIVE_URL);

   if ( (res.code / 100) == 2) {

      myhtml_t* myhtml = myhtml_create();
      myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);
      
      myhtml_tree_t* tree = myhtml_tree_create();
      myhtml_tree_init(tree, myhtml);
      
      myhtml_parse(tree, MyENCODING_UTF_8, res.text, strlen(res.text));
      myhtml_collection_t *collection = myhtml_get_nodes_by_tag_id(tree, NULL, MyHTML_TAG_INPUT, NULL);
      
      if (collection && collection->list && collection->length) {

         myhtml_tree_node_t *node = collection->list[0];
	 myhtml_tree_attr_t *attr = myhtml_attribute_by_key(node, "value", 5);
	 const char *token = myhtml_attribute_value(attr, NULL);
	    
	 cred->secret = malloc(strlen(token)+1);
         strcpy(cred->secret, token);
      
      }

      myhtml_collection_destroy(collection);
      myhtml_tree_destroy(tree);
      myhtml_destroy(myhtml);

   }
   
   CURLCleaner(&res);
}

void authenticate(Credentials* cred) {
   CURLResponse res = pagePOST(CAPTIVE_URL, cred);
  
   if ( (res.code / 100) == 2 ) {
	// We should check the presence of error messages here
   }
   
   CURLCleaner(&res);
}

int isCaptive() {

   CURLResponse res = pageGET(TEST_URL);
   
   if ( (res.code / 100) == 3 ) {
      // If the code begins with 3, it means we are likely being redirected by the captive portal
      CURLCleaner(&res);
      return 1;
   }

   else {
      CURLCleaner(&res);
      return 0;
   }

   
}


