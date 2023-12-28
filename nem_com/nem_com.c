//
// Created by zhou on 2023/12/7.
//

#include "nem_com.h"
#include "nem_com_sock.h"


int nem_send_msg()
{

}

int nem_com_send(struct nem_com_msg *msg)
{
    if(msg == NULL)
    {
        return -1;
    }
    if(msg->msg == NULL)
    {
        return -1;
    }

    return 0;

}


static struct nem_module glob_reg_mod[NEM_MOD_NUM_MAX] = {
        NEM_MOD_INIT(nem_com_mod_self,"self",8010),
        NEM_MOD_INIT(nem_com_mod_send,"send",8009),
        NEM_MOD_INIT(nem_com_mod_recv,"recv",8010),

};

static struct nem_mod_ctrl glob_mod_ctrls[NEM_MOD_NUM_MAX];

int nem_com_init()
{
    int i, ret;
    for (int i = 0; i < NEM_MOD_NUM_MAX; ++i) {
        glob_mod_ctrls[i].status = NEM_MOD_STATUS_INIT;
    }
}

u_int nem_gen_msg_id(u_int mod_id)
{
    u_int id;
    struct nem_mod_ctrl *ctrl = &glob_mod_ctrls[mod_id];
    if(ctrl == NULL)
    {
        return 0;
    }
    id = ctrl->msg_id;
    ctrl->msg_id++;
    if(ctrl->msg_id > NEM_MOD_MSG_ID_MAX)
    {
        ctrl->msg_id = 0;
    }
    return id;
}

int nem_com_register_moudle(int id,
                            char *ip,
                            int port,
                            recv_msg_f call,
                            u_char type)
{
    struct nem_mod_ctrl *ctrl;
    struct nem_module *mod;
    int ret;
    if(id > NEM_MOD_NUM_MAX)
    {
        return NEM_COM_ERR;
    }
    ctrl = &glob_mod_ctrls[id];
    if(ctrl->status != NEM_MOD_STATUS_INIT)
    {
        return NEM_COM_ERR;
    }
    ctrl->status = NEM_MOD_STATUS_REGISTERED;
    ctrl->msg_id = 0;
    ctrl->mod.id = id;
    ctrl->mod.ip = ip;
    ctrl->mod.port = port;
    mod = &glob_reg_mod[id];
    if (mod) {
        if (mod->ip) {
            ctrl->mod.ip = mod->ip;
        }
        ctrl->mod.port = mod->port;
    }

    ctrl->recv_ctrl.recv_msg = call;

    ret = nem_com_create_recv_agent_server(ctrl->mod.ip, ctrl->mod.port, &ctrl->recv_ctrl);
    if(ret != NEM_COM_OK)
    {
        return NEM_COM_ERR;
    }


}


int nem_com_send_msg(u_int mod_id,
                     u_int dest_id,
                     void *msg,
                     u_short type,
                     u_short length
)
{
    struct nem_com_msg *head;
    void *p;
    size_t size;
    int ret;

    if(msg == NULL){
        return NEM_COM_ERR;
    }
    if(length <= 0){
        return NEM_COM_ERR;
    }
    if(!nem_com_msg_align_4b(length)){
        return NEM_COM_ERR;
    }

    struct nem_mod_ctrl *ctrl = &glob_mod_ctrls[mod_id];
    if(ctrl == NULL)
    {
        return NEM_COM_ERR;
    }
    size = sizeof (struct nem_com_msg);
    size += length;
    head = (struct nem_com_msg *) nem_alloc(size);
    if(head == NULL){
        return NEM_COM_ERR;
    }
    nem_com_msg_set_magic(head);
    head->id = nem_gen_msg_id(mod_id);
    nem_com_msg_set_header(head,mod_id,dest_id,msg,length);
    p = (void *) (head + 1);
    nem_memcpy(p, msg, length);




}