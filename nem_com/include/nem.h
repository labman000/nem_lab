//
// Created by zhou on 2023/12/7.
//

#ifndef NEM_LAB_NEM_H
#define NEM_LAB_NEM_H


typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;



#define NEM_COM_OK 0
#define NEM_COM_ERR -1

typedef enum {
    nem_com_mod_self = 0,
    nem_com_mod_send,
    nem_com_mod_recv,
    nem_com_num
};

typedef int (*recv_msg_f)(void *msg, size_t size);

int nem_com_register_moudle(int id,
                            char *ip,
                            int port,
                            recv_msg_f call,
                            u_char type);

int nem_com_send_msg(u_int mod_id,
                     u_int dest_id,
                     void *msg,
                     u_short type,
                     u_short length
        );


/*int nem_com_send(struct nem_com_msg *msg);*/

int nem_com_init();

#endif //NEM_LAB_NEM_H
