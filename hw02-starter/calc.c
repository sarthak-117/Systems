#include <stdio.h>
#include <stdlib.h>

long calculate(long a, long b, char* op);

int main (int argc, char* argv[]) {
	if (argc != 4) {
		printf("Usage:\n  %s N op M", argv[0]);
		return 1;
	}
	
	if (strcmp("+", argv[2]) != 0 && strcmp("-", argv[2]) != 0
		&& strcmp("*", argv[2]) != 0 && strcmp("/", argv[2]) != 0) {
		printf("Usage:\n  %s N op M", argv[0]);
		return 1;
	}	

	printf("%ld %s %ld = %ld", atol(argv[1]), argv[2], atol(argv[3]), calculate(atol(argv[1]),atol(argv[3]), argv[2]));
	return 0;
}


long calculate(long a, long b, char* op) {
	if (strcmp("+", op) == 0) {
			return a + b;
	}
	else if (strcmp("-", op) == 0) {
			return a - b;
	}
	else if (strcmp("*", op) == 0) {
			return a * b;
	}
	else if (strcmp("/", op) == 0) {
			return a / b;
	}
}
