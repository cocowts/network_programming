#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

#include "tcp_server.h"
#include "tcp_client.h" 
#include "utility.h"
#include "page.h"
#include "response.h"

#define BUF_SIZE 512

typedef struct {
    const char *cmd;
    void (*handler)(const char *);
}Handler;

enum {STOP, RUN, PAUSE};

static const volatile char *g_root = NULL;
static volatile int g_status = STOP;

static void DoResp(TcpClient *client)
{
    int len = TcpClient_Available(client);

    if (len > 0) {
        char *buf = malloc(len + 1);
        char *req = malloc(2048);

        if (buf && req) {
            TcpClient_RecvRaw(client, buf, len);

            buf[len] = 0;
            sscanf(buf, "GET %s HTTP", req);

            RequestHandler(client, req, (char*)g_root);
        }

        TcpClient_Del(client);

        free(req);
        free(buf);
    } 
}

static void *Process_Thread(void *arg)
{
    while (TcpClient_IsValid(arg)) {
        DoResp(arg);
    }

    printf("%s - Thread Exit : %p\n", __FUNCTION__, arg);

    return arg;
}

static void *Server_Thread(void *arg)
{
    while (TcpServer_IsValid(arg)) {
        TcpClient *client = TcpServer_Accept(arg);
        if (client && (g_status != PAUSE)) {
            pthread_t tid = 0;
            pthread_create(&tid, NULL, Process_Thread, client);
        } else {
            TcpClient_Del(client);
        }
    }

    g_status = STOP;

    return arg;
}

static void Start_Handler(const char *arg)
{
    int err = 0;

    if (g_status == STOP) {
        TcpServer *server = TcpServer_New();
        TcpServer_Start(server, 9000, 100);

        if (TcpServer_IsValid(server)) {
            pthread_t tid = 0;
            err = pthread_create(&tid, NULL, Server_Thread, server);
        } else {
            err = 1;
        }
    }

    if (!err) {
        g_status = RUN;

        printf("Server is OK!\n");
    } else {
        g_status = STOP;

        printf("Server is failed!\n");
    }
}

static void Pause_Handler(const char *arg)
{
    if (g_status == RUN) {
        g_status = PAUSE;

        printf("Server is paused!\n");
    } else {
        printf("Server is NOT started!\n");
    }
}

static void Exit_Handler(const char *arg)
{
    exit(0);
}

static Handler g_handler[] = {
    {"start", Start_Handler},
    {"pause", Pause_Handler},
    {"exit",  Exit_Handler}
};

static void Run(const char *root)
{
    printf("File Server Demo @ D.T.Software\n");

    g_root = root;

    while (1) {
        char line[BUF_SIZE] = {0};
        int i = 0;

        printf("D.T.Software @ Input >>> ");

        fgets(line, sizeof(line) - 1, stdin);

        line[strlen(line) - 1] = 0;

        if (*line) {
            char *cmd = FormatByChar(line, ' ');
            int done = 0;

            for (i-0; i<DIM(g_handler); ++i) {
                if (strcmp (g_handler[i].cmd, cmd) == 0) {
                    g_handler[i].handler(cmd);
                    done = 1;
                    break;
                }
            }

            if (!done) {
                printf("'\%s\' can NOT be parsed!\n", cmd);
            }

            free(cmd);
        }
    }
}

int main(int argc, char *argv[])
{
    if( argc >= 2 )
    {
        DIR* dir = opendir(argv[1]);
        
        if( dir != NULL )
        {
            closedir(dir);
            
            Run(argv[1]);
        }
    } 
    
    printf("Can not launch File Server, need a valid directory as root.\n");

    return 0;
}