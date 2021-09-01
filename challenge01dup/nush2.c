#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "list.h"
#include "tokenizer.h"

void execute(svec* cmd);

void check_rv(int rv) {
	if (rv == -1) {
		perror("fail");
		exit(1);
	}
}

void cd(char** cmd) {
  	char* dir = cmd[1];
   	int status;
   	status = chdir(dir);
   	if (status == -1) {
        	printf("nush: cd: %s: No such file or directory\n", dir);
    	}

}

void execute_semicolon(svec* tokens) {
	char left[256];
	char right[256];
	strcpy(left, "");
	strcpy(right, "");
	int semi = 0;
	for(int i = 0; i < tokens->size; ++i) {
		if (strcmp(svec_get(tokens, i), ";") == 0 && semi == 0) {
			semi = 1;
		} else if (semi == 0) {
			strcat(left, svec_get(tokens, i));
			strcat(left, " ");
		} else {
			strcat(right,svec_get(tokens, i));
			strcat(right, " ");
		}
	}
	svec* first = tokenize(left);
	svec* second = tokenize(right);
	execute(first);
	free_svec(first);
	execute(second);
	free_svec(second);

}

void 
execute_pipe(svec* cmd) {/*
	char left[256];
	char right[256];
	strcpy(left, "");
	strcpy(right, "");
	int toggle = 0;
	list* iterator = cmd;
	for (; iterator; iterator = iterator->tail) {
		if (strcmp(iterator->head, "|") == 0) {
			toggle = 1;
		} else if (toggle == 0) {
			strcat(left, iterator->head);
			strcat(left, " ");
		} else {
			strcat(right, iterator->head);
			strcat(right, " ");
		}
	}

	int cpid1;
	if ((cpid1 = fork())) {//parent 1
		check_rv(cpid1);
		int status;
		waitpid(cpid1, &status, 0);
	} 
	else { //child 1
		int p[2];
		int rv = pipe(p);
		check_rv(rv);

		int p_read = p[0];
		int p_write = p[1];

		int cpid2;
		if ((cpid2 = fork())) { ///child1/parent2
			check_rv(cpid2);
			int status; 
			waitpid(cpid2, &status, 0); 
			
			close(p_write);
			close(0);
			
			rv = dup(p_read);			
			check_rv(rv);
			close(p_read);
			list* second = tokenize(right);
			execute(second);
			free_list(second);


		} else { //child 2
			close(p_read);
			close(1);
			rv = dup(p_write);
			check_rv(rv);
			close(p_write);
			list* first = tokenize(left);
		//	char* command = first->head;
		//	char** args = to_array(first);
			execute(first);
			free_list(first);
		//	execvp(command, args);
		}

	}*/
}

void 
execute_logic(svec* cmd, int op) {
/*	char operator[3];
	if (op == 0) {
		strcpy(operator, "&&");
	} 
	else {
		strcpy(operator, "||");
	}

		char right[256];
		strcpy(right, "");
		char left[256];
		strcpy(left, "");

		int seen = 0;
		list* iterator = cmd;
		for (; iterator; iterator = iterator->tail) {
			if (strcmp(iterator->head, operator) == 0 && seen == 0) {
				seen = 1;
			} 
			else if (seen == 1) {
				strcat(right, iterator->head);
				strcat(right, " ");
			}
		       	else {
				strcat(left, iterator->head);
				strcat(left, " ");
			}
		}

	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if ((op == 1 && WEXITSTATUS(status)) || (op == 0 && !WEXITSTATUS(status))) {
			list* second = tokenize(right);
			execute(second);
			free_list(second);
		}
	}
       	else {
		list* first = tokenize(left);
		execute(first);
		free_list(first);
		//char** args = to_array(first);
		//execvp(args[0], args);
	}*/
}
// execute returns an int, check to see if it is successful, dont need fork


void 
execute_sleep(svec* cmd){/*
	int cpid;
	if ((cpid = fork())) {
		//wait(3);
	}
	else {
		char dup[256];
		strcpy(dup, "");
		list* iterator = cmd;
		for (; iterator; iterator = iterator->tail) {
			if (strcmp(cmd->head, "&") == 0) {
				break;
			} else {
				strcat(dup, cmd->head);
				strcat(dup, " ");
			}
		}
		list* first = tokenize(dup);
		execute(first);
		free_list(first);		
	}*/
}

void
execute_in(svec* cmd) {/*
 	char* left = "";
	char* file = "";
	list* iterator = cmd;
	for (; iterator; iterator = iterator->tail) {
		if (strcmp(iterator->head, "<") == 0) {
			iterator = iterator->tail;
			file = iterator->head;
			break;
		} else {
			strcat(left, iterator->head);
			strcat(left, " ");
		}
	}

	int rv = dup(0);
	check_rv(rv);

	int cpid;
	if((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
	}
	else {

		list* first = tokenize(left);
		close(0);
		int fd = open(file, O_RDONLY);
		int rv = dup(fd);
		check_rv(rv);
		close(fd);
		execute(first);
		free_list(first);
	}*/
}

void
execute_out(svec* cmd) {/*
    int cpid;
    char* left = "";
    char* file = "";

    for (int i = 0; svec; iterator = iterator->tail) {
	    if (strcmp(iterator->head, ">") == 0) {
		    iterator = iterator->tail;
		    file = iterator->head;
		    break;
	    } else {
		    strcat(left, iterator->head);
		    strcat(left, " ");
	    }
    }
    svec* first = tokenize(left);

    //gotten from scratch repo
    if ((cpid = fork())) {
        waitpid(cpid, 0, 0);
    }
    else {
        int fd = open(
            file,
            O_CREAT | O_APPEND | O_WRONLY,
            0644
        );

        close(1); // stdout
        int rv =  dup(fd); // copy fd to 1 (= stdout)
        check_rv(rv);
        close(fd);
	execute(first);
	free_svec(first);
    }*/
}

void
execute(svec* cmd)
{
	if (svec_contains(cmd, ";") == 0) {
		execute_semicolon(cmd);
	} 
	else if (svec_contains(cmd, "|") == 0) {
		execute_pipe(cmd);
	}
	else if (svec_contains(cmd, "&&") == 0) {
		execute_logic(cmd, 0);
	}
	else if (svec_contains(cmd, "||") == 0) {
		execute_logic(cmd, 1);
	}
	else if (svec_contains(cmd, "&") == 0) {
		execute_sleep(cmd);
	}
	else if (svec_contains(cmd, "<") == 0) {
		execute_in(cmd);
	}
	else if (svec_contains(cmd, ">") == 0) {
		execute_out(cmd);
	}
       	else {
		if (cmd->size > 0) {
		if (strcmp(svec_get(cmd,0),  "cd") == 0) {
			cd(cmd->data);
			return;
		}
		else if (strcmp(svec_get(cmd,0), "exit") == 0) {
			exit(0);
		} else {
			int cpid;
			if ((cpid = fork())) {
				int status;
				waitpid(cpid, &status, 0);
			}
			else {
			//in child
				char* args[cmd->size + 1];
				for (int i = 0; i < cmd->size; i++) {
					strcpy(args[i], svec_get(cmd, i));
				}
				args[cmd->size] = 0; // null terminate
	//			free_list(cmd);
				execvp(svec_get(cmd, 0), args);	
			}
		}
	}
	}

}

int 
main(int argc, char* argv[]) 
{
	char cmd[256];

	if (argc == 1) {
		while(1) {
			printf("nush$ ");
			char* rv = fgets(cmd, 256, stdin);
			fflush(stdout);
			if (!rv) {
				break;
			}
			svec* toks = tokenize(rv);
			if (toks != NULL) {
				execute(toks);
			}
			free_svec(toks);
		}
	} else {
		FILE* file;
		file = fopen(argv[1], "r");
		if (file != NULL) {
			while(1) {
				char* rv = fgets(cmd, 256, file);
				fflush(stdout);
				if (!rv) 
				{
					//fclose(file);
					break;
				}
				svec* toks = tokenize(rv);
				execute(toks);
				free_svec(toks);
			}
		}
		fclose(file);
	}
	return 0;
}
