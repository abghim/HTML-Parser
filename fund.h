#ifndef FUND_H
#define FUND_H
typedef struct html_node {
    struct html_node *next[20]; // array of pointers to next nodes
    struct html_node *parent;
	struct html_node *add_to;
	int next_count;
    struct option *options[20]; // array of next options
    int op_count;
    char *name;
} Node;


typedef struct option {
	char *key;
    char *value;
} Option;


Node *new_node( char *);
void add_node(Node *, Node *);

Option *new_option(char *, char *);
void add_option(Node *, Option *);

void recurse_free(Node *);
void recurse_print(Node *);

#endif
