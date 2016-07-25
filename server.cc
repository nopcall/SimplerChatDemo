#include "server.H"

#include <pthread.h>
#include <string.h>

#include <iostream>

Server::
Server(int port, int maxConnection)
        : _port(port), _maxConnection(maxConnection), _connected(0), onlineUser(NULL)
{
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
        listen(srvfd, _maxConnection);
        std::cout << "listening ..." << std::endl;

        // accept client connect
        onlineUser = (Connection *)malloc(sizeof(Connection) * _maxConnection);
        memset(onlineUser, 0, sizeof(Connection) * _maxConnection);

        while (onlineUser[_connected].fd =
               accept(srvfd, (struct sockaddr *)&(onlineUser[_connected].addr),
                      &onlineUser[_connected].addrLen)) {
                // 交付给处理线程
                handlerArg cliArg = {_connected, this};
                pthread_create(&onlineUser[_connected].tid, NULL, clientHandler, (void*)&cliArg);

                _connected++;
        }
}

void*
Server::clientHandler(void *arg)
{
        handlerArg *rarg = (handlerArg *)arg;
        int i = rarg->i;
        Server *srv = rarg->srv;

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
                        printf("data:%s\n", msgBuf->data);
                        // 回显
                        write(srv->onlineUser[i].fd, msgBuf, sizeof(MsgFormat));

                        return nullptr;
                        break;
                case 1: // login
                        break;
                case 2: // logout
                        break;
                case 3: //sendMsgToFriend
                        break;
                default:
                        break;
                }

                readByte = 0;
                readLeft = sizeof(MsgFormat) - readByte;
        }

        return NULL;
}

int
main(int argc, char *argv[])
{
        Server instant;

        return 0;
}
