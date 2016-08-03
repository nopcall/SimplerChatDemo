#ifndef SERVER_H
#define SERVER_H

#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// socket头
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/epoll.h>

#include <vector>

#include <mysql++/mysql++.h>

#include "Timer.H"

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
        char msgContent[256];
        char userName[32];
        char userPassword[48];
};

struct handlerArg {
        int i;
        Server *srv;
};

struct Connection {
        int fd;
        pthread_t tid;
        sockaddr_in addr;
        socklen_t addrLen;
        char userName[32];
        int beat;
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
        int sqlCmd(char *cmd);

        // 简单函数
        int userRegister();
        int userLogin();
        int userLogout();
        static void* userHeartBeat(void *arg);

        // 功能
        int getFriendList();
        int sendMsgToFriend();

private:
        Connection *onlineUser;
        short _port;
        int _maxConnection;
        int _connected;
        // mysql
        mysqlpp::Connection _sql;
        char *_sqlAddr;
        char *_sqlUserName;
        char *_sqlPassword;
        char *_databaseName;
};

#endif /* SERVER_H */
