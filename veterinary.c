#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INPUT_FILE "logs.txt"

typedef struct Cat_s {
    int arrival;            // Cat's arrival time at the clinic
                            // Minutes since clinic opened (unique)
    char *name;             // Cat's unique name (dynamic string)
    int duration;           // Required treatment time in minutes
} Cat;

typedef struct SLLNode_s {
    Cat *cat;               // Pointer to a dynamically allocated Cat
    struct SLLNode_s *next;
} SLLNode;


// Function Prototypes
void myMain(FILE *ifile);

// You may add more functions if necessary

typedef struct Queue_s {
    SLLNode *front;
    SLLNode *rear;
} Queue;

typedef struct NameNode_s {
    char *name;
    struct NameNode_s *next;
} NameNode;

typedef struct Stack_s {
    NameNode *top;
} Stack;


// housekeeping for Cat + SLLNode

Cat *cat_create(int arrival, const char *name_in, int duration) {
    Cat *c = (Cat *)malloc(sizeof(Cat));
    c->arrival = arrival;
    c->duration = duration;

    char buf[26];
    strcpy(buf, name_in);

    int len = 0;
    while (buf[len] != '\0') len++;

    c->name = (char *)malloc((len + 1) * sizeof(char));
    strcpy(c->name, buf);

    return c;
}

void cat_destroy(Cat *c) {
    if (!c)
        return;
    free(c->name);
    free(c);
}

SLLNode *sll_create_node_cat(Cat *c) {
    SLLNode *n = (SLLNode *)malloc(sizeof(SLLNode));
    n->cat = c;
    n->next = NULL;
    return n;
}

void sll_destroy_node_cat(SLLNode *node) {
    free(node);
}

Queue *queue_create(void) {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    return q;
}

int queue_is_empty(Queue *q) {
    return q->front == NULL;
}

// Enqueue a cat
void queue_enqueue(Queue *q, Cat *c) {
    SLLNode *tmp = sll_create_node_cat(c);

    // Scenario 1
    if (queue_is_empty(q)) {
        q->front = tmp;
        q->rear = tmp;
        return;
    }

    // Scenario 2
    q->rear->next = tmp;
    q->rear = tmp;
}

// Dequeue returns cat and frees the queue node
Cat *queue_dequeue(Queue *q) {
    if (queue_is_empty(q)) return NULL;

    SLLNode *tmp = q->front;
    q->front = tmp->next;

    if (q->front == NULL) {
        q->rear = NULL;
    }

    Cat *c = tmp->cat;
    sll_destroy_node_cat(tmp);
    return c;
}

void queue_destroy(Queue *q) {
    SLLNode *ptr = q->front;
    SLLNode *tmp;

    while (ptr != NULL) {
        tmp = ptr->next;
        sll_destroy_node_cat(ptr);
        ptr = tmp;
    }

    free(q);
}

Stack *stack_create(void) {
    Stack *s = (Stack *)malloc(sizeof(Stack));
    s->top = NULL;
    return s;
}

int stack_is_empty(Stack *s) {
    return s->top == NULL;
}

void stack_push_name(Stack *s, const char *name) {
    int len = 0;
    while (name[len] != '\0') len++;

    NameNode *n = (NameNode *)malloc(sizeof(NameNode));
    n->name = (char *)malloc((len + 1) * sizeof(char));
    strcpy(n->name, name);

    n->next = s->top;
    s->top = n;
}

char *stack_pop_name(Stack *s) {
    if (stack_is_empty(s)) return NULL;

    NameNode *tmp = s->top;
    s->top = tmp->next;

    char *name = tmp->name;  // caller will free
    free(tmp);
    return name;
}

void stack_destroy(Stack *s) {
    while (!stack_is_empty(s)) {
        char *name = stack_pop_name(s);
        free(name);
    }
    free(s);
}

void insert_event_sorted(SLLNode **head, Cat *c) {
    SLLNode *n = sll_create_node_cat(c);

    if (*head == NULL || (*head)->cat->arrival > c->arrival) {
        n->next = *head;
        *head = n;
        return;
    }

    SLLNode *cur = *head;
    while (cur->next != NULL && cur->next->cat->arrival < c->arrival) {
        cur = cur->next;
    }

    n->next = cur->next;
    cur->next = n;
}

void reject_queue_remaining(Queue *q) {
    while (!queue_is_empty(q)) {
        Cat *c = queue_dequeue(q);
        printf("Cannot accommodate %s\n", c->name);
        cat_destroy(c);
    }
}

void free_event_list(SLLNode *events) {
    while (events != NULL) {
        SLLNode *tmp = events;
        events = events->next;
        cat_destroy(tmp->cat);
        sll_destroy_node_cat(tmp);
    }
}

void print_exposure_report(Stack *exposed) {
    if (stack_is_empty(exposed)) {
        printf("No Exposed Cats\n");
        return;
    }

    printf("Exposed Cats\n");
    while (!stack_is_empty(exposed)) {
        char *name = stack_pop_name(exposed);
        printf("%s\n", name);
        free(name);
    }
}

/* BEGIN: DO NOT MODIFY THE MAIN FUNCTION */
#ifndef MAIN_FUNCTION
int main(void) {
    // Open the input file for reading.
    // This is the only part of the entire code where the file
    // is going to be opened. You should not have any
    // fopen() function call in your functions. Simply
    // pass this existing FILE pointer when necessary.
    FILE *ifile = fopen(INPUT_FILE, "r");

    if( ifile == NULL ) {
        printf("File Does Not Exist!\n");
        return 1;
    }

    // Calls your own main function and passes the file stream
    myMain(ifile);

    // Close the file
    fclose(ifile);

    return 0;
}
#endif
/* END: DO NOT MODIFY THE MAIN FUNCTION */


/* Function Definitions */
void myMain(FILE *ifile) {
    // TODO: Complete this function

    SLLNode *events = NULL;          // sorted by arrival (built via ordered insertion)
    Queue *waiting = queue_create(); // shared FIFO waiting queue
    Stack *exposed = stack_create(); // LIFO list of names treated by Dr. Dos

    while (1) {
        int arrival;
        if (fscanf(ifile, "%d", &arrival) != 1) break;
        if (arrival == -1) break;

        char name_buf[26];
        int duration;

        fscanf(ifile, "%25s %d", name_buf, &duration);

        Cat *c = cat_create(arrival, name_buf, duration);

        // Zero-duration rule: ignore completely
        if (c->duration == 0) {
            cat_destroy(c);
        } else {
            insert_event_sorted(&events, c);
        }
    }

    // minute by minute simulation
    int uno_free_time = 0;
    int dos_free_time = 0;

    for (int t = 0; t < 480; t++) {
        // Add all cats whose arrival == t into waiting queue
        while (events != NULL && events->cat->arrival == t) {
            SLLNode *tmp = events;
            events = events->next;

            queue_enqueue(waiting, tmp->cat);
            sll_destroy_node_cat(tmp);
        }

        int changed = 1;
        while (changed) {
            changed = 0;

            // Assign Dr. Uno first if free
            if (t >= uno_free_time && !queue_is_empty(waiting)) {
                while (!queue_is_empty(waiting)) {
                    Cat *c = queue_dequeue(waiting);

                    if (t + c->duration <= 480) {
                        printf("Doctor Uno treated %s at %d\n", c->name, t);
                        uno_free_time = t + c->duration;
                        cat_destroy(c);
                        changed = 1;
                        break;
                    } else {
                        printf("Cannot accommodate %s\n", c->name);
                        cat_destroy(c);
                        changed = 1;
                    }
                }
            }

            // Assign Dr. Dos if free
            if (t >= dos_free_time && !queue_is_empty(waiting)) {
                while (!queue_is_empty(waiting)) {
                    Cat *c = queue_dequeue(waiting);

                    if (t + c->duration <= 480) {
                        printf("Doctor Dos treated %s at %d\n", c->name, t);
                        dos_free_time = t + c->duration;

                        stack_push_name(exposed, c->name);

                        cat_destroy(c);
                        changed = 1;
                        break;
                    } else {
                        printf("Cannot accommodate %s\n", c->name);
                        cat_destroy(c);
                        changed = 1;
                    }
                }
            }
        }
    }

    // After closing reject anyone still waiting
    reject_queue_remaining(waiting);

    // Any cats that arrived at/after 480 are still in events list
    while (events != NULL) {
        SLLNode *tmp = events;
        events = events->next;

        printf("Cannot accommodate %s\n", tmp->cat->name);
        cat_destroy(tmp->cat);
        sll_destroy_node_cat(tmp);
    }

    // report
    print_exposure_report(exposed);

    queue_destroy(waiting);
    stack_destroy(exposed);
}
