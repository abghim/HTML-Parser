
#include "fund.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAGNAME 0
#define OP_NAME 1
#define OP_VAL 2




Node *new_node(char *key)
{
	Node *n = (Node *) malloc(sizeof(Node));

	if (n == NULL){
		fprintf(stderr, "%s allocating %s\n", "insufficient memory", key);
		exit(0);
	}

	n->name = strdup( key);
	n->next_count = 0;
	n->op_count = 0;
	return n;
}

void add_node(Node *parent, Node *child)
{
	(parent->next)[parent->next_count++] = child;
	child->parent = parent;
}

Option *new_option(char *k, char *v)
{
	Option *n = (Option *) malloc(sizeof(Option));
	n->key = strdup( k);
	n->value = strdup(v);
	return n;
}

void add_option(Node *parent, Option *child)
{
	(parent->options)[parent->op_count++] = child;
}

void recurse_print(Node *root)
{
	static char *mother;
	int i, j;

	printf("under %s: %s", mother, root->name);
	free(mother);
	printf(" (");
	if (root->op_count >0){ for (i=0; i<root->op_count; i++){
		printf("%s = %s", ((root->options)[i])->key,  ((root->options)[i])->value);
	}}
	printf(")\n");
	

	if (root->next_count > 0){ for (j=0; j<root->next_count; j++){

		mother = strdup( root->name);
		recurse_print((root->next)[j]);
	}}
	return;
}


/*
void recurse_free(Node *root)
{
	int i;
	static int j;



	if (root->next_count > 0){ for (j=0; j<(root->next_count); j++){
		printf("from %s recursing into %s (%s has %d child(ren) left)\n", root->name, (root->next)[j]->name, root->name, root->next_count);
		recurse_free((root->next)[j]);
		printf("(%s has %d child(ren) left)\n", root->name, --root->next_count);
	}} else {


	if (root->op_count >0){ for (i=0; i<root->op_count; i++){
		printf("freeing option %s...", (root->options)[i]->key);
		free(((root->options)[i])->key);
		free( ((root->options)[i])->value);
		free((root->options)[i]);
		printf("done\n");
		
	}}
		printf("freeing %s...", root->name);
		free(root->name);
		free(root);
		printf("done\n");
	}
	return;
}
*/
