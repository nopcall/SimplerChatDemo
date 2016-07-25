#include "client.H"

#include <iostream>

#define DEBUG(arg) std::cerr << "error happen here NO." << arg << std::endl

void*
Client::
outHandler(void *arg)
{
        long srvfd = (long)arg;
        MsgFormat *msgbuf = (MsgFormat *)malloc(sizeof(MsgFormat));
        int readByte = 0;
        int readLeft = sizeof(MsgFormat) - readByte;

        while ((readByte = read(srvfd, msgbuf + readByte, readLeft)) != 0) {
                if (readByte != sizeof(MsgFormat)) { // 没读完 继续读
                        readLeft = sizeof(MsgFormat) - readByte;
                        continue;
                }

                std::cout << msgbuf->data << std::endl;

                readByte = 0;
                readLeft = sizeof(MsgFormat) - readByte;
        }

        return nullptr;
}

void*
Client::
inHandler(void *arg)
{
        long srvfd = (long)arg;

        char buf[256];

        // 输出流要更改成按行输入
        while (true) {
                scanf("%s", buf);

                struct MsgFormat msg;
                msg.type = 0;
                for (int i = 0; i < 256; i++) {
                        msg.data[i] = buf[i];
                }

                write(srvfd, &msg, sizeof(MsgFormat));
                memset(buf, 0, 256);
        }

        return nullptr;
}

int
main(int argc, char *argv[])
{

        int srvfd = socket(AF_INET, SOCK_STREAM, 0);

        sockaddr_in srvaddr;

        memset(&srvaddr, 0, sizeof(srvaddr));
        srvaddr.sin_family = AF_INET;
        srvaddr.sin_port = htons(9090);
        srvaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(srvfd, (const sockaddr *)&srvaddr, sizeof(srvaddr)) < 0) {
                std::cerr << "connect to server failed" << std::endl;
                exit(EXIT_FAILURE);
        }

        sleep(1);
        pthread_t tid[2];
        // 用户输出线程
        pthread_create(&tid[0], NULL, Client::inHandler, (void *)srvfd);
        // 服务器回显线程
        pthread_create(&tid[1], NULL, Client::outHandler, (void *)srvfd);

        pthread_join(tid[0], NULL);
        pthread_join(tid[1], NULL);

        return 0;
}
