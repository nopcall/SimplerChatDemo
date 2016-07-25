#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>

// socket头
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>

class Server;
class Connection;
class handlerArg;

struct MsgFormat {
        // 0: register
        // 1: login
        // 2: logout
        // 3: sendMsgToFriend
        // 4: heartbeat
        int type;
        char data[256];
};

struct handlerArg {
        int i;
        Server *srv;
};

class Connection {
public:
        int fd;
        pthread_t tid;
        sockaddr_in addr;
        socklen_t addrLen;
        char userName[32];
};

class Server {
public:
        Server(int port = 9090, int maxConnection = 10);
        ~Server() = default;

        // 负责启动服务器
        void initServer();
        static void* clientHandler(void *arg);

        // 连接数据库和发送命令
        int sqlConnect();
        int sqlCmd();

        // 简单函数
        int userRegister();
        int userLogin();
        int userLogout();
        int userHeartBeat();

        // 功能
        int getFriendList();
        int sendMsgToFriend();

private:
        Connection *onlineUser;
        short _port;
        int _maxConnection;
        int _connected;
};

#endif /* SERVER_H */
