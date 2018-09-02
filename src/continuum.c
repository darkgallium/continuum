#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "communicator.h"
#include "utils.h"

int main() {

    printf("Getting credentials from config file ");
    printf(CONFIG_PATH);
    printf("\n");

    Credentials c;
    getCredentials(&c);
    
    while(1) {
        
	if (isCaptive()) {
	    	    
	    printf("It seems we're hurting the captive portal, authenticating...\n");

            getSecret(&c);
	    authenticate(&c);

	    free(c.secret);
        }


	sleep(30);

    }
}
