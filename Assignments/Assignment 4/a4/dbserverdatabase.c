// CSSE2310 Assignment 4 a4
// By Hugo Burton
// Start Date: 4pm Friday 13th May, 2022
// Due Date: 4pm Friday 3rd June, 2022
// Student Number: s4698512 (46985123)

#ifndef dbclient_h
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <csse2310a3.h>     // Library from a3
#include <csse2310a4.h>     // Library for a4
#include <stringstore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

#define MAX_BUFFER_SIZE 52
#define DBSERVER_USAGE "Usage: dbserver authfile connections [portnum]\n"
#define DBSERVER_AUTH_READ "dbserver: unable to read authentication string\n"
#define DBSERVER_SOCK "dbserver: unable to open socket for listening\n"
#define HTTP_PUT "PUT"
#define HTTP_GET "GET"
#define HTTP_DELETE "DELETE"

#define PUBLIC "public"
#define PRIVATE "private"

#define HTTP_OK 200
#define HTTP_OK_TXT "OK\0"
#define HTTP_BAD_REQUEST 400
#define HTTP_BAD_REQUEST_TXT "Bad Request\0"
#define HTTP_NOT_FOUND 404
#define HTTP_NOT_FOUND_TXT "Not found\0"
#define HTTP_UNAUTHORIZED 401
#define HTTP_UNAUTHORIZED_TXT "Unauthorized\0"

#define CON_LEN "Content-Length\0"
#define AUTHORIZATION "Authorization\0"

#define ZERO_STR "0\0"

#endif      // dbclient_h

// Structs

struct Client {
    pthread_t threadId;
    FILE* write;
    FILE* read;
};

typedef struct database_struct {
    char* key;
    char* value;
    struct database_struct* next;
} database;

// --- Function Declarations ---

void check_arguments(int argc, char* argv[], FILE** authfile, 
        int* connections, char** portnum); 

int open_listen(const char* port, int connections);

void process_connection(int fdServer);

void* client_thread(void* arg);

void HTTP_request_loop(int id);

void Http_response(int id, database*** db, char** addrToks, 
        HttpHeader*** headers, int* authStatus, char** key, char** value, 
        HttpHeader*** resHeaders, int* numResHeaders, char** resBody, 
        int* status, char** statusExplanation, char** body, char** method); 

int select_database(database*** db, char* dbSelect, HttpHeader*** headers,
        int* authStatus);

void Http_methods(database*** db, int* status, char** statusExplanation,
        char** body, char** resBody, char** key, char** value, char** method,
        HttpHeader*** headers, HttpHeader*** resHeaders);

void add_header(HttpHeader*** header_arr, int* num_h, char* name, char* value); 

void edit_header(HttpHeader*** headerArr, char* name, char* newValue);

int check_header(HttpHeader** headerArr, char* nameMatch, char* valueMatch);

int header_exist(HttpHeader** headerArr, char* name);

database** db_allocate(void);

void db_dealloc(database** db);

void* get_record(database* db, char* key);

void add_record(database** db, char* key, char* value);

void delete_record(database** db, char* key);

// Globals

int clientid = -1;
struct Client* clients;
database** public;      // Public db
database** private;     // Private db
char* authString;

// Program

int main(int argc, char* argv[]) {
    // argv takes
    // ./dbclient portnum key [value]

    // Vars
    char* portnum = malloc(2 * sizeof(char));
    strcpy(portnum, "0\0");
    //char* key;
    //char* value;
    
    // Usage: dbserver authfile connections [portnum]
    //  argc:    0        1         2         [3]
    if (argc < 3) {
        fprintf(stderr, DBSERVER_USAGE);
        exit(1);
    }

    FILE* authfile;
    int connections = 0;
    
    // get input
    check_arguments(argc, argv, &authfile, &connections, &portnum);
   
    //const char* constport = portnum;
    public = db_allocate();
    private = db_allocate();
    // getting here means arguments are valid
    
    // open connection

    int fdServer;
    fdServer = open_listen(portnum, connections);
    
    clients = malloc(0 * sizeof(struct Client));
    process_connection(fdServer);

    free(clients);
    db_dealloc(public);
    db_dealloc(private);
    free(authString);

    return 0;
}


/* check_arguments()
 * -----------------
 * Checks arguments passed to the program. If criteria not met, program
 * will exit with appropriate exit status
 *
 * argc: number of arguments passed to the program. Should be less than maxArgs
 *       however, function will silently ignore extra arguments if provided
 * argv: array of array of characters (array of Strings) containing the 
 *       arguments passed to the program. These are checked in this function.
 * authfile: pointer to FILE* which opens the anthentication file.
 * connections: pointer to integer which will contain connections.
 * portnum: pointer to integer which will contain the port number if specified.
 */
void check_arguments(int argc, char* argv[], FILE** authfile, 
        int* connections, char** portnum) {
    // connections
    for (int i = 0; i < strlen(argv[2]); i++) {
        if (!isdigit(argv[2][i])) {
            // alpha character detected
            // print to stderr err message
            fprintf(stderr, DBSERVER_USAGE);
            fflush(stderr);
            exit(1);
        }
    }
    
    *connections = atoi(argv[2]);
    
    if (*connections < 0) {
        fprintf(stderr, DBSERVER_USAGE);
        fflush(stderr);
        exit(1);
    }
    
    if (argc >= 4) {        // portnum
        for (int i = 0; i < strlen(argv[3]); i++) {
            if (!isdigit(argv[3][i])) {
                fprintf(stderr, DBSERVER_USAGE);
                fflush(stderr);
                exit(1);
            }
        }
        int intport = atoi(argv[3]);
        *portnum = realloc(*portnum, strlen(argv[3]) * sizeof(char));
        strcpy(*portnum, argv[3]);
        
        if ((intport < 1024 || intport > 65535) && intport != 0) {
            fprintf(stderr, DBSERVER_USAGE);
            fflush(stderr);
            exit(1);
        }
    }

    // authfile
    if ((*authfile = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, DBSERVER_AUTH_READ);
        fflush(stderr);
        exit(2);
    }
    authString = read_line(*authfile);  // first line is authentication string
    if (authString == NULL) {
        fprintf(stderr, DBSERVER_AUTH_READ);
        fflush(stderr);
        exit(2);
    }
}


/* open_listen()
 * -------------
 * Opens port and listens on it
 *
 * port: port number as char*
 * connections: maximum number of connections as integer
 */
int open_listen(const char* port, int connections) {
    struct addrinfo* adinfo = 0;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;       // IPv4;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;    // listen on all IP addresses

    int error;
    if ((error = getaddrinfo(NULL, port, &hints, &adinfo))) {
        freeaddrinfo(adinfo);
        fprintf(stderr, "%s\n", gai_strerror(error));
        return 1;
    }

    freeaddrinfo(adinfo);

    int listenfd = socket(AF_INET, SOCK_STREAM, 0); // 0 = default protocol TCP

    int optVal = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optVal, 
                sizeof(int)) < 0) {
        perror("Error settings socket option");
        exit(1);
    }

    if (bind(listenfd, (struct sockaddr*)adinfo->ai_addr, 
                sizeof(struct sockaddr)) < 0) {
        //perror("Binding");
        fprintf(stderr, DBSERVER_SOCK);
        exit(3);
    }

    if (listen(listenfd, connections) < 0) {
        perror("Listen");
        return 4;
    }

    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    
    if (getsockname(listenfd, (struct sockaddr*)&sin, &len)) {
        perror("getsockname");
    } else {
        fprintf(stderr, "%d\n", ntohs(sin.sin_port));
        // print port number as per spec #133
    }
    
    return listenfd;
}


/* process_connection()
 * --------------------
 * Handles connection from client to the server
 * 
 * fdServer: file descriptor which handles connection to client
 */
void process_connection(int fdServer) {
    struct sockaddr_in fromAddr;
    socklen_t fromAddrSize;
    int write;
    int read;

    while(1) {
        fromAddrSize = sizeof(struct sockaddr_in);

        write = accept(fdServer, (struct sockaddr*)&fromAddr, &fromAddrSize);
        read = dup(write); 

        clientid++;
        clients = realloc(clients, (clientid + 1) * sizeof(struct Client));
        
        int* id = malloc(sizeof(int));
        *id = clientid;
        clients[*id].write = fdopen(write, "w");
        clients[*id].read = fdopen(read, "r");
        
        if (write < 0) {
            perror("Error accepting connection");
            exit(1);
        }

        char hostname[NI_MAXHOST];
        int error = getnameinfo((struct sockaddr*)&fromAddr, 
                fromAddrSize, hostname, NI_MAXHOST, NULL, 0, 0);
        if (error) {
            fprintf(stderr, "Error getting hostname: %s\n", 
                    gai_strerror(error));
        } else {
            printf("Accepted connection from %s (%s), port %d\n",
                    inet_ntoa(fromAddr.sin_addr), hostname, 
                    ntohs(fromAddr.sin_port));
        }

        pthread_create(&clients[*id].threadId, NULL, client_thread, id);
        pthread_detach(clients[*id].threadId);
    }
}


/* client_thread()
 * ---------------
 * Thread which handles communication with client
 * 
 * arg: void pointer which contains file descriptor used for talking with 
 *      client
 *
 * Returns: void* used by pthread to handle the client_thread
 */
void* client_thread(void* arg) {
    int id = *(int*)arg;
    free(arg);
    
    HTTP_request_loop(id);
        
    // error or EOF - client disconnect
    fclose(clients[id].write);
    fclose(clients[id].read);
    
    return NULL;
}

/* HTTP_request_loop()
 * -------------------
 * Loops infinitely and waits for HTTP requests from client. Processes them
 * and sends response.
 * 
 * id: id number for current pthread
 */
void HTTP_request_loop(int id) {
    char* method, * address, * body;
    HttpHeader** headers = 0;
    database** db; 
    char* key, * value;
    int authStatus = 0;

    // response
    int status = 0;
    char* statusExplanation;
    char* resBody;
    HttpHeader** resHeaders;
    int numResHeaders = 0;

    while (1) {
        if (get_HTTP_request(clients[id].read, &method, &address, &headers, 
                    &body)) {   // success
            // Address
            char** addrToks = split_by_char(address, '/', 0);
            // 0 = "" since / at start, 1 = table, 2 = key
            if (sizeof(addrToks) < 3) {     // invalid request
                break;
            }

            statusExplanation = calloc(15, sizeof(char));
            resHeaders = malloc(1 * sizeof(HttpHeader));
            resHeaders[0] = NULL; 

            resBody = (char*)calloc(1, sizeof(char));
            strcpy(resBody, "\0");
            Http_response(id, &db, addrToks, &headers, &authStatus, &key, 
                    &value, &resHeaders, &numResHeaders, &resBody, &status, 
                    &statusExplanation, &body, &method);

            // Response
            char* response = construct_HTTP_response(status, statusExplanation, 
                    resHeaders, resBody);
            fprintf(clients[id].write, "%s", response);
            fflush(clients[id].write);

            
            free(method);
            //free(address);
            free(addrToks);
            if (headers != NULL) {
                free_array_of_headers(headers);
            }
            if (body != NULL) {
               free(body);
            }
            
            if (resHeaders != NULL) {
                free_array_of_headers(resHeaders);
            }
            free(statusExplanation);
            free(resBody);
        } else {
            return;      // failure
        }
    }
}

/* Http_response()
 * ---------------
 * Configures HTTP response before constructing it into a string using library
 * function and printing it to the write FILE* for thread with id given.
 *
 * db: 
 */
void Http_response(int id, database*** db, char** addrToks, 
        HttpHeader*** headers, int* authStatus, char** key, char** value, 
        HttpHeader*** resHeaders, int* numResHeaders, char** resBody, 
        int* status, char** statusExplanation, char** body, char** method) {
    *key = malloc((strlen(addrToks[2])) * sizeof(char));
    strcpy(*key, addrToks[2]);
    
    // Authorisation
    if (select_database(db, addrToks[1], headers, authStatus)) {
        *status = HTTP_BAD_REQUEST;
        strcpy(*statusExplanation, HTTP_BAD_REQUEST_TXT);
    } else if (*authStatus == 0 && !strcmp(addrToks[1], PRIVATE)) {
        // auth header not provided and private db requested
        *status = HTTP_UNAUTHORIZED; // HTTP response 401 Unauthorized
        strcpy(*statusExplanation, HTTP_UNAUTHORIZED_TXT);
    } else {     // Check methods
        Http_methods(db, status, statusExplanation, body, resBody, key, value, 
                method, headers, resHeaders);
    } 
    
    // Content Length Header (yet to create function for)
    int body_len = (int)strlen(*resBody) + 1;
    char* body_len_char = malloc(sizeof(body_len) *
            sizeof(char));
    sprintf(body_len_char, "%i", body_len);
    add_header(resHeaders, numResHeaders, CON_LEN, body_len_char);
    free(body_len_char);
    free(*key);
}

/* select_database()
 * -----------------
 * Selects datbase from address of HTTP request. Checks for authorzation
 * header if private database is requested
 *
 * db: pointer to array of databases (records)
 * dbSelect: string entered by user which selects the database
 * headers: pointer to array of HttpHeaders which are iterated over for auth
 * authStatus: keeps track of status of authorisation string
 *
 * Returns: Status of database select. 0 = success, 1 = fail.
 */
int select_database(database*** db, char* dbSelect, HttpHeader*** headers,
        int* authStatus) {
    if (!strcmp(dbSelect, PUBLIC)) {
        *db = public;
    } else if (!strcmp(dbSelect, PRIVATE)) {
        *db = private;
        // check for auth key. If not valid, exit loop
        // Do this by looping over headers. If authentication header 
        // found then look at the value. If value matches auth.txt, 
        // then the user is granted access to the /private/ database
        HttpHeader** ptr;
        for (ptr = *headers; ptr != NULL; ptr = ptr + sizeof(ptr)) {
            if (!strcmp((*ptr)->name, AUTHORIZATION)) {
                // auth header detected
                *authStatus = 1;
                if (!strcmp((*ptr)->value, authString)) {
                    // correct authString entered
                    *authStatus = 2;
                }
                break;  // can skip checking rest of headers
            }
        }
    } else {
        return 1;       // invalid address BAD REQUEST
    }

    return 0;
}

/* Http_methods()
 * --------------
 * Handles selection of Status line, headers and body based on given request
 * 
 * db: pointer to array of databases (records)
 * status: pointer to integer which handles status of response
 * statusExplanation: pointer to array of characters which stores status text
 * body: pointer to array of characters which stores request body
 * resBody: pointer to array of characters which handles response body
 * key: pointer to array of characters storing client supplied key
 * value: pointer to array of characters which handles value to and from client
 * method: pointer to array of characters which handles request method
 * headers: pointer to array of HttpHeaders which stores request headers
 * resHeaders: pointer to array of HttpHeaders used for HTTP response
 *
 * Returns: status of methods selection. 0 = success, 1 = fail.
 */
void Http_methods(database*** db, int* status, char** statusExplanation,
        char** body, char** resBody, char** key, char** value, char** method,
        HttpHeader*** headers, HttpHeader*** resHeaders) {
    if (!strcmp(*method, HTTP_GET)) {
        *value = get_record(**db, *key);
        if (*value == NULL) {
            *status = HTTP_NOT_FOUND;
            strcpy(*statusExplanation, HTTP_NOT_FOUND_TXT);
        } else {
            *status = HTTP_OK;
            strcpy(*statusExplanation, HTTP_OK_TXT);

            // Body
            *resBody = realloc(*resBody, (strlen(*value) + 1) * sizeof(char));
            strcpy(*resBody, *value);
        }
    } else if (!strcmp(*method, HTTP_PUT)) {
        if (!strlen(*body)) {    // If body isn't ""
            if (header_exist(*headers, CON_LEN)) {   // CON-LEN header missing
                *status = HTTP_BAD_REQUEST;
                strcpy(*statusExplanation, HTTP_BAD_REQUEST_TXT);
                return;
            }
        }
        add_record(*db, *key, *body);       // Add record
        *status = HTTP_OK;                  // 200 OK status
        strcpy(*statusExplanation, HTTP_OK_TXT);
    } else if (!strcmp(*method, HTTP_DELETE)) {
        *value = get_record(**db, *key);
        if (*value == NULL) {
            *status = HTTP_NOT_FOUND;       // Error 404
            strcpy(*statusExplanation, HTTP_NOT_FOUND_TXT);
        } else {
            delete_record(*db, *key);       // Delete record
            *status = HTTP_OK;              // 200 OK Response
            strcpy(*statusExplanation, HTTP_OK_TXT);
        }
    } else {
        *status = HTTP_BAD_REQUEST;
        strcpy(*statusExplanation, HTTP_BAD_REQUEST_TXT);
    }
}

/* add_header()
 * ------------
 * Allocates memory for a header in the HttpHeader** array and sets name value
 * pair.
 *
 * headerArr: Pointer to HttpHeader** where name value pair is to be added.
 * numH: Pointer to integer of number of headers in array exc NULL terminator.
 * name: char* containing name of header.
 * value: char* containing value of header.
 */
void add_header(HttpHeader*** headerArr, int* numH, char* name, 
        char* value) { 
    *headerArr = realloc(*headerArr, (*numH + 2) * sizeof(HttpHeader*));
    (*headerArr)[*numH] = malloc(sizeof(HttpHeader));
    ((*headerArr)[*numH])->name = malloc(strlen(name) * sizeof(char));
    ((*headerArr)[*numH])->value = malloc(strlen(value) * sizeof(char));
    strcpy(((*headerArr)[*numH])->name, name);
    strcpy(((*headerArr)[*numH])->value, value);
    (*headerArr)[*numH + 1] = NULL;
    
    (*numH)++; 
}

/* edit_header()
 * -------------
 * Edits an HttpHeader array's value
 * 
 * headerArr: array of HttpHeaders to edit
 * name: char* containing HttpHeader name to edit
 * newValue: char* containing new value to update
 */
void edit_header(HttpHeader*** headerArr, char* name, char* newValue) {
    HttpHeader** ptr;
    for (ptr = *headerArr; ptr != NULL; ptr = ptr + sizeof(ptr)) {
        if (!strcmp((*ptr)->name, name)) {
            (*ptr)->value = realloc((*ptr)->value, 
                    strlen(newValue) * sizeof(char));
            strcpy((*ptr)->value, newValue);
            return;
        }
    }
}

/* check_header()
 * --------------
 * Checks a given array of HttpHeaders to see if a given name value pair exist.
 *
 * headerArr: array of HttpHeaders to check
 * nameMatch: name to find
 * valueMatch: value to match
 *
 * Returns: 0 if match, 1 if no match
 */
int check_header(HttpHeader** headerArr, char* nameMatch, char* valueMatch) {
    HttpHeader** ptr;
    for (ptr = headerArr; ptr != NULL; ptr = ptr + sizeof(ptr)) {
        if (!strcmp((*ptr)->name, nameMatch)) {
            if (!strcmp((*ptr)->value, valueMatch)) {
                return 0;
            }
            break;
        }
    }

    return 1;
}

/* header_exist()
 * --------------
 * Checks a HttpHeader array to see if a header with given name exists.
 *
 * headerArr: HttpHeader array to iterate over
 * name: HttpHeader name to check for
 *
 * Returns: 0 if header exists, 1 if doesn't exist
 */
int header_exist(HttpHeader** headerArr, char* name) {
    HttpHeader** ptr;
    for (ptr = headerArr; ptr != NULL; ptr = ptr + sizeof(ptr)) {
        if (!strcmp((*ptr)->name, name)) {
            return 1;
        }
    }

    return 1;
}

/* db_allocate()
 * -------------
 * Allocates dynamic memory for the database table
 *
 * Returns: database** - a pointer to a pointer to a database. That is a
 *                       pointer to an array of databases (or records)
 */
database** db_allocate(void) {
    return malloc(sizeof(database));
}

/* db_dealloc()
 * ------------
 * Frees a database struct from stack
 *
 * db: database to free
 */
void db_dealloc(database** db) {
    free(db);
}

/* get_record()
 * ------------
 * Gets record from database db given key string
 *
 * db: database to search within
 * key: string to query for
 *
 * Returns: void* - void pointer to record in the database
 */
void* get_record(database* db, char* key) {
    database* ptr;
    for (ptr = db; ptr != NULL; ptr = ptr->next) {
        if (!strcmp(ptr->key, key)) {
            return ptr->value;
        }
    }
    
    return NULL;
}

/* add_record()
 * ------------
 * Adds a record to the database with key key and value value
 *
 * db: database to add record to
 * key: key at which to store value
 * value: string to store in the record
 */
void add_record(database** db, char* key, char* value) {
    delete_record(db, key); // Clear key incase present
    database* d = malloc(sizeof(struct database_struct));
    d->key = malloc(strlen(key) + 1);
    strcpy(d->key, key);
    d->value = malloc(strlen(value) + 1);
    strcpy(d->value, value);
    d->next = *db;
    *db = d;
}

/* delete_record()
 * ---------------
 * Deletes record from database db with key key
 *
 * db: database to delete record from
 * key: identifier of record to delete
 */
void delete_record(database** db, char* key) {
    database* ptr;
    database* prev;
    for (ptr = *db, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next) {
        if (!strcmp(ptr->key, key)) {
            if (ptr->next != NULL) {
                if (prev == NULL) {
                    *db = ptr->next;
                } else {
                    prev->next = ptr->next;
                }
            } else if (prev != NULL) {
                prev->next = NULL;
            } else {
                *db = NULL;
            }

            free(ptr->key);
            free(ptr);

            return;
        }
    }
}
