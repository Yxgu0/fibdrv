#include <stdlib.h>

#include "bn_pool.h"

void *mp_malloc(uint32_t size)
{
    if (mp.free_list) {
        struct list_head *head =
            &mp.free_list->link;  // indirect pointer to list head
        if (!list_empty(head)) {
            mp.free_list = list_entry(head->next, _bn_node, link);
            list_del_init(head);
        } else {
            mp.free_list = NULL;
        }

        return (void *) list_entry(head, _bn_node, link);
    }

    return (void *) malloc(size);
}

void mp_free(void *src)
{
    struct list_head *node = &((_bn_node *) src)->link;

    if (!mp.free_list) {
        list_del_init(node);
        mp.free_list = list_entry(node, _bn_node, link);
    } else {
        list_del_init(node);
        list_add_tail(node, &mp.free_list->link);
    }
}

void mp_del()
{
    if (!mp.free_list)
        return;

    _bn_node *head_node = mp.free_list, *node = NULL;
    list_for_each_entry (node, &head_node->link, link) {
        free(node);
    }
    free(head_node);

    mp.free_list = NULL;
}