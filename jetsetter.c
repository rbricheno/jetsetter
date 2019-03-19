#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hiredis.h>


int main(int argc, char **argv) {
    char *line = NULL;
    char *pch = NULL;
    size_t len = 0;
    ssize_t read;
    char fullkeyname[255];

    unsigned int j, isunix = 0;
    redisContext *c;
    redisReply *reply;

    if (argc < 2) {
        printf("error: missing command line argument (key name root)\n");
        return 1;
    }

    const char *keyroot = argv[1];

    if (strlen(keyroot) > 250) {
        printf("error: key name root must not exceed 250 characters\n");
        return 1;
    }

    const char *hostname = (argc > 2) ? argv[2] : "127.0.0.1";

    if (argc > 3) {
        if (*argv[3] == 'u' || *argv[3] == 'U') {
            isunix = 1;
            /* in this case, host is the path to the unix socket */
            printf("Will connect to unix socket @%s\n", hostname);
        }
    }

    int port = (argc > 3) ? atoi(argv[3]) : 6379;

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds

    if (isunix) {
        c = redisConnectUnixWithTimeout(hostname, timeout);
    } else {
        c = redisConnectWithTimeout(hostname, port, timeout);
    }
    if (c == NULL || c->err) {
        if (c) {
            printf("Connection error: %s\n", c->errstr);
            redisFree(c);
        } else {
            printf("Connection error: can't allocate redis context\n");
        }
        exit(1);
    }

    while ((read = getline(&line, &len, stdin)) != -1) {
        /* printf("Retrieved line of length %zu :\n", read); */
        /* printf("%s", line); */

        pch = strtok (line," ");
        int i = 0;
        while (pch != NULL) {
            /* printf ("%s%d\n",keyroot, i); */
            /* printf ("%s\n",pch); */

            sprintf(fullkeyname, "%s%d", keyroot, i);
            reply = redisCommand(c,"SET %s %s", fullkeyname, pch);
            /* printf("SET: %s\n", reply->str); */
            freeReplyObject(reply);

            pch = strtok (NULL, " ");

            i++;
            if (i >= 999) {
                /* that's too many! */
                break;
            }
        }
    }

    /* Disconnects and frees the context */
    redisFree(c);

    free(line);
    free(pch);

    return 0;
}
