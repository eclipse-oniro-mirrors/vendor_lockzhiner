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
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "los_task.h"
#include "lz_hardware.h"
#include "config_network.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/stats.h"
#include "lwip/inet_chksum.h"

/* 任务的堆栈大小 */
#define TASK_STACK_SIZE     20480
/* 任务的优先级 */
#define TASK_PRIO           24

/* 循环等待时间 */
#define GET_WIFI_WAIT_MSEC      1000
#define SERVER_WAIT_MSEC        2000
#define CLIENT_WAIT_MSEC        100

/* 打印信息 */
#define LOG_TAG             "udp"

/* UDP服务端IP地址和端口号 */
#define OC_SERVER_IP        "192.168.2.49"
#define SERVER_PORT         6666
/* UDP客户端端口号 */
#define CLIENT_LOCAL_PORT   8888

/* 字节缓冲区大小 */
#define BUFF_LEN            256

WifiLinkedInfo wifiinfo;

int udp_get_wifi_info(WifiLinkedInfo *info)
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
                    goto connect_done;
                }
            }
        }
        LOS_Msleep(GET_WIFI_WAIT_MSEC);
        retry--;
    }

connect_done:
    return ret;
}


void udp_server_msg_handle(int fd)
{
    char buf[BUFF_LEN];  //接收缓冲区
    socklen_t len;
    int cnt = 0, count;
    struct sockaddr_in client_addr = {0};

    while (1) {
        memset(buf, 0, BUFF_LEN);
        len = sizeof(client_addr);
    
        printf("[udp server]------------------------------------------------\n");
        printf("[udp server] waitting client message!!!\n");
        
        /* recvfrom是阻塞函数，没有数据就一直阻塞 */
        count = recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&client_addr, &len);
        if (count == -1) {
            printf("[udp server] recieve data fail!\n");
            LOS_Msleep(SERVER_WAIT_MSEC);
            break;
        }
        
        printf("[udp server] remote addr:%s port:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("[udp server] rev:%s\n", buf);
        memset(buf, 0, BUFF_LEN);
        sprintf(buf, "I have recieved %d bytes data! recieved cnt:%d", count, ++cnt);
        printf("[udp server] send:%s\n", buf);
        sendto(fd, buf, strlen(buf), 0, (struct sockaddr*)&client_addr, len);   // 发送信息给client
    }
    lwip_close(fd);
}

int wifi_udp_server(void* arg)
{
    int server_fd, ret;
    struct in_addr addr;

    while (1) {
        server_fd = socket(AF_INET, SOCK_DGRAM, 0); // AF_INET:IPV4;SOCK_DGRAM:UDP
        if (server_fd < 0) {
            printf("create socket fail!\n");
            return -1;
        }

        /*
         * 设置调用close(socket)后,仍可继续重用该socket。
         * 调用close(socket)一般不会立即关闭socket，而经历TIME_WAIT的过程。
         */
        int flag = 1;
        
        ret = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));
        if (ret != 0) {
            printf("[CommInitUdpServer]setsockopt fail, ret[%d]!\n", ret);
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
        
        addr.s_addr = (in_addr_t)wifiinfo.ipAddress;
        printf("[udp server] local  addr:%s,port:%u\n", inet_ntoa(addr), ntohs(serv_addr.sin_port));

        /* 处理接收到的数据 */
        udp_server_msg_handle(server_fd);
        LOS_Msleep(SERVER_WAIT_MSEC);
    }
}



void udp_client_msg_handle(int fd, struct sockaddr* dst)
{
    socklen_t len = sizeof(*dst);

    struct sockaddr_in client_addr;
    int cnt = 0, count = 0;

    printf("[udp client] remote addr:%s port:%u\n", inet_ntoa(((struct sockaddr_in*)dst)->sin_addr),
        ntohs(((struct sockaddr_in*)dst)->sin_port));
    connect(fd, dst, len);
    getsockname(fd, (struct sockaddr*)&client_addr, &len);
    printf("[udp client] local  addr:%s port:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    while (1) {
        char buf[BUFF_LEN];
        
        printf("[udp client]------------------------------------------------\n");
        printf("[udp client] waitting server message!!!\n");

        /*
         * recv接收来server的消息
         * recvfrom是阻塞函数，没有数据就一直阻塞
         */
        count = recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*)&client_addr, &len);
        if (count == -1) {
            printf("[udp client] No server message!!!\n");
        } else {
            printf("[udp client] remote addr:%s port:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            printf("[udp client] rev:%s\n", buf);
        }
        memset(buf, 0, BUFF_LEN);
        sprintf(buf, "UDP TEST cilent send:%d", ++cnt);
        /* 发送数据给server */
        count = sendto(fd, buf, strlen(buf), 0, (struct sockaddr*)&client_addr, len);
        printf("[udp client] send:%s\n", buf);
        printf("[udp client] client sendto msg to server %dbyte,waitting server respond msg!!!\n", count);

        LOS_Msleep(CLIENT_WAIT_MSEC);
    }
    lwip_close(fd);
}


int wifi_udp_client(void* arg)
{
    int client_fd, ret;
    struct sockaddr_in serv_addr, local_addr;

    while (1) {
        client_fd = socket(AF_INET, SOCK_DGRAM, 0); // AF_INET:IPV4;SOCK_DGRAM:UDP
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
            printf("[CommInitUdpServer]setsockopt fail, ret[%d]!\n", ret);
            close(client_fd);
            continue;
        }

        memset(&local_addr, 0, sizeof(local_addr));
        local_addr.sin_family = AF_INET;
        local_addr.sin_addr.s_addr = wifiinfo.ipAddress;
        local_addr.sin_port = htons(CLIENT_LOCAL_PORT);
        /* 绑定本地ip端口号 */
        ret = bind(client_fd, (struct sockaddr*)&local_addr, sizeof(local_addr));
        if (ret == -1) {
            printf("client bind failed(%d)\n", ret);
            close(client_fd);
            continue;
        }

        memset(&serv_addr, 0, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        /* IP地址，需要进行网络序转换，INADDR_ANY：本地地址 */
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        serv_addr.sin_port = htons(SERVER_PORT);
        udp_client_msg_handle(client_fd, (struct sockaddr*)&serv_addr);

        LOS_Msleep(CLIENT_WAIT_MSEC);
    }

    return 0;
}


void wifi_udp_process(void *args)
{
    unsigned int threadID_client, threadID_server;
    unsigned int ret = LOS_OK;

    WifiLinkedInfo info;

    /* 开启WiFi连接任务 */
    ExternalTaskConfigNetwork();
    while (udp_get_wifi_info(&info) != 0) {
        /* 等待，不做任何事情 */
    }

    wifiinfo = info;
    LOS_Msleep(WAIT_MSEC);

    CreateThread(&threadID_client,  wifi_udp_client, NULL, "udp client@ process");
    CreateThread(&threadID_server,  wifi_udp_server, NULL, "udp server@ process");
}


void wifi_udp_example(void)
{
    unsigned int ret = LOS_OK;
    unsigned int thread_id;
    TSK_INIT_PARAM_S task = {0};

    task.pfnTaskEntry = (TSK_ENTRY_FUNC)wifi_udp_process;
    task.uwStackSize = TASK_STACK_SIZE;
    task.pcName = "wifi_process";
    task.usTaskPrio = TASK_PRIO;
    ret = LOS_TaskCreate(&thread_id, &task);
    if (ret != LOS_OK) {
        printf("Falied to create wifi_process ret:0x%x\n", ret);
        return;
    }
}


APP_FEATURE_INIT(wifi_udp_example);
