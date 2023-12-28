//
// Created by zhou on 2023/12/7.
//

#ifndef NEM_LAB_NEM_COM_H
#define NEM_LAB_NEM_COM_H

#include <pthread.h>
#include <netinet/in.h>
#include <stddef.h>
#include <malloc.h>
#include <string.h>
#include "nem.h"
#include "nem_com_queue.h"


#define NEM_MAGIC_NUMBER_H 0xcdef   //魔术头
#define NEM_MAGIC_NUMBER_T 0x1234   //魔术尾

#define nem_alloc       malloc
#define nem_free        free
#define nem_memcpy(dst, src, n)         (void) memcpy(dst, src, n)

#define nem_com_msg_set_magic(msg)              \
        (msg)->magic_h = NEM_MAGIC_NUMBER_H;     \
        (msg)->magic_t = NEM_MAGIC_NUMBER_T

#define nem_com_msg_magic_legal(msg) \
        ((msg)->magic_h == NEM_MAGIC_NUMBER_H && (msg)->magic_t == NEM_MAGIC_NUMBER_T)

#define nem_com_msg_align_4b(len) \
        ((len)%4 == 0)

#define nem_com_msg_set_header(msg, _mod_id, _dst_id, _msg_typ, _msg_len) \
         (msg)->mod_id = (_mod_id);                                        \
         (msg)->dst_id = (_dst_id);                                        \
         (msg)->attr = 0;                                                 \
         (msg)->prior = 0;                                                \
         (msg)->typ = (_msg_typ);                                          \
         (msg)->len = (_msg_len)


typedef struct nem_com_msg
{
    u_long magic_h;
    u_int id;
    u_int mod_id;
    u_int dst_id;
    u_short prior;
    u_short attr;
    u_int typ;
    u_short len;
    void * msg;
    u_long magic_t;

};

typedef struct nem_com_msg_node
{
    struct nem_com_queue queue;
    struct nem_com_msg *msg;
    long time_ms;

};
#define NEM_MOD_STATUS_INIT         0
#define NEM_MOD_STATUS_REGISTERED   1
#define NEM_MOD_STATUS_EXCEPT       2

#define NEM_MOD_MSG_ID_MAX ((u_int) -1)

#define NEM_MOD_NAM_LEN 16
#define NEM_MOD_NUM_MAX 8
typedef struct nem_module
{
    int id;
    char *ip;
    int port;
    char name[NEM_MOD_NAM_LEN];
};

#define NEM_LOOPBACK_IP ((char *) "127.0.0.1")

#define NEM_MOD_INIT(_id, _name, _port) \
        {                              \
            (_id),                     \
            (NEM_LOOPBACK_IP),          \
            (_port),                    \
            (_name)                 \
        }




#define RECV_QUEUE_NUM  4

typedef struct nem_fake_recv_ctl_t
{
    struct nem_recv_ctrl * ctrl;
    int idx;
};
typedef struct nem_recv_ctrl
{
    int sockfd;
    struct nem_com_msg_node recv[RECV_QUEUE_NUM];
    pthread_mutex_t recv_mtx[RECV_QUEUE_NUM];
    pthread_t call[RECV_QUEUE_NUM];
    pthread_t thread;
    int (* recv_msg)(void *, size_t);

};

typedef struct nem_send_ctrl
{
    int sockfd;
    struct sockaddr_in sockaddrIn;
    struct nem_com_msg_node tosend;     //send queue
    pthread_mutex_t tosend_mtx;
    struct nem_com_msg_node waiting;
    pthread_mutex_t waiting_mtx;
    pthread_t thread;
    pthread_t wait;
    long wait_ms;

};


typedef struct nem_mod_ctrl
{
    struct nem_module mod;
    struct nem_recv_ctrl recv_ctrl;
    struct nem_send_ctrl send_ctrl;
    u_char status;
    u_char type;
    u_int msg_id;

}nem_mod_ctrl_s;


#endif //NEM_LAB_NEM_COM_H
