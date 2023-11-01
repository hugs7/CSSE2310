// CSSE2310 Assignment 4 a4
// By Hugo Burton
// Start Date: 4pm Friday 13th May, 2022
// Due Date: 4pm Friday 3rd June, 2022
// Student Number: s4698512 (46985123)

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
#include <semaphore.h>
#include <signal.h>

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
#define HTTP_NOT_FOUND_TXT "Not Found\0"
#define HTTP_UNAUTHORIZED 401
#define HTTP_UNAUTHORIZED_TXT "Unauthorized\0"

#define CON_LEN "Content-Length\0"
#define AUTHORIZATION "Authorization\0"

#define ZERO_STR "0\0"

// Structs

/* Stores a StringStore along with a semlock */
struct Database {
    StringStore* ss;
    sem_t* guard;
};

/* Stores information for the clients including file streams and databases */
struct Client {
    pthread_t threadId;
    FILE* write;
    FILE* read;
    struct Database public;
    struct Database private;
};

/* Handles statistics for dbserver */
struct Statistics {
    int conClients;
    int comClients;
    int authFails;
    int get;
    int put;
    int delete;
    sem_t* guard;
};

// --- Function Declarations ---

void check_arguments(int argc, char* argv[], FILE** authfile, 
        int* connections, char** portnum); 

int open_listen(const char* port, int connections);

void process_connection(int fdServer);

void client_init(struct Client* client, int read, int write, 
        struct Database public, struct Database private, sem_t* pubGuard, 
        sem_t* priGuard);

void* client_thread(void* arg);

void http_request_loop(struct Client client);

void http_response(struct Client client, struct Database db, char** addrToks, 
        HttpHeader*** headers, int* authStatus, char** key, char** value, 
        HttpHeader*** resHeaders, int* numResHeaders, char** resBody, 
        int* status, char* statusExplanation, char* body, char* method); 

int select_stringstore(struct Client client, struct Database* db, 
        char* ssSelect, HttpHeader** headers, int* authStatus);

void http_methods(struct Database db, int* status, char* statusExplanation,
        char* body, char** resBody, const char* key, const char* value, 
        char* method, HttpHeader*** headers, HttpHeader*** resHeaders);

void add_header(HttpHeader*** headerArr, int* numH, char* name, char* value); 

void edit_header(HttpHeader*** headerArr, char* name, char* newValue);

int check_header(HttpHeader** headerArr, char* nameMatch, char* valueMatch);

int header_exist(HttpHeader** headerArr, char* name);

void init_lock(sem_t* l);

void take_lock(sem_t* l);

void release_lock(sem_t* l);

void init_statistics(struct Statistics* s);

void signal_handle(int s);

// Globals

/* authentication string for private database */
char* authString;

/* structure Statistics handling statistics for server. Must be global for 
 * accessibility in signal handler */
struct Statistics stats;

// Program

int main(int argc, char* argv[]) {
    char* portnum = malloc(2 * sizeof(char));
    strcpy(portnum, "0\0");

    struct sigaction sa; 
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handle;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &sa, 0);
    
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
    // getting here means arguments are valid
    
    // Server stats
    init_statistics(&stats);
    
    // open connection
    int fdServer;
    fdServer = open_listen(portnum, connections);
     
    process_connection(fdServer);
    
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
    int clientId = -1;
    struct Client* clients;
    clients = malloc(0 * sizeof(struct Client));
    int write, read;
    struct Database public;
    struct Database private;
    public.ss = stringstore_init();
    private.ss = stringstore_init();
    sem_t pubGuard, priGuard;
    init_lock(&pubGuard);
    init_lock(&priGuard);

    while (1) {
        fromAddrSize = sizeof(struct sockaddr_in);
        write = accept(fdServer, (struct sockaddr*)&fromAddr, &fromAddrSize);
        read = dup(write); 
        clientId++;
        clients = realloc(clients, (clientId + 1) * sizeof(struct Client));
        client_init(&clients[clientId], read, write, public, private, 
                &pubGuard, &priGuard);
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
            stats.conClients++;
        }
        pthread_create(&clients[clientId].threadId, NULL, client_thread, 
                &clients[clientId]);
        pthread_detach(clients[clientId].threadId);
    }

    stringstore_free(public.ss);
    stringstore_free(private.ss);
    sem_destroy(&pubGuard);
    sem_destroy(&priGuard);
    sem_destroy(stats.guard);
    free(clients);
}

/* client_init()
 * -------------
 * Initialises the struct before passing to client pthread
 *
 * client: pointer to struct Client which will contain information needed by 
 *         the client thread
 */
void client_init(struct Client* client, int read, int write, 
        struct Database public, struct Database private, sem_t* pubGuard, 
        sem_t* priGuard) {
    client->write = fdopen(write, "w");
    client->read = fdopen(read, "r");
    client->public = public;
    client->private = private;
    client->public.guard = pubGuard;
    client->private.guard = priGuard;
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
    struct Client client = *(struct Client*)arg;
    http_request_loop(client);
        
    // error or EOF - client disconnect
    stats.conClients--;
    stats.comClients++;
    fclose(client.write);
    fclose(client.read);
    
    return NULL;
}

/* HTTP_request_loop()
 * -------------------
 * Loops infinitely and waits for HTTP requests from client. Processes them
 * and sends response.
 * 
 * id: id number for current pthread
 */
void http_request_loop(struct Client client) {
    char* method, *address, *body;
    HttpHeader** headers = 0;
    struct Database db;
    char* key, *value, *resBody, *statusExplanation;
    int authStatus = 0, status = 0, numResHeaders = 0;
    HttpHeader** resHeaders;

    while (1) {
        if (get_HTTP_request(client.read, &method, &address, &headers, 
                &body)) {   // success
            char** addrToks = split_by_char(address, '/', 0);
            if (sizeof(addrToks) < 3) {     // invalid request
                break;
            }

            statusExplanation = calloc(15, sizeof(char));
            resHeaders = malloc(1 * sizeof(HttpHeader));
            resHeaders[0] = NULL; 

            resBody = (char*)calloc(1, sizeof(char));
            strcpy(resBody, "\0");
            http_response(client, db, addrToks, &headers, &authStatus, &key, 
                    &value, &resHeaders, &numResHeaders, &resBody, &status, 
                    statusExplanation, body, method);

            char* response = construct_HTTP_response(status, 
                    statusExplanation, resHeaders, resBody);
            fprintf(client.write, "%s", response);
            fflush(client.write);
            
            free(method);
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

/* http_response()
 * ---------------
 * Configures HTTP response before constructing it into a string using library
 * function and printing it to the write FILE* for thread with id given.
 *
 * client: structure Client which stores information used
 * db: database being accessed
 * addrToks: address requested tokens
 * headers: pointer to array of HttpHeaders used in request
 * authStatus: status of authentication used for private db
 * key: pointer to string handling key
 * value: pointer to string handling value
 * status: pointer to integer handling response status
 * statusExplanation: string handling HTTP response text
 * body: string handling request body
 * method: string handling request method
 */
void http_response(struct Client client, struct Database db, char** addrToks, 
        HttpHeader*** headers, int* authStatus, char** key, char** value, 
        HttpHeader*** resHeaders, int* numResHeaders, char** resBody, 
        int* status, char* statusExplanation, char* body, char* method) {
    *key = addrToks[2];
    const char* constKey = *key;
    const char* constValue = *value;

    // Authorisation
    if (select_stringstore(client, &db, addrToks[1], *headers, authStatus)) {
        *status = HTTP_BAD_REQUEST;
        strcpy(statusExplanation, HTTP_BAD_REQUEST_TXT);
    } else if (*authStatus == 0 && !strcmp(addrToks[1], PRIVATE)) {
        // auth header not provided and private ss requested
        *status = HTTP_UNAUTHORIZED; // HTTP response 401 Unauthorized
        strcpy(statusExplanation, HTTP_UNAUTHORIZED_TXT);
        stats.authFails++;
    } else {     // Check methods
        http_methods(db, status, statusExplanation, body, resBody, constKey, 
                constValue, method, headers, resHeaders);
    } 

    // Content Length Header (yet to create function for)
    int bodyLen = (int)strlen(*resBody) + 1;
    char* bodyLenChar = malloc(sizeof(bodyLen) *
            sizeof(char));
    sprintf(bodyLenChar, "%i", bodyLen);
    add_header(resHeaders, numResHeaders, CON_LEN, bodyLenChar);
    free(bodyLenChar);
}

/* select_stringstore()
 * --------------------
 * Selects datbase from address of HTTP request. Checks for authorzation
 * header if private database is requested
 *
 * ss: StringStore array
 * ssSelect: string entered by user which selects the StringStore
 * headers: pointer to array of HttpHeaders which are iterated over for auth
 * authStatus: keeps track of status of authorisation string
 *
 * Returns: Status of database select. 0 = success, 1 = fail.
 */
int select_stringstore(struct Client client, struct Database* db, 
        char* ssSelect, HttpHeader** headers, int* authStatus) {
    if (!strcmp(ssSelect, PUBLIC)) {
        *db = client.public;
    } else if (!strcmp(ssSelect, PRIVATE)) {
        *db = client.private;
        // check for auth key. If not valid, exit loop
        // Do this by looping over headers. If authentication header 
        // found then look at the value. If value matches auth.txt, 
        // then the user is granted access to the /private/ database
        HttpHeader** ptr;
        for (ptr = headers; *ptr != NULL; ptr = ptr + sizeof(ptr)) {
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

/* http_methods()
 * --------------
 * Handles selection of Status line, headers and body based on given request
 * 
 * ss: array of StringStores
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
void http_methods(struct Database db, int* status, char* statusExplanation,
        char* body, char** resBody, const char* key, const char* value, 
        char* method, HttpHeader*** headers, HttpHeader*** resHeaders) {
    take_lock(db.guard);
    take_lock(stats.guard);
    if (!strcmp(method, HTTP_GET)) {
        value = stringstore_retrieve(db.ss, key);
        if (value == NULL) {
            *status = HTTP_NOT_FOUND;
            strcpy(statusExplanation, HTTP_NOT_FOUND_TXT);
        } else {
            *status = HTTP_OK;
            strcpy(statusExplanation, HTTP_OK_TXT);

            *resBody = strdup(value);        // body
        }
        stats.get++;        // stats
    } else if (!strcmp(method, HTTP_PUT)) {
        if (!strlen(body)) {    // If body isn't ""
            if (header_exist(*headers, CON_LEN)) {   // CON-LEN header missing
                *status = HTTP_BAD_REQUEST;
                strcpy(statusExplanation, HTTP_BAD_REQUEST_TXT);
                release_lock(db.guard);
                release_lock(stats.guard);
                return;
            }
        }
        value = body;
        stringstore_add(db.ss, key, value);       // Add record
        *status = HTTP_OK;                  // 200 OK status
        strcpy(statusExplanation, HTTP_OK_TXT);
        stats.put++;        // stats
    } else if (!strcmp(method, HTTP_DELETE)) {
        value = stringstore_retrieve(db.ss, key);
        if (value == NULL) {
            *status = HTTP_NOT_FOUND;       // Error 404
            strcpy(statusExplanation, HTTP_NOT_FOUND_TXT);
        } else {
            stringstore_delete(db.ss, key);       // Delete record
            *status = HTTP_OK;              // 200 OK Response
            strcpy(statusExplanation, HTTP_OK_TXT);
        }
        stats.delete++;     // stats
    } else {
        *status = HTTP_BAD_REQUEST;
        strcpy(statusExplanation, HTTP_BAD_REQUEST_TXT);
    }

    release_lock(db.guard);
    release_lock(stats.guard);
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
void add_header(HttpHeader*** headerArr, int* numH, char* name, char* value) { 
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

/* init_lock()
 * -----------
 * initialises sem_t* l passed to it
 *
 * l: pointer to sem_t
 */
void init_lock(sem_t* l) {
    sem_init(l, 0, 1);
}

/* take_lock()
 * -----------
 * locks sem_t* l passed to it
 *
 * l: pointer to sem_t
 */
void take_lock(sem_t* l) {
    sem_wait(l);
}

/* release_lock()
 * -----------
 * unlocks sem_t* l passed to it
 *
 * l: pointer to sem_t
 */
void release_lock(sem_t* l) {
    sem_post(l);
}

/* init_statistics()
 * -----------------
 * Initialises statistics for use by server
 *
 * s: pointer to struct Statistics
 */
void init_statistics(struct Statistics* s) {
    s->conClients = 0;
    s->comClients = 0;
    s->authFails = 0;
    s->get = 0;
    s->put = 0;
    s->delete = 0;
    sem_t l;
    init_lock(&l);
    s->guard = &l;
}

/* signal_handle()
 * --------------
 * A signal handler function for SIGHUP which is used for statistics recording.
 *
 * s: signal number
 */
void signal_handle(int s) {
    if (s == SIGHUP) {
        fprintf(stderr, "Connected clients:%i\n", stats.conClients);
        fprintf(stderr, "Completed clients:%i\n", stats.comClients);
        fprintf(stderr, "Auth failures:%i\n", stats.authFails);
        fprintf(stderr, "%s operations:%i\n", HTTP_GET, stats.get);
        fprintf(stderr, "%s operations:%i\n", HTTP_PUT, stats.put);
        fprintf(stderr, "%s operations:%i\n", HTTP_DELETE, stats.delete);
        fflush(stderr);
    }
}

