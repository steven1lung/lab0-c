#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */

struct list_head *list_get_mid(struct list_head *head);
void merge(struct list_head *head, struct list_head *second);
void list_free_node(struct list_head *li);


struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    struct list_head *li, *safe;
    list_for_each_safe (li, safe, l)
        list_free_node(li);
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    // construct new element
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = malloc(strlen(s) + 1);
    if (!new->value) {
        free(new);
        return false;
    }
    INIT_LIST_HEAD(&new->list);
    strncpy(new->value, s, strlen(s) + 1);

    // add element to queue head
    list_add(&new->list, head);
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    // construct new element
    element_t *new = malloc(sizeof(element_t));
    if (!new)
        return false;
    new->value = malloc(strlen(s) + 1);
    if (!new->value) {
        free(new);
        return false;
    }
    INIT_LIST_HEAD(&new->list);
    strncpy(new->value, s, strlen(s) + 1);

    // add element to queue tail
    list_add_tail(&new->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *target = list_first_entry(head, element_t, list);
    list_del_init(&target->list);
    if (sp) {
        strncpy(sp, target->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return target;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *target = list_last_entry(head, element_t, list);
    list_del_init(&target->list);
    if (sp) {
        strncpy(sp, target->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return target;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;
    int len = 0;
    struct list_head *li;
    list_for_each (li, head)
        ++len;
    return len;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;
    struct list_head *mid = list_get_mid(head);
    list_free_node(mid);

    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    struct list_head *li = head->next, *safe;
    while (li != head) {
        struct list_head *cur = li;
        if (cur->next != head &&
            !strcmp(list_entry(cur, element_t, list)->value,
                    list_entry(cur->next, element_t, list)->value)) {
            while (cur->next != head &&
                   !strcmp(list_entry(cur, element_t, list)->value,
                           list_entry(cur->next, element_t, list)->value)) {
                safe = cur->next;
                list_free_node(cur);
                cur = safe;
            }
            safe = cur->next;
            list_free_node(cur);
            cur = safe;
        }
        li = cur->next;
    }
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || q_size(head) <= 1)
        return;
    struct list_head *li = head->next;
    int pairs = q_size(head) / 2;
    for (int i = 0; i < pairs; ++i) {
        element_t *cur = list_entry(li, element_t, list);
        element_t *next = list_entry(li->next, element_t, list);
        char *tmp = cur->value;
        cur->value = next->value;
        next->value = tmp;
        li = li->next->next;
    }
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head || q_size(head) <= 1)
        return;
    struct list_head *li, *safe;
    list_for_each_safe (li, safe, head) {
        li->next = li->prev;
        li->prev = safe;
    }
    safe = head->next;
    head->next = li->prev;
    head->prev = safe;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    if (!head || q_size(head) <= 1)
        return;

    LIST_HEAD(head2);
    list_cut_position(&head2, head, list_get_mid(head));
    q_sort(head);
    q_sort(&head2);
    merge(head, &head2);
}


void merge(struct list_head *head, struct list_head *head2)
{
    struct list_head *i_head = head->next, *i_head2, *next;
    for (i_head2 = head2->next; !list_empty(head2); i_head2 = next) {
        while (i_head != head &&
               strcmp(list_entry(i_head, element_t, list)->value,
                      list_entry(i_head2, element_t, list)->value) < 0) {
            i_head = i_head->next;
        }
        if (i_head == head)
            list_splice_tail_init(head2, i_head);
        else {
            next = i_head2->next;
            list_del_init(i_head2);
            list_add_tail(i_head2, i_head);
        }
    }
}

/*
 * Return the middle node of the queue
 */
struct list_head *list_get_mid(struct list_head *head)
{
    struct list_head *left = head, *right = head;
    while (!(right->prev == left || right->prev == left->next)) {
        left = left->next;
        right = right->prev;
    }
    return right->prev;
}

/*
 * Free the selected mode in the queue
 */
void list_free_node(struct list_head *li)
{
    if (!li)
        return;
    list_del_init(li);
    free(list_entry(li, element_t, list)->value);
    free(list_entry(li, element_t, list));
}