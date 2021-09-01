#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "svec.h"


int 
special_char(const char* text, long ii) {
	if (text[ii] == '|' 
	    || text[ii] == '&'
	    || text[ii] == ';'
	    || text[ii] == '<'
	    || text[ii] == '>') {
		return 0;
	} else {
		return 1;
	}
}

char* 
read_string(const char* text, long ii) {
	int nn = 0;
	while (!isspace(text[ii + nn])) {
		if (special_char(text, ii + nn) == 0) {
			break;
		}
		++nn;
	}
	//plus 1 bc mem terminate
	char* str = malloc(nn + 1);
	memcpy(str, text + ii, nn);
	str[nn] = 0;
	return str;
}


svec* 
tokenize(const char* text) {
	svec* sv = make_svec();
	int nn = strlen(text);
	int ii = 0;
	while (ii < nn) {
		if (isspace(text[ii])){
			++ii;
			continue;
		}

		//special characters
		if (special_char(text, ii) == 0) {
			if ((text[ii] == '|' && text[ii + 1] == '|') 
			      || (text[ii] == '&' && text[ii + 1] == '&')) {
				char* op = malloc(3);
			//	int nn = ii + 2;
			//	memcpy(op, text + ii, nn);
				op[0] = text[ii];
				op[1] = text[ii + 1];
				op[2] = '\0';
				svec_push_back(sv, op);
				ii += 2;
				free(op);
			}
			else {
				char* op = malloc(2);
				//int nn = ii + 1;
				//memcpy(op, text +  ii, nn);
				op[0] = text[ii];
				op[1] = '\0';
				svec_push_back(sv, op);
				++ii;
				free(op);
			}
			continue;
		}
			

		
			char* str = read_string(text, ii);
			svec_push_back(sv, str);
			ii += strlen(str);
			free(str);	
	}

	return sv;
}
