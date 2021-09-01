#include "tokenizer.h"
#include "list.h"
#include "svec.h"
#include <stdio.h>
/* TODO:

   while (1) {
     printf("tokens$ ");
     fflush(stdout);
     line = read_line()
     if (that was EOF) {
        exit(0);
     }
     tokens = tokenize(line);
     foreach token in reverse(tokens):
       puts(token)
   }

*/

int
main(int _ac, char* _av[]) {

	char line[100];
	while(1) {	
		printf("token$ ");
		char* rv = fgets(line, 96, stdin);
		fflush(stdout);
		if (!rv) {
			break;
		}	
		svec* toks =  tokenize(rv);
		for (int i = 0; i < toks->size; i++) {
			puts(svec_get(toks, i));
		}
	}	
}


