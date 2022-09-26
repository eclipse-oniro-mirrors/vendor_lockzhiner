/*
 * Copyright (c) 2022 FuZhou Lockzhiner Electronic Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "los_task.h"
#include "lz_hardware.h"
#include "config_network.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/stats.h"
#include "lwip/inet_chksum.h"

/* 任务的堆栈大小 */
#define TASK_STACK_SIZE         20480
/* 任务的优先级 */
#define TASK_PRIO               24

/* 循环等待时间 */
#define GET_WIFI_WAIT_MSEC      1000
#define SERVER_WAIT_MSEC        2000
#define CLIENT_WAIT_MSEC        3000

#define LOG_TAG                 "tcp"

/* 服务器IP地址和端口 */
#define OC_SERVER_IP            "192.168.2.156"
#define SERVER_PORT             6666
/* 服务器监听数量 */
#define SERVER_LISTEN_MAX       64

/* 缓冲区大小 */
#define BUFF_LEN                256

int get_wifi_info(WifiLinkedInfo *info)
{
    int ret = -1;
    int gw, netmask;
    memset(info, 0, sizeof(WifiLinkedInfo));
    unsigned int retry = 15;
    struct in_addr addr;

    while (retry) {
        if (GetLinkedInfo(info) == WIFI_SUCCESS) {
            if (info->connState == WIFI_CONNECTED) {
                if (info->ipAddress != 0) {
                    addr.s_addr = (in_addr_t)info->ipAddress;
                    LZ_HARDWARE_LOGD(LOG_TAG, "rknetwork IP (%s)", inet_ntoa(addr));

                    if (WIFI_SUCCESS == GetLocalWifiGw(&gw)) {
                        addr.s_addr = (in_addr_t)gw;
                        LZ_HARDWARE_LOGD(LOG_TAG, "network GW (%s)", inet_ntoa(addr));
                    }
                    if (WIFI_SUCCESS == GetLocalWifiNetmask(&netmask)) {
                        addr.s_addr = (in_addr_t)netmask;
                        LZ_HARDWARE_LOGD(LOG_TAG, "network NETMASK (%s)", inet_ntoa(addr));
                    }
                    if (WIFI_SUCCESS == SetLocalWifiGw()) {
                        LZ_HARDWARE_LOGD(LOG_TAG, "set network GW");
                    }
                    if (WIFI_SUCCESS == GetLocalWifiGw(&gw)) {
                        addr.s_addr = (in_addr_t)gw;
                        LZ_HARDWARE_LOGD(LOG_TAG, "network GW (%s)", inet_ntoa(addr));
                    }
                    if (WIFI_SUCCESS == GetLocalWifiNetmask(&netmask)) {
                        addr.s_addr = (in_addr_t)netmask;
                        LZ_HARDWARE_LOGD(LOG_TAG, "network NETMASK (%s)", inet_ntoa(addr));
                    }
                    ret = 0;
                    retry = 0;
                    continue;
                }
            }
        }
        LOS_Msleep(GET_WIFI_WAIT_MSEC);
        retry--;
    }

    return ret;
}


void tcp_server_msg_handle(int fd)
{
    char buf[BUFF_LEN];  // 接收缓冲区
    socklen_t client_addr_len;
    int cnt = 0, count;
    int client_fd;
    struct sockaddr_in client_addr = {0};

    printf("waitting for client connect...\n");
    /* 监听socket 此处会阻塞 */
    client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_len);
    printf("[tcp server] accept <%s:%d>\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    while (1) {
        memset(buf, 0, BUFF_LEN);
        printf("-------------------------------------------------------\n");
        printf("[tcp server] waitting client msg\n");
        count = recv(client_fd, buf, BUFF_LEN, 0);       // read是阻塞函数，没有数据就一直阻塞
        if (count == -1) {
            printf("[tcp server] recieve data fail!\n");
            LOS_Msleep(SERVER_WAIT_MSEC);
            break;
        }
        printf("[tcp server] rev client msg:%s\n", buf);
        memset(buf, 0, BUFF_LEN);
        snprintf(buf, sizeof(buf), "I have recieved %d bytes data! recieved cnt:%d", count, ++cnt);
        printf("[tcp server] send msg:%s\n", buf);
        send(client_fd, buf, strlen(buf), 0);           // 发送信息给client
    }
    lwip_close(client_fd);
    lwip_close(fd);
}

int wifi_server(void)
{
    int server_fd, ret;

    while (1) {
        server_fd = socket(AF_INET, SOCK_STREAM, 0);         // AF_INET:IPV4;SOCK_STREAM:TCP
        if (server_fd < 0) {
            printf("create socket fail!\n");
            return -1;
        }

        /* 设置调用close(socket)后,仍可继续重用该socket。
         * 调用close(socket)一般不会立即关闭socket，而经历TIME_WAIT的过程。
         */
        int flag = 1;
        ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
        if (ret != 0) {
            printf("[CommInitTcpServer]setsockopt fail, ret[%d]!\n", ret);
        }

        struct sockaddr_in serv_addr = {0};
        serv_addr.sin_family = AF_INET;
        /* IP地址，需要进行网络序转换，INADDR_ANY：本地地址 */
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        /* 端口号，需要网络序转换 */
        serv_addr.sin_port = htons(SERVER_PORT);
        /* 绑定服务器地址结构 */
        ret = bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
        if (ret < 0) {
            printf("socket bind fail!\n");
            lwip_close(server_fd);
            return -1;
        }
        /* 监听socket 此处不阻塞 */
        ret = listen(server_fd, SERVER_LISTEN_MAX);
        if (ret != 0) {
            printf("socket listen fail!\n");
            lwip_close(server_fd);
            return -1;
        }
        printf("[tcp server] listen:%d<%s:%d>\n", server_fd, inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
        /* 处理接收到的数据 */
        tcp_server_msg_handle(server_fd);
        LOS_Msleep(SERVER_WAIT_MSEC);
    }
}

void tcp_client_msg_handle(int fd_src, struct sockaddr* dst)
{
    int fd = fd_src;
    socklen_t len = sizeof(*dst);

    int cnt = 0, count = 0;
    while (connect(fd, dst, len) < 0) {
        printf("connect server failed...%d\n", ++count);
        lwip_close(fd);
        LOS_Msleep(CLIENT_WAIT_MSEC);
        fd = socket(AF_INET, SOCK_STREAM, 0); // AF_INET:IPV4;SOCK_STREAM:TCP
    }

    while (1) {
        char buf[BUFF_LEN];
        
        snprintf(buf, sizeof(buf), "TCP TEST cilent send:%d", ++cnt);
        count = send(fd, buf, strlen(buf), 0);                          // 发送数据给server
        // count = lwip_write(fd, buf, strlen(buf));                    // 发送数据给server
        printf("------------------------------------------------------------\n");
        printf("[tcp client] send:%s\n", buf);
        printf("[tcp client] client sendto msg to server %d,waitting server respond msg!!!\n", count);
        memset(buf, 0, BUFF_LEN);
        count = recv(fd, buf, BUFF_LEN, 0);                             // 接收来自server的信息
        if (count == -1) {
            printf("[tcp client] recieve data fail!\n");
            LOS_Msleep(CLIENT_WAIT_MSEC);
            break;
        }
        printf("[tcp client] rev:%s\n", buf);
    }
    lwip_close(fd);
}

int wifi_client(void)
{
    int client_fd, ret;
    struct sockaddr_in serv_addr;

    while (1) {
        client_fd = socket(AF_INET, SOCK_STREAM, 0);    // AF_INET:IPV4;SOCK_STREAM:TCP
        if (client_fd < 0) {
            printf("create socket fail!\n");
            return -1;
        }

        /*
         * 设置调用close(socket)后,仍可继续重用该socket。
         * 调用close(socket)一般不会立即关闭socket，而经历TIME_WAIT的过程。
         */
        int flag = 1;
        ret = setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
        if (ret != 0) {
            printf("[CommInitTcpServer]setsockopt fail, ret[%d]!\n", ret);
        }

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = inet_addr(OC_SERVER_IP);
        serv_addr.sin_port = htons(SERVER_PORT);
        printf("[tcp client] connect:%d<%s:%d>\n", client_fd, inet_ntoa(serv_addr.sin_addr),
            ntohs(serv_addr.sin_port));

        tcp_client_msg_handle(client_fd, (struct sockaddr*)&serv_addr);

        LOS_Msleep(CLIENT_WAIT_MSEC);
    }

    return 0;
}


void wifi_process(void)
{
    unsigned int threadID_client, threadID_server;
    unsigned int ret = LOS_OK;

    WifiLinkedInfo info;

    /* 开启WiFi连接任务 */
    ExternalTaskConfigNetwork();
    while (get_wifi_info(&info) != 0) {
        /* 什么都不做，等待 */
    }

    CreateThread(&threadID_client,  wifi_client, NULL, "client@ process");
    CreateThread(&threadID_server,  wifi_server, NULL, "server@ process");
}


void wifi_tcp_example(void)
{
    unsigned int ret = LOS_OK;
    unsigned int thread_id;
    TSK_INIT_PARAM_S task = {0};

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)wifi_process;
    task.uwStackSize = TASK_STACK_SIZE;
    task.pcName = "wifi_process";
    task.usTaskPrio = TASK_PRIO;
    ret = LOS_TaskCreate(&thread_id, &task);
    if (ret != LOS_OK) {
        printf("Falied to create wifi_process ret:0x%x\n", ret);
        return;
    }
}

APP_FEATURE_INIT(wifi_tcp_example);
