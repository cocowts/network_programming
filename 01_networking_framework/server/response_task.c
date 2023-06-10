#include "response_task.h"

#include "local_service.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#include "udp_point.h"
#include "msg_def.h"

#define DESC_SIZE 64
#define ADDR_SIZE 16
#define USAGE_SIZE 256

int get_localip(const char * eth_name, char *local_ip_addr)
{
	int ret = -1;
    register int fd, intrface;
    struct ifreq ifr[32];
    struct ifconf ifc;
 
    if (local_ip_addr == NULL || eth_name == NULL)
	{
        return ret;
	}
	if ((fd=socket(AF_INET, SOCK_DGRAM, 0)) > 0)
	{
		ifc.ifc_len = sizeof ifr;
		ifc.ifc_buf = (caddr_t)ifr;
		if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc))
		{
			intrface = ifc.ifc_len / sizeof(struct ifreq);
			while (intrface-- > 0)
			{
				if (!(ioctl(fd, SIOCGIFADDR, (char*)&ifr[intrface])))
				{
					if(strcmp(eth_name, ifr[intrface].ifr_name) == 0)
					{
						ret = 0;
						sprintf(local_ip_addr, "%s", inet_ntoa(((struct sockaddr_in*)(&ifr[intrface].ifr_addr))->sin_addr));
						break;
					}
				}
			}
		}
	}
	if (fd > 0)
	{
		close(fd);
	}
    return ret;
}

static void Query_Handler(UdpPoint *udp, Message *msg, const char *remote, int port)
{
    Message *resp = Message_New(TYPE_RESPONSE, 0, 0, 0, NULL, DESC_SIZE + ADDR_SIZE + USAGE_SIZE);

    if (resp) {
        char ip[16] = {0};
        get_localip("ens33", ip);

        strncpy(resp->payload, Service_GetDesc(), DESC_SIZE);
        strncpy(resp->payload + DESC_SIZE, ip, ADDR_SIZE);
        strncpy(resp->payload + DESC_SIZE + ADDR_SIZE, Service_GetUsage(), USAGE_SIZE);

        UdpPoint_SendMsg(udp, resp, remote, port);
    }

    free(resp);
}

void* Response_Task(void* arg)
{
    UdpPoint *udp = UdpPoint_New(9999);

    printf("%s : enter response task ...\n", __FUNCTION__);

    if (udp) {
        char remote[16] = {0};
        int port = 0;

        printf("%s : udp = %p\n", __FUNCTION__, udp);

        while (1) {
            Message *msg = UdpPoint_RecvMsg(udp, remote, &port);

            if (msg && msg->type == TYPE_QUERY) {
                printf("%s : msg = %p\n", __FUNCTION__, msg);
                printf("%s : remote = %s\n", __FUNCTION__, remote);
                printf("%s : port = %d\n", __FUNCTION__, port);
                printf("%s : msg->type = %d\n", __FUNCTION__, msg->type);
                printf("%s : msg->cmd = %d\n", __FUNCTION__, msg->cmd);

                Query_Handler(udp, msg, remote, port);
            } else {
                printf("%s : msg is NULL ...\n", __FUNCTION__);
            }

            free(msg);
        }
    }

    return arg;
}