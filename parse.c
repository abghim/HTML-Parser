#include "fund.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STREQ(x,y) (strcmp((x),(y))==0)
#define TNAME 1
#define OPNAME 2
#define OPVAL 3
#define TEXT 4
#define TNAME_RED 5
#define OPNAME_RED 6
#define OPVAL_RED 7
#define INNTEXT 8
#define UTNAME 9

short isunary(char *);
void parse(Node *, char *);

int main(int argc, char **argv)
{
	FILE *fin;
	char content[100000];
	int ci=0;

	char c;

	if (argc==1){
		fin = stdin;
	} else if (argc==2) {
		fin = fopen(argv[1], "r");
	} else { fprintf(stderr, "bad file use\n"); return 1; }

	do {
		c = fgetc(fin);
		if( feof(fin) ) {
			break ;
		} content[ci++] = c;
	} while(1);

	// printf("%d", isahead(content, "<!DOCTYPE html", 0, -1 +0+14));
	Node *root = new_node("DOM");
	parse(root, content);

	recurse_print(root);

	return 0;
}

void parse(Node *DOM, char *raw) {
	int cursor = 0;
	char c;
	int wi = 0;
	short mode = TEXT;
	char inntext[10000] = {0};
	char tname[25] = {0};
	char opname[25] = {0};
	char opval[10000] = {0};

	short quoted = 0;
	
	short endtag = 0;
	short utag = 0;

	Node *working = NULL;
	Node *add_under = DOM;
	while (cursor < strlen(raw)) {
		c = raw[cursor];

		if (mode == TEXT) {
			if (c == '<') {
				if (working != NULL && ! STREQ(inntext, "")){
					add_option(working, new_option("text", inntext)); printf("(added text under %s)", working->name);
                    strcpy(inntext, "");
				}
				printf("\e[0;31m%c\e[0;37m", c);  // Red for '<'
				mode = TNAME;
				wi = 0;
				if (raw[cursor+1]=='/'){
					endtag = 1;
				} else { endtag = 0; }
			} else {
				// Collecting inner text
				if (c != '\t' && c != '\n'){
				inntext[wi++] = c;
				inntext[wi] = '\0';}
				printf("\e[0;36m%c\e[0;37m", c);  // Cyan for inner text
				
			}
		} else if (mode == TNAME) {
			if (!endtag) { //////// STARTTAG
				if ((97 <= c && c <= 122) || (65 <= c && c <= 90) || (48<=c && c<=57)) {
					tname[wi++] = c;
					tname[wi] = '\0';
					printf("\e[0;32m%c\e[0;37m", c);  // Green for tag name
				} else if (c == ' ') {
					printf(" ");
					mode = OPNAME;
					wi = 0;
					if (isunary(tname)) { utag = 1; } else { utag = 0; }
					// make tag
					working = new_node(tname); printf("(created%snode %s ", (utag) ? " unary ":" ", working->name);
					working->add_to = add_under; printf("add_to %s)", working->add_to->name);
					if (!utag) { add_under = working; }
				} else if (c == '>') {
					printf("\e[0;31m%c\e[0;37m", c);  // Red for '>'
					mode = TEXT;
					wi = 0;
					if (isunary(tname)) { utag = 1; } else { utag = 0; }
					// make tag
					working = new_node(tname); printf("(created%snode %s ",(utag) ? " unary " : " ", working->name);
					working->add_to = add_under; printf("add_to %s)", working->add_to->name);
                    if (!utag) { add_under = working; }
					if (utag){
						// wrapup
						add_node(working->add_to, working);
						printf("(added %s under %s)", working->name, working->add_to->name);
						utag = 0; endtag = 0; working = working->parent;
						printf("(working %s)", working->name);
						add_under = working;
					}
				}
			} else { ///// ENDTAG
                if ((97 <= c && c <= 122) || (65 <= c && c <= 90) || (48<=c && c<=57)) {
                    printf("\e[0;32m%c\e[0;37m", c);  // Green for tag name
				} else if (c=='/'){
					printf("\e[0;31m%c\e[0;37m", c);
                } else if (c == ' ') {
                    printf(" ");
                } else if (c == '>') {
                    printf("\e[0;31m%c\e[0;37m", c);  // Red for '>'
                    mode = TEXT;
                    wi = 0;
					// wrapup
                    add_node(working->add_to, working);
                    printf("(added %s under %s)", working->name, working->add_to->name);
					utag = 0; endtag = 0; working = working->parent;
					printf("(working %s)", working->name);
					add_under = working;
				}
			}
		} else if (mode == OPNAME) {
			if ((97 <= c && c <= 122) || (65 <= c && c <= 90) || (48<=c && c<=57)) {
				opname[wi++] = c;
				opname[wi] = '\0';
				printf("\e[0;33m%c\e[0;37m", c);  // Yellow for option name
			} else if (c == '=') {
				mode = OPVAL;
				wi = 0;
				printf("\e[0;31m%c\e[0;37m", c);  // Red for '='
			} else if (c == '>') {
				printf("\e[0;31m%c\e[0;37m", c);  // Red for '>'
				mode = TEXT;
				wi = 0;
				if (endtag && !utag) {
					// wrapup
                        add_node(working->add_to, working);
                        printf("(added %s under %s)", working->name, working->add_to->name);
                        utag = 0; endtag = 0; working = working->parent;
						printf("(working %s)", working->name);
						add_under = working;
				} else if (utag){
                    add_option(working, new_option(opname, opval)); printf("(added op %s under %s)", opname, working->name);
                    strcpy(opname, ""); strcpy(opval, "");
					// wrapup
                        add_node(working->add_to, working);
                        printf("(added %s under %s)", working->name, working->add_to->name);
                        utag = 0; endtag = 0; working = working->parent;
						printf("(working %s)", working->name);
						add_under = working;
				} else {
					add_option(working, new_option(opname, opval)); printf("(added op %s under %s)", opname, working->name);
					strcpy(opname, ""); strcpy(opval, "");
				}

			} else if (c == ' ') {
				printf(" ");
				// Ignore spaces between attributes
			} // else if (c == '/') {
//				printf("\e[0;31m%c\e[0;37m", c);  // Red for '/'
//			}
		} else if (mode == OPVAL) {
			if (c == '\"') {
				quoted = !quoted;
				printf("\e[0;31m%c\e[0;37m", c);  // Magenta for quotes in option value
				if (!quoted) {
					mode = OPNAME;
					wi = 0;
				}
			} else {
				printf("\e[0;35m%c\e[0;37m", c);  // Magenta for option value
				opval[wi++] = c;
				opval[wi] = '\0';
				if (!quoted && c == ' ') {
					mode = OPNAME;
					wi = 0;
				} else if (!quoted && c == '>') {
					printf("\e[0;31m%c\e[0;37m", c);  // Red for '>'
					mode = TEXT;
					wi = 0;
                	if (endtag && !utag) {
                	    // wrapup
                        add_node(working->add_to, working);
                        printf("(added %s under %s)", working->name, working->add_to->name);
                        utag = 0; endtag = 0; working = working->parent;
						printf("(working %s)", working->name);
						add_under = working;
                	} else if (utag){
                	    add_option(working, new_option(opname, opval)); printf("(added op %s under %s)", opname, working->name);
                	    strcpy(opname, ""); strcpy(opval, "");
                	    // wrapup
                        add_node(working->add_to, working);
                        printf("(added %s under %s)", working->name, working->add_to->name);
                        utag = 0; endtag = 0; working = working->parent;
						printf("(working %s)", working->name);
						add_under = working;
                	} else {
                	    add_option(working, new_option(opname, opval)); printf("(added op %s under %s)", opname, working->name);
                	    strcpy(opname, ""); strcpy(opval, "");
                	}
				}
			}
		}

		cursor++;
	}
}


/*
// wrapup
add_node(working->add_to, working);
printf("(added %s under %s)", working->add_to, working);
unary = 0; endtag = 0; working = NULL;
   

*/


short isunary(char *str) {
    // List of HTML tags that have no closing tags
    const char *self_closing_tags[] = {
        "area", "base", "br", "col", "embed", "hr", "img", "input", 
        "link", "meta", "param", "source", "track", "wbr"
    };

    // Number of self-closing tags
    int num_tags = sizeof(self_closing_tags) / sizeof(self_closing_tags[0]);

    // Iterate through the list of self-closing tags
    for (int i = 0; i < num_tags; i++) {
        if (strstr(str, self_closing_tags[i]) != NULL) {
            return 1;
        }
    }

    // If the tag is not found in the list, return 0
    return 0;
}
