#include "connection_server.h"
#include <time.h>
struct sockaddr_in address_server;

int parse_msg(int client, char *msg, options_t *options_client) {
    printf("\n\nNew Request\nParse_msg()\nclient after parse : %d \n", client);
    printf("Message parsed : %s\n\n", msg, msg);
    int code = 0;
    char content[MSG_SIZE];
    sscanf(msg, "%d %s", &code, content);
    switch(code) {
        case MSG_QUIT:

        case MSG_CHEAT_CODE:

            return OK;
        case MSG_START_GAME:;
            char *hangman_word = malloc(256);
            int line = randomizer(length_list(options_client->list), options_client);
            hangman_word = load_word(line, options_client->list);
            char *underscore;
            underscore = malloc(256);
            for(int i = 0; i < strlen(hangman_word); i++) {
                underscore[i] = '_';
            }
            if(send_string(client, MSG_WORD, underscore) == -1) {
                perror("Send_string()");
            }
            printf("The word %s at the %d line has been selected\n", hangman_word, ++line);
            printf("\nThis will be displayed to the client : %s", underscore);
            return OK;
        case MSG_OPTIONS_RCV:

            break;
        case MSG_OPTIONS_REQ:
            strcpy(options_client->name, content);
            printf("\n Client's name : %s \n", options_client->name);
            if(send_options(client, options_client) == -1) {
                perror("Error send_options");
            }
            break;
        case MSG_ADD_WORD:
            break;
        case MSG_LETTER:
            break;
        case MSG_WORD:
            break;
        case STRCT_NAME:
            strcpy(options_client->name, content);
            break;
        case STRCT_TRIES:
            options_client->tries = atoi(content);
            options_client->state = 64 - options_client->tries;
            break;
        case STRCT_MIN:
            options_client->min = atoi(content);
            break;
        case STRCT_MAX:
            options_client->max = atoi(content);
            break;
        case STRCT_TIME:
            options_client->time = atoi(content);
            break;
        case STRCT_TYPE:
            options_client->type = atoi(content);
            break;
        case STRCT_LIST:
            strcpy(options_client->name, content);
            break;
        default:
            printf("Default case encountered");
            break;
    }
}

int init_server() {
    int server_socket = socket(IPV4, TCP, PROTOCOL);
    memset(&address_server, 0, sizeof(address_server));
    address_server.sin_family = IPV4;
    address_server.sin_port = htons(CONNECT_PORT);
    inet_aton("127.0.0.1", &address_server.sin_addr);
    bind(server_socket, (struct sockaddr *)&address_server, sizeof address_server);
    listen(server_socket, SOMAXCONN);
    return server_socket;
}

int connect_client(int socket_in) {
    int *pclient = malloc(sizeof(int));
    if((*pclient = accept(socket_in, NULL, NULL)) == -1) {
        return -1;
    }
    pthread_t thread;
    return pthread_create(&thread, NULL, wait_client, pclient);
}

void *wait_client(void *p_client_socket) {
    int client_socket = *((int *)p_client_socket);
    free(p_client_socket);
    options_t *options_client;
    init_options(options_client);
    char *msg;
    msg = malloc(256);
    srand(time(0));
    printf("\nThread successfully created\n");
    while(recv(client_socket, msg, 256, 0) != -1) {
        parse_msg(client_socket, msg, options_client);
    }
    printf("Thread %d to close", client_socket);
}

int send_options(int client, options_t *options_client) {

    if(send_string(client, STRCT_NAME, options_client->name) == -1) {
        perror("Error when sending name");
    }
    if(send_int(client, STRCT_TRIES, options_client->tries) == -1) {
        perror("Error when sending tries");
    }
    if(send_int(client, STRCT_MIN, options_client->min) == -1) {
        perror("Error when sending min");
    }
    if(send_int(client, STRCT_MAX, options_client->max) == -1) {
        perror("Error when sending max");
    }
    if(send_int(client, STRCT_STATE, options_client->state) == -1) {
        perror("Error when sending state");
    }
    if(send_string(client, STRCT_LIST, options_client->list) == -1) {
        perror("Error when sending list");
    }

    if(send_int(client, STRCT_TIME, options_client->time) == -1) {
        perror("Error when sending time");
    }
    if(send_int(client, STRCT_TYPE, options_client->type) == -1) {
        perror("Error when sending type");
    }
    // EBAUCHE POUR TOUT ENVOYER D'UN COUP
    //  char *msg = malloc(MSG_SIZE);
    //  sprintf(msg, "%d~%d~%s", MSG_OPTIONS_RCV, sizeof game_options, game_options);
    //  printf("Affichage de msg après sprintf : %s",msg);
    //  printf("Avant le send");
    //  if (send(client, msg, MSG_SIZE, 0) == -1) {
    //      perror("Error send");
    //  }
}

int send_int(int client, int sig, int content) {
    char *msg = malloc(MSG_SIZE);
    sprintf(msg, "%d %d", sig, content);
    send(client, msg, MSG_SIZE, 0);
}
int send_string(int client, int sig, char *content) {
    char *msg = malloc(MSG_SIZE);
    sprintf(msg, "%d %s", sig, content);
    send(client, msg, MSG_SIZE, 0);
}