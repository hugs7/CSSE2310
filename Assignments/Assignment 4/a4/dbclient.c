// By Hugo Burton
// Start Date: 4pm Friday 13th May, 2022
// Due Date: 4pm Friday 3rd June, 2022
// Student Number: s4698512 (46985123)


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <csse2310a4.h>

// -- Definitions

#define MAX_BUFFER_SIZE 80
#define DBCLIENT_USAGE "Usage: dbclient portnum key [value]\n"
#define DBCLIENT_SPACE_NL "dbclient: key must not contain spaces or newlines\n"
#define DBCLIENT_PORT_CON(P) "dbclient: unable to connect to port (P)\n"
#define HTTP_OK "HTTP/1.1 200 OK\n"
#define HTTP_OK_CODE 200

// ---- Function Declarations -----

void check_key(const char* key);

void req_and_res(int argc, char* argv[], int write, const char* key);

void update_value(FILE** to, const char* key, const char* value);

void get_value(FILE** to, const char* key);

// Program

int main(int argc, char* argv[]) {
    // min arguments 3
    // Additional arguments ignored

    if (argc < 3) {
        fprintf(stderr, DBCLIENT_USAGE);
        exit(1);        // #56
    }
    
    const char* portnum = argv[1];   // get port number
    const char* key = argv[2];
    struct addrinfo* ai = 0;
    struct addrinfo hints;
    
    check_key(key);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    int error;
    if ((error = getaddrinfo(NULL, portnum, &hints, &ai))) {
        freeaddrinfo(ai);
        return 1;
    }
    
    int write = socket(AF_INET, SOCK_STREAM, 0); // 0 == use default protocol
    if (connect(write, (struct sockaddr*)ai->ai_addr, 
            sizeof(struct sockaddr))) {
        fprintf(stderr, DBCLIENT_PORT_CON (portnum)); // #63
        exit(2);    // exit status 2 - #63
    }

    freeaddrinfo(ai);

    // write connected
    // separate streams
    
    req_and_res(argc, argv, write, key);
    
    // client exists successfully
        
    return 0;
}

/* check_key()
 * -----------
 * checks to ensure key doesn't contain spaces or new lines
 *
 * key: const char* pointing to key
 */
void check_key(const char* key) {
    // check key value
    // No spaces or new lines
    // iterate over string
    for (int i = 0; i < strlen(key); i++) {
        if (key[i] == ' ' || key[i] == '\n') {
            // bad key provided
            fprintf(stderr, DBCLIENT_SPACE_NL);
            exit(1);    // #60
        }
    }
}

/* req_and_res()
 * -------------
 * Handles request and response from and back to the client
 *
 * argc: number of arguments passed to dbclient
 * argv: array of arguments passed to dbclient
 * write: socket used for writing to server
 * key: key provided to client in arguments
 */
void req_and_res(int argc, char* argv[], int write, const char* key) {
    int read = dup(write);
    FILE* to = fdopen(write, "w");
    FILE* from = fdopen(read, "r");
    
    if (argc >= 4) {
        // Value present (and potentially other arguments)
        const char* value = argv[3];
        update_value(&to, key, value);  
    } else {  
        // just getting value using key provided
        get_value(&to, key);        
    }

    int status;
    char* statusExplanation;
    char* body;
    HttpHeader** headers;

    if (get_HTTP_response(from, &status, &statusExplanation, &headers, 
            &body)) {
        // success receiving response 
    } else {
        if (argc >= 4) {
            exit(4);
        } else {
            exit(3);
        }
    }

    if (argc >= 4) {
        if (status == HTTP_OK_CODE) {
            exit(0);
        } else {
            exit(4);
        }
    } else {
        if (status == HTTP_OK_CODE) {
            printf("%s\n", body);
            exit(0);
        } else {
            exit(3);
        }
    }
}

/* update_value()
 * --------------
 * Updates value by making a PUT request to server
 *
 * to: pointer to FILE* so function can write to server
 * key: pointer to characters containing key
 * value: pointer to characters containing value
 */
void update_value(FILE** to, const char* key, const char* value) {
    //fprintf(to, "key, value: %s, %s\n", key, value);
    // calculate content length. Check this but should be able to use
    // strlen here
    
    int conLength = strlen(value);
    
    fprintf(*to, "PUT /public/%s HTTP/1.1\r\nContent-Length: %d\r\n\r\n%s",
            key, conLength, value);
    fflush(*to);
    fclose(*to);     // close 'to' after done sending data to server

}

/* get_value()
 * -----------
 * Gets value from server by making GET request
 *
 * to: pointer to FILE* so function can write to server
 * key: string containing key provided to dbclient as argument
 */
void get_value(FILE** to, const char* key) {
    fflush(stdout);
    fprintf(*to, "GET /public/%s HTTP/1.1\r\n\r\n", key);
    // Get /public/<key> HTTP/1.1CRLF blankline terminated CRLF
    // check the CRLFs are right here
    
    fflush(*to);
    fclose(*to);     // close 'to' after done sending data to server
}
