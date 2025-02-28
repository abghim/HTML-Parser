void parse(Node *DOM, char *raw) {
	int cursor = 0;
	char c;
	int wi = 0;
	short mode = TEXT;
	char inntext[10000] = {0};
	char tname[25] = {0};
	char opname[25] = {0};
	char opval[10000] = {0};

	short unary = 0;
	short endtag = 0;

	short quoted = 0;

	Option *options[];
	int opi = 0;
	Node *done = DOM;
	Node *working = DOM;


	while (cursor < strlen(raw)) { // wrapup -> endtag&&'>' || !endtag&unary
		c = raw[cursor];

		if (mode == TEXT) {
			if (c == '<') {
				printf("\e[0;31m%c\e[0;37m", c);  // Red for '<'
				mode = TNAME;
				wi = 0;
				if (raw[cursor+1]=='/'){
					endtag = 1;
                    options[opi++] = new_option("text", inntext);
                    strcpy(inntext, "");
				} else { endtag = 0; }
			} else {
				// Collecting inner text
				inntext[wi++] = c;
				inntext[wi] = '\0';
				printf("\e[0;36m%c\e[0;37m", c);  // Cyan for inner text
			}
		} else if (mode == TNAME) {
			if ((97 <= c && c <= 122) || (65 <= c && c <= 90)) {
				tname[wi++] = c;
				tname[wi] = '\0';
				printf("\e[0;32m%c\e[0;37m", c);  // Green for tag name
			} else if (c == ' ') {
				printf(" ");
				mode = OPNAME;
				working = new_node(tname);
				wi = 0;
			} else if (c == '>') {
				printf("\e[0;31m%c\e[0;37m", c);  // Red for '>'
				working = new_node(tname);
				if (!unary && endtag){
					// wrapup
					for (int i=0; i<(sizeof(options)/sizeof(options[0])); i++){
				    	add_option(working, options[i]);
				    	options[i] = NULL;
					} add_node(done, working);
					done = working;
					working = NULL;
					mode = TEXT;
					wi = 0;
				} else if (unary){
                    for (int i=0; i<(sizeof(options)/sizeof(options[0])); i++){
                        add_option(working, options[i]);
                        options[i] = NULL;
                    } add_node(done, working);
                    done = working;
                    working = NULL;
					mode=TEXT;
					wi = 0;
					// wrapup
				} else {
					mode = TEXT;
					wi = 0;
				}
			} else if (c=='/') {
				printf("\e[0;31m%c\e[0;37m", c);  // Red for '/'
			}
		} else if (mode == OPNAME) {
			if ((97 <= c && c <= 122) || (65 <= c && c <= 90)) {
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
                if (!unary && endtag){
                    // wrapup
                    for (int i=0; i<(sizeof(options)/sizeof(options[0])); i++){
                        add_option(working, options[i]);
                        options[i] = NULL;
                    } add_node(done, working);
                    done = working;
                    working = NULL;
                } else if (unary){
                    // wrapup
                    for (int i=0; i<(sizeof(options)/sizeof(options[0])); i++){
                        add_option(working, options[i]);
                        options[i] = NULL;
                    } add_node(done, working);
                    done = working;
                    working = NULL;
                } else {
                    mode = TEXT;
                    wi = 0;
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
					options[opi++] = new_option(opname, opval);
					strcpy(opname, "");
					strcpy(opval, "");
				}
			} else {
				printf("\e[0;35m%c\e[0;37m", c);  // Magenta for option value
				opval[wi++] = c;
				opval[wi] = '\0';
				if (!quoted && c == ' ') {
					printf(" ");
					mode = OPNAME;
					wi = 0;
                    options[opi++] = new_option(opname, opval);
                    strcpy(opname, "");
                    strcpy(opval, "");
				} else if (!quoted && c == '>') {
					printf("\e[0;31m%c\e[0;37m", c);  // Red for '>'
					mode = TEXT;
					wi = 0;
                    options[opi++] = new_option(opname, opval);
                    strcpy(opname, "");
                    strcpy(opval, "");
                	if (!unary && endtag){
                    	// wrapup
                    	for (int i=0; i<(sizeof(options)/sizeof(options[0])); i++){
                    	    add_option(working, options[i]);
                    	    options[i] = NULL;
                    	} add_node(done, working);
                    	done = working;
                    	working = NULL;
                	} else if (unary){
                    	// wrapup
                    	for (int i=0; i<(sizeof(options)/sizeof(options[0])); i++){
                    	    add_option(working, options[i]);
                    	    options[i] = NULL;
                    	} add_node(done, working);
                    	done = working;
                    	working = NULL;
                	} else {
                	    mode = TEXT;
                	    wi = 0;
                	}
				}
			}
		}

		cursor++;
	}
}


