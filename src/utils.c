#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "communicator.h"
#include "utils.h"

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    if (result == NULL) {
	   printf("not enough memory (malloc returned NULL)\n"); 
	   return "";
    } 
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void getCredentials(Credentials* c) {
    
    FILE* file = NULL;
    char username[BUFF_SIZE];
    char password[BUFF_SIZE];

    file = fopen(CONFIG_PATH, "r");
                                                        
    if (file != NULL)
    {
        if (fgets(username, BUFF_SIZE, file) != NULL && fgets(password, BUFF_SIZE, file) != NULL) {
		//username[strcspn(username, "\r\n")] = 0;
		//password[strcspn(password, "\r\n")] = 0;

		c->username = username;
		c->password = password;	
	}
        
        fclose(file);
    }
    else {
	printf("Config file not found. Using blank credentials.");
    }    

}
