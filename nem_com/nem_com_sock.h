//
// Created by zhou on 2023/12/14.
//

#ifndef NEM_LAB_NEM_COM_SOCK_H
#define NEM_LAB_NEM_COM_SOCK_H



#define NEM_COM_RECV_BUF_LEN_MAX    6144    //6K 6*1024

int nem_com_create_recv_agent_server(char *ip,
                                     int port,
                                     struct nem_recv_ctrl *ctrl);


#endif //NEM_LAB_NEM_COM_SOCK_H
