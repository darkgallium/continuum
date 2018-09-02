#ifndef DEF_COM
#define DEF_COM

#define TEST_URL "http://detectportal.firefox.com/"
#define CAPTIVE_URL "<complete with your isp's captive portal url>"
#define USER_AGENT "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36"
#define COOKIE_FILE "/tmp/continuum.cookies"
#define CA_PATH "/etc/ssl/certs/ca-certificates.crt"

typedef struct Credentials Credentials;
struct Credentials {
    char* username;
    char* password;
    char* secret;
};

typedef struct MemoryStruct MemoryStruct;
struct MemoryStruct {
    char *memory;
    size_t size;
};

typedef struct CURLResponse CURLResponse;
struct CURLResponse {
    char* text;
    long code;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
static void CURLCleaner(CURLResponse* c);
static CURLResponse pageGET(char* url);
static CURLResponse pagePOST(char* url, Credentials* cred);

void authenticate(Credentials* cred);
void getSecret(Credentials* cred);
int isCaptive();

#endif
