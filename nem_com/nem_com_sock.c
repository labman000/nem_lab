//
// Created by zhou on 2023/12/14.
//

//#include <malloc.h>
#include <stdlib.h>
//#include <printf.h>
#include "nem_com.h"
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "nem_com_sock.h"


void * nem_com_thread_queue_consumer(void * p_ctrl)
{
    struct nem_fake_recv_ctl_t *f_ctrl = (struct nem_fake_recv_ctl_t *)p_ctrl;
    int idx = f_ctrl->idx;
    struct nem_recv_ctrl *ctrl = f_ctrl->ctrl;
    struct nem_com_queue *head;
    struct nem_com_msg_node * node;
    struct nem_com_msg  *msg;
    size_t siz;
    int ret;
    nem_free(p_ctrl);
    p_ctrl = NULL;
    pthread_detach(pthread_self());

    while(1)
    {
        if(!nem_com_queue_empty(&ctrl->recv[idx].queue))
        {
            (void) pthread_mutex_lock(&ctrl->recv_mtx[idx]);
            head = nem_com_queue_head(&ctrl->recv[idx].queue);
            nem_com_queue_remove(head);
            (void ) pthread_mutex_unlock(&ctrl->recv_mtx[idx]);


            node = nem_com_queue_data(head, struct nem_com_msg_node,queue);
            (struct nem_com_msg_node *) ((u_char *)head - offsetof( struct nem_com_msg_node, queue));
            msg = node->msg;
            nem_free(node);

            siz = sizeof (struct nem_com_msg) + msg->len;
            if(ctrl->recv_msg != NULL)
            {
                ret = ctrl->recv_msg(msg, siz);
            }
            nem_free(msg);
        }
    }
    pthread_exit(NULL);
    return NULL;
}

void * nem_com_thread_recvfrom(void * p_ctrl)
{
    struct nem_recv_ctrl *ctrl = p_ctrl;
    int ret;
    void * buf;
    int fd = ctrl->sockfd;
    struct sockaddr_in cli;
    socklen_t len = sizeof (cli);
    size_t siz;
    int idx;

    struct nem_com_msg *msg;
    struct nem_com_msg_node * node;


    pthread_detach(pthread_self());
    buf = nem_alloc(NEM_COM_RECV_BUF_LEN_MAX);
    if(buf == NULL)
    {
        return NULL;
    }

    printf("thread recv start@sock:%d...\n",fd);

    while (1)
    {
        ret = recvfrom(fd,buf,NEM_COM_RECV_BUF_LEN_MAX,0,&cli, len);
        if(ret > 0)
        {
            msg = (struct nem_com_msg*)buf;
            if(!nem_com_msg_magic_legal(msg))
            {
                continue;
            }
            if (!nem_com_msg_align_4b(msg->len))
            {
                continue;
            }
            //整个消息大小
            siz = sizeof (struct nem_com_msg) + msg->len;
            if((int) siz > ret)
            {
                continue;
            }
            node = nem_alloc(sizeof (struct nem_com_msg_node));
            if(node != NULL)
            {
                node->msg = nem_alloc(siz);
                if(node->msg != NULL)
                {
                    nem_memcpy(node->msg, buf, siz);
                    idx = node->msg->id % RECV_QUEUE_NUM;
                    (void) pthread_mutex_lock(&ctrl->recv_mtx[idx]);
                    nem_com_queue_insert_tail(&ctrl->recv[idx].queue,&node->queue);
                    (void ) pthread_mutex_unlock(&ctrl->recv_mtx[idx]);


                }
                else {
                    printf("nem_alloc failed\n");
                }
            }
        }
        usleep(1000);
    }
    nem_free(buf);
    return NULL;

}

int nem_com_create_thread(pthread_t *t,
                          void* (*call)(void* p),
                          void *arg)
{
    int ret = pthread_create(t,NULL,call,arg);
    if(ret == 0)
    {
        return NEM_COM_OK;
    } else
    {
        return NEM_COM_ERR;
    }
}

int nem_com_create_recv_agent_server(char *ip,
                                     int port,
                                     struct nem_recv_ctrl *ctrl)
{
    int sockfd, ret;
    struct sockaddr_in addr;
    pthread_t thread, call;
    sockfd = socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd == -1)
    {
        return NEM_COM_ERR;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    ret = bind(sockfd,(struct sockaddr *)&addr, sizeof addr);
    if (ret < 0)
    {
        return NEM_COM_ERR;
    }

    for (int idx = 0; idx < RECV_QUEUE_NUM; ++idx) {
        nem_com_queue_init(&ctrl->recv[idx].queue);
        pthread_mutex_init(&ctrl->recv_mtx[idx],NULL);
    }
    ctrl->sockfd = sockfd;

    ret = nem_com_create_thread(&thread,
                                &nem_com_thread_recvfrom,
                                (void *) ctrl);
    ctrl->thread = thread;

    //起消费线程
    for (int idx = 0; idx < RECV_QUEUE_NUM; ++idx) {
        struct nem_fake_recv_ctl_t * f_ctrl = nem_alloc(sizeof (struct nem_fake_recv_ctl_t));
        f_ctrl->ctrl = ctrl;
        f_ctrl->idx = idx;
        ret = nem_com_create_thread(&call, &nem_com_thread_queue_consumer,f_ctrl);
        if(ret != NEM_COM_OK)
        {
            printf("thread create failed\n");
            continue;
        }
        ctrl->call[idx] = call;

    }

    return NEM_COM_OK;

}


int nem_com_create_send_agent_client(char *ip,
                                     int port,
                                     struct nem_send_ctrl *ctrl)
{
    int sockfd;
    int ret;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd == -1);
    {
        return NEM_COM_ERR;
    }
    ctrl->sockfd = sockfd;
    ctrl->sockaddrIn.sin_addr.s_addr = inet_addr(ip);
    ctrl->sockaddrIn.sin_family = AF_INET;
    ctrl->sockaddrIn.sin_port = htons(port);
    nem_com_queue_init(&ctrl->tosend.queue);
    pthread_mutex_init(&ctrl->tosend_mtx,NULL);
    nem_com_queue_init(&ctrl->waiting.queue);
    pthread_mutex_init(&ctrl->waiting_mtx,NULL);
    return NEM_COM_OK;
}