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
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *cur = head->next;
    bool deleted = false;

    while (cur != head || cur->next != head) {
        // compare between cur->value and next_cur->value7
        const element_t *node = list_entry(cur, element_t, list);
        const element_t *next_node = list_entry(cur->next, element_t, list);

        if (strcmp(node->value, next_node->value) == 0) {
            deleted = true;
            const char *dup_value = node->value;
            struct list_head *tmp;

            while (cur != head &&
                   strcmp(list_entry(cur, element_t, list)->value, dup_value)) {
                tmp = cur->next;
                list_del(cur);
                element_t *del_node = list_entry(cur, element_t, list);
                free(del_node->value);
                free(del_node);
                cur = tmp;
            }
        } else {
            cur = cur->next;
        }
    }

    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
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

    struct list_head *cur = head->prev;

    while (cur != head) {
        struct list_head *prev = cur->prev;
        list_move(cur, head);
        cur = prev;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k <= 1)
        return;

    struct list_head *cur, *next;
    int count = 0;

    list_for_each (cur, head)
        count++;

    if (count < k)
        return;

    cur = head->next;

    while (count >= k) {
        struct list_head *group_tail = cur;
        int i = 0;

        list_for_each_safe (cur, next, head) {
            if (++i == k)
                break;
            list_move_tail(next, head);
        }
        cur = group_tail->next;
        count -= k;
    }
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
