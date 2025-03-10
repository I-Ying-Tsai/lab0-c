#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head) {
        return NULL;
    }
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head) {
        return;
    }

    struct list_head *current, *tmp;
    list_for_each_safe (current, tmp, head) {
        element_t *node = list_entry(current, element_t, list);
        list_del(current);
        free(node->value);
        free(node);
    }
    free(head);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node) {
        return false;
    }

    new_node->value = strdup(s);
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    list_add(&new_node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head || !s) {
        return false;
    }

    element_t *new_node = malloc(sizeof(element_t));
    if (!new_node) {
        return false;
    }
    new_node->value = strdup(s);
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    INIT_LIST_HEAD(&new_node->list);
    list_add_tail(&new_node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    struct list_head *first = head->next;
    element_t *node = list_entry(first, element_t, list);

    list_del(first);

    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return node;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    struct list_head *last = head->prev;
    element_t *node = list_entry(last, element_t, list);

    list_del(last);

    if (sp) {
        strncpy(sp, node->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *slow = head->next;
    struct list_head *fast = head->next;

    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    element_t *node = list_entry(slow, element_t, list);

    list_del(&node->list);

    free(node->value);
    free(node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    struct list_head *cur, *next;
    bool deleted = false;

    list_for_each_safe (cur, next, head) {
        element_t *node = list_entry(cur, element_t, list);

        if (next != head) {
            const element_t *next_node = list_entry(next, element_t, list);

            if (strcmp(node->value, next_node->value) == 0) {
                deleted = true;
                const char *dup_value = node->value;
                struct list_head *tmp;

                while (next != head &&
                       strcmp(list_entry(next, element_t, list)->value,
                              dup_value) == 0) {
                    tmp = next->next;
                    list_del(next);
                    element_t *del_node = list_entry(next, element_t, list);
                    free(del_node->value);
                    free(del_node);
                    next = tmp;
                }

                tmp = cur->next;
                list_del(cur);
                free(node->value);
                free(node);
                cur = tmp;
            }
        }
    }
    return deleted;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head || list_empty(head) || head->next == head->prev) {
        return;
    }

    struct list_head *cur = head->next;

    while (cur != head && cur->next != head) {
        struct list_head *first = cur;
        struct list_head *second = cur->next;

        list_move(first, second);

        cur = first->next;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || head->next == head->prev) {
        return;
    }

    struct list_head *cur = head, *tmp;

    do {
        tmp = cur->next;
        cur->next = cur->prev;
        cur->prev = tmp;
        cur = tmp;
    } while (cur != head);
    return;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k <= 1)
        return;

    struct list_head *cur, *group_start, *next;
    int count = 0;

    list_for_each (cur, head)
        count++;

    if (count < k)
        return;

    cur = head->next;

    while (count >= k) {
        group_start = cur;
        struct list_head *prev_tail = group_start->prev;

        for (int i = 0; i < k; i++) {
            next = cur->next;
            list_move(cur, prev_tail);
            cur = next;
        }

        group_start->next = cur;
        if (cur)
            cur->prev = group_start;

        count -= k;
    }
}

/* Sort elements of queue in ascending/descending order */
static struct list_head *merge(struct list_head *l1,
                               struct list_head *l2,
                               bool descend)
{
    struct list_head dummy;
    struct list_head *tail = &dummy;

    while (l1 && l2) {
        const element_t *e1 = list_entry(l1, element_t, list);
        const element_t *e2 = list_entry(l2, element_t, list);

        bool condition = descend ? (strcmp(e1->value, e2->value) < 0)
                                 : (strcmp(e1->value, e2->value) > 0);

        if (!condition || strcmp(e1->value, e2->value) == 0) {
            tail->next = l1;
            l1 = l1->next;
        } else {
            tail->next = l2;
            l2 = l2->next;
        }

        tail->next->prev = tail;
        tail = tail->next;
    }

    tail->next = l1 ? l1 : l2;
    if (tail->next)
        tail->next->prev = tail;

    return dummy.next;
}
static struct list_head *merge_sort(struct list_head *head, bool descend)
{
    if (!head || !head->next)
        return head;

    struct list_head *slow = head, *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    struct list_head *mid = slow->next;
    slow->next = NULL;

    struct list_head *left = merge_sort(head, descend);
    struct list_head *right = merge_sort(mid, descend);

    return merge(left, right, descend);
}

void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || head->next == head)
        return;

    head->prev->next = NULL;
    struct list_head *sorted = merge_sort(head->next, descend);

    if (!sorted) {
        head->next = head;
        head->prev = head;
        return;
    }

    struct list_head *cur = sorted, *prev = head;
    while (cur) {
        cur->prev = prev;
        prev->next = cur;
        prev = cur;
        cur = cur->next;
    }

    prev->next = head;
    head->prev = prev;
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *cur = head->prev, *prev;
    const element_t *min_elem = list_entry(cur, element_t, list);
    int count = 1;

    while (cur != head) {
        prev = cur->prev;
        element_t *e = list_entry(cur, element_t, list);

        if (strcmp(e->value, min_elem->value) > 0) {
            list_del(cur);
            free(e->value);
            free(e);
        } else {
            min_elem = e;
            count++;
        }

        cur = prev;
    }

    head->prev = min_elem->list.prev;
    head->prev->next = head;

    return count;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *cur = head->prev;
    struct list_head *prev = cur->prev;
    const element_t *max_elem = list_entry(cur, element_t, list);
    int count = 1;

    while (prev != head) {
        element_t *e = list_entry(prev, element_t, list);
        struct list_head *tmp = prev->prev;

        if (strcmp(e->value, max_elem->value) < 0) {
            list_del(prev);
            free(e->value);
            free(e);
        } else {
            max_elem = e;
            count++;
        }

        prev = tmp;
    }

    return count;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    struct list_head *cur, *next;
    queue_contex_t *main_ctx = list_entry(head->next, queue_contex_t, chain);
    if (!main_ctx->q)
        return 0;

    int total_size = main_ctx->size;

    list_for_each_safe (cur, next, head) {
        queue_contex_t *ctx = list_entry(cur, queue_contex_t, chain);
        if (ctx == main_ctx || !ctx->q || list_empty(ctx->q))
            continue;

        struct list_head *sub_queue = ctx->q;
        while (!list_empty(sub_queue)) {
            struct list_head *node = sub_queue->next;
            list_del(node);
            list_add_tail(node, main_ctx->q);
        }

        INIT_LIST_HEAD(ctx->q);
        total_size += ctx->size;
        ctx->size = 0;
    }

    main_ctx->q->prev->next = main_ctx->q;
    main_ctx->q->next->prev = main_ctx->q;

    q_sort(main_ctx->q, descend);

    main_ctx->size = total_size;
    return main_ctx->size;
}
