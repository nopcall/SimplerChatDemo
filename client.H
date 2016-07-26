#ifndef CLIENT_H
#define CLIENT_H

#include <unistd.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>

#include <pthread.h>

struct MsgFormat {
        // 0: register
        // 1: login
        // 2: logout
        // 3: sendMsgToFriend
        int type;
        char msgContent[256];
        char userName[32];
        char userPassword[48];
};

class Client {
public:
        Client() { pthread_mutex_init(&_mutex, NULL); };
        ~Client() = default;

        static void* outHandler(void *arg);
        static void* inHandler(void *arg);
private:
        pthread_mutex_t _mutex;
};

#endif /* CLIENT_H */
