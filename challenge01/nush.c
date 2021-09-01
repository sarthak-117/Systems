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

//checks return value of forks
void check_rv(int rv) {
	if (rv == -1) {
		perror("fail");
		exit(1);
	}
}

// changes directory
void cd(char** cmd) {
  	char* dir = cmd[1];
   	int status;
   	status = chdir(dir);
   	if (status == -1) {
        	printf("nush: cd: %s: No such file or directory\n", dir);
    	}

}

// executes code for the semicolon operation
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

// executes pipe command by using fork and pipe
void 
execute_pipe(svec* cmd) {
	char left[256];
	char right[256];
	strcpy(left, "");
	strcpy(right, "");
	int toggle = 0;
	for (int i = 0; i < cmd->size; i++) {
		if (strcmp(svec_get(cmd, i), "|") == 0 && toggle == 0) {
			toggle = 1;
		} else if (toggle == 0) {
			strcat(left, svec_get(cmd, i));
			strcat(left, " ");
		} else {
			strcat(right, svec_get(cmd, i));
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
			svec* second = tokenize(right);
			execute(second);
			free_svec(second);


		} else { //child 2
			close(p_read);
			close(1);
			rv = dup(p_write);
			check_rv(rv);
			close(p_write);
			svec* first = tokenize(left);
		//	char* command = first->head;
		//	char** args = to_array(first);
			execute(first);
			free_svec(first);
		//	execvp(command, args);
		}

	}
}

// executes && and ||, currently a broken function
void 
execute_logic(svec* cmd, int op) {
	char operator[3];
	if (op == 0) {
		operator[0] = '&';
		operator[1] = '&';
		operator[2] = '\0';
	} 
	else {
		operator[0] = '|';
		operator[1] = '|';
		operator[2] = '\0';

	}

	char right[256];
	strcpy(right, "");
	char left[256];
	strcpy(left, "");
	int seen = 0;
	for (int i = 0; i < cmd->size; ++i) {
		if (strcmp(svec_get(cmd, i), operator) == 0) {
			seen = 1;
		} 
		else if (seen == 1) {
			strcat(right, svec_get(cmd, i));
			strcat(right, " ");
		}
	       	else {
			strcat(left, svec_get(cmd, i));
			strcat(left, " ");
		}
	}
	svec* first = tokenize(left);
	svec* second = tokenize(right);

	int cpid;
	if (cpid = fork()) {
		check_rv(cpid);
		int status;	
		waitpid(cpid, &status, 0);
		if (WIFEXITED(status)) {
			if (op == 0 && WEXITSTATUS(status)== 0) {
				//execute(second);
				execvp(svec_get(first, 0), first->data);

			}
	       		else if (op == 1 && WEXITSTATUS(status) != 0) {
			//	execute(second);
			execvp(svec_get(first, 0), first->data);

			}
		}
	} else {
		execvp(svec_get(first, 0), first->data);
	}
	
/*
	int cpid;
	if ((cpid = fork())) {
		int status;
		waitpid(cpid, &status, 0);
		if ((op == 1 && status != 0) || (op == 0 && status == 0)) {
			execute(second);
		}
	}
       	else {
		execute(first);
		//char** args = to_array(first);
		//execvp(args[0], args);
	}*/
	free_svec(first);
	free_svec(second);
}

//execute_sleep helps with background processes
void
execute_sleep(svec* cmd){
	svec* update = make_svec();
	for (int i = 0; i < cmd->size-1; ++i) {
		if (strcmp(svec_get(cmd, i), "&") == 0) {
			//do nothing, we dont want this element
		} else {	
		svec_push_back(update, svec_get(cmd, i));
		}
	}
	cmd->data[cmd->size -1] = 0;
	int cpid;
	if ((cpid = fork())) {
		check_rv(cpid);
		//nothing here
 		int status;
		waitpid(cpid, &status, WNOHANG);
	}
	else {
	//	close(0);
      	//	int sleep = open("dev/null", O_CREAT | O_TRUNC | O_WRONLY,0644);
        //	dup(sleep);
        //	close(sleep);
		execvp(svec_get(update, 0), update->data);
	//	execute(cmd);
	//	execvp(svec_get(cmd, 0), cmd->data);
	}
	free(update);
}

// reads in from input files (first attempted by using open as O_RDONLY)
void
execute_in(svec* cmd) {
 	char left[256];
	char file[256];
	strcpy(left, "");
	strcpy(file, "");

	for (int i = 0; i < cmd->size; i++) {
		if (strcmp(svec_get(cmd, i), "<") == 0) {
			//strcat(file, svec_get(cmd, i + 1));

			strcat(left, svec_get(cmd, i+1));
			break;
		} 
		else {
			strcat(left, svec_get(cmd, i));
			strcat(left, " ");
		}
	}
	// the code block below used fopen to read inputs from the file... that did not pass the test
	/*
	char data[256];
	strcpy(data, "");

	FILE* pfile = fopen(file, "r");
	char reader[256];

	if (pfile != NULL) {
	while(1) {
		char* rv = fgets(reader, 256, pfile);
		if (!rv) 
		{
			//fclose(file);
			break;
		}
		strcat(data, rv);
		}
	}
	fclose(pfile);
	svec* first = tokenize(left);
	print_svec(first);
	svec* data2 = tokenize(data);
	print_svec(data2);
	for (int i = 0; i < data2->size; ++i) {
		svec_push_back(first, svec_get(data2, i));
	}
	free_svec(data2);*/
	// instead, i remove the < token and execute and that works 
	svec* first = tokenize(left);
	int cpid;
	if((cpid = fork())) {
		check_rv(cpid);
		int status;
		waitpid(cpid, &status, 0);
	}
	else {

	//	close(0);
	//	int rv = dup(fd);
	//	check_rv(rv);
	//	close(fd);
		execute(first);
	}
	free_svec(first);
}

// redirects output to a file
void
execute_out(svec* cmd) {
    int cpid;
    char left[256];
    char file[256];
    strcpy(file, "");
    strcpy(left,"");
    for (int i = 0; i < cmd->size; ++i) {
	    if (strcmp(svec_get(cmd, i), ">") == 0) {
		    strcat(file, svec_get(cmd, i+1));
		    break;
	    } else {
		    strcat(left, svec_get(cmd, i));
		    strcat(left, " ");
	    }
    }
    svec* first = tokenize(left);

    //gotten from nat tuck's  scratch repo
    if ((cpid = fork())) {
        waitpid(cpid, 0, 0);
    }
    else {
        int fd = open(
            file,
            O_CREAT | O_APPEND | O_WRONLY, 0644);

        close(1); // stdout
        int rv =  dup(fd); // copy fd to 1 (= stdout)
        check_rv(rv);
        close(fd);
	execute(first);
	free_svec(first);
    }
}

//input function
void
execute(svec* cmd)
{
	if (svec_contains(cmd, ";") == 0) {
		execute_semicolon(cmd);
	} 
	else if (svec_contains(cmd, "<") == 0) {
		execute_in(cmd);

	}
	else if (svec_contains(cmd, ">") == 0) {
		execute_out(cmd);
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
	else if (svec_contains(cmd, "|") == 0) {
		execute_pipe(cmd);
	}
       	else {
		if (cmd->size > 0) {
		if (strcmp(svec_get(cmd,0),  "cd") == 0) {
			cd(cmd->data);
		}
		else if (svec_contains(cmd, "exit") == 0) {
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
					args[i] =  svec_get(cmd, i);
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
