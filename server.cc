#include "server.H"

#include <pthread.h>
#include <string.h>

#include <iostream>
#include <string>

int
Server::
sqlConnect()
{
        _sql.connect(_databaseName, _sqlAddr, _sqlUserName, _sqlPassword);
        return 0;
}

Server::
Server(int port, int maxConnection)
        : _port(port), _maxConnection(maxConnection), _connected(0), onlineUser(NULL)
{
        // connect to mysql
        _sqlAddr = "localhost";
        _sqlUserName = "root";
        _sqlPassword = "mysql";
        _databaseName = "nChat";
        sqlConnect();

        // server
        initServer();
}

void
Server::
initServer()
{
        // socket fd
        int srvfd = socket(AF_INET, SOCK_STREAM, 0);

        // server address
        sockaddr_in srvAddr;
        memset(&srvAddr, 0, sizeof(srvAddr));
        srvAddr.sin_family = AF_INET;
        srvAddr.sin_port = htons(_port);
        srvAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        // bind socketfd with server address
        bind(srvfd, (const struct sockaddr *)&srvAddr, sizeof(srvAddr));

        // listenning socketfd begin
        if (listen(srvfd, _maxConnection) == 0) {
                std::cout << "listening ..." << std::endl;
        }

        // accept client connect
        onlineUser = (Connection *)malloc(sizeof(Connection) * _maxConnection);
        memset(onlineUser, 0, sizeof(Connection) * _maxConnection);

        // epoll
        int efd = epoll_create(64);
        if (efd == -1) {
                std::cerr << "epoll create error" << std::endl;
                return;
        }

        struct epoll_event fdevent;
        fdevent.data.fd = srvfd;
        fdevent.events = EPOLLIN | EPOLLOUT;
        epoll_ctl(efd, EPOLL_CTL_ADD, srvfd, &fdevent);

        struct epoll_event *responEvents =
                (struct epoll_event*)calloc(64, sizeof(struct epoll_event));
        int respondNum = 0;
        for ( ;  ;  ) {
                respondNum = epoll_wait(efd, responEvents, 64, -1);
                for (int i = 0; i < respondNum; i++) {
                        if (responEvents[i].data.fd == srvfd) {
                                onlineUser[_connected].fd =
                                        accept(srvfd,
                                               (struct sockaddr *)&(onlineUser[_connected].addr),
                                               &onlineUser[_connected].addrLen);

                                // 交付给处理线程
                                handlerArg cliArg = {_connected, this};
                                pthread_create(&onlineUser[_connected].tid, NULL, clientHandler, (void*)&cliArg);

                                _connected++;

                        }

                }
        }


        // while (onlineUser[_connected].fd =
        //        accept(srvfd, (struct sockaddr *)&(onlineUser[_connected].addr),
        //               &onlineUser[_connected].addrLen)) {
        //         // 交付给处理线程
        //         handlerArg cliArg = {_connected, this};
        //         pthread_create(&onlineUser[_connected].tid, NULL, clientHandler, (void*)&cliArg);

        //         _connected++;
        // }
}

void*
Server::
userHeartBeat(void *arg)
{
        Connection *cli = (Connection*)arg;
        if (cli->beat == 5) {
                // logout
                std::cout << "user:" << cli->userName << " no respond will kickout" << std::endl;
                close(cli->fd);
                return NULL;
        }
        MsgFormat hbMSG;
        hbMSG.type = 4;
        write(cli->fd, &hbMSG, sizeof(MsgFormat));
        cli->beat++;
        std::cout << "user:" << cli->userName << " heartBeat +1 " << std::endl;
        return (void *)1;
}

void*
Server::clientHandler(void *arg)
{
        handlerArg *rarg = (handlerArg *)arg;
        int i = rarg->i;
        Server *srv = rarg->srv;

        // heartbeat
        Timer heartBeatTimer(1, userHeartBeat, &srv->onlineUser[i]);
        heartBeatTimer.start();

        MsgFormat *msgBuf = (MsgFormat*)malloc(sizeof(MsgFormat));
        int readByte = 0;
        int readLeft = sizeof(MsgFormat) - readByte;

        while ((readByte = read(srv->onlineUser[i].fd, (msgBuf + readByte), readLeft)) != 0) {
                if (readByte != sizeof(MsgFormat)) { // 没读完整个包 继续读
                        readLeft = sizeof(MsgFormat) - readByte;
                        continue;
                }

                switch (msgBuf->type) {
                case 0:  // register
                        printf("type:%d\n", msgBuf->type);
                        printf("data:%s\n", msgBuf->msgContent);
                        // 回显
                        write(srv->onlineUser[i].fd, msgBuf, sizeof(MsgFormat));
                        break;

                case 1: {// login
                        char loginUser[32];
                        strcpy(loginUser, msgBuf->userName);
                        char loginPassword[48];
                        strcpy(loginPassword, msgBuf->userPassword);

                        std::string sqlcmd;
                        sqlcmd += " select * from userInfo ";
                        sqlcmd += " where userName = \"";
                        sqlcmd += loginUser;
                        sqlcmd += "\" and userPassword = \"";
                        sqlcmd += loginPassword;
                        sqlcmd += "\";";

                        mysqlpp::Query loginQuery = srv->_sql.query();
                        loginQuery << sqlcmd;
                        mysqlpp::StoreQueryResult res = loginQuery.store();

                        std::cout << "user:" << msgBuf->userName << " login" << std::endl;
                        break;
                }
                case 2: // logout
                        break;
                case 3: // sendMsgToFriend
                        break;
                case 4: { // heartbeat
                        srv->onlineUser[i].beat = 0;
                        break;
                }
                default:
                        break;
                }

                readByte = 0;
                readLeft = sizeof(MsgFormat) - readByte;
        }

        heartBeatTimer.stop();

        return NULL;
}

int
main(int argc, char *argv[])
{
        Server instant;

        return 0;
}
