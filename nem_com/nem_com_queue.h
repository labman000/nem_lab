//
// Created by zhou on 2023/12/11.
//

#ifndef NEM_LAB_NEM_COM_QUEUE_H
#define NEM_LAB_NEM_COM_QUEUE_H

typedef struct nem_com_queue nem_com_queue_t;

struct nem_com_queue
{
    nem_com_queue_t *prev;
    nem_com_queue_t *next;
}nem_com_queue_s;

#define nem_com_queue_init(q)           \
        (q)->prev = q;                   \
        (q)->next = q;


#define nem_com_queue_empty(h)      \
        (h == (h)->prev)


#define nem_com_queue_insert_head(h, x) \
        (x)->next = (h)->next;         \
         (x)->next->prev = (x)         \
        (x)->prev = (h);                \
        ((h))->next = (x);


#define nem_com_queue_insert_tail(h,x)  \
        (x)->prev = (h)->prev;          \
        (x)->next = (h);                \
        (x)->prev->next = (x);          \
        (h)->prev = (x);

#define nem_com_queue_head(h)   \
        (h)->next

#define nem_com_queue_tail(h)   \
        (h)->prev


#define nem_com_queue_remove(x) \
        (x)->prev->next = (x)->next;     \
        (x)->next->prev = (x)->prev     \

#define nem_com_queue_data(q, type, link) \
        (type *) ((u_char *)q - offsetof(type, link))


#endif //NEM_LAB_NEM_COM_QUEUE_H
