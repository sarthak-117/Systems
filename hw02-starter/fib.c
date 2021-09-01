
#include <stdio.h>
#include <stdlib.h>

long fib(long val);

int
main(int argc, char* argv[])
{
    if (argc != 2 || atol(argv[1])< 0) {
        printf("Usage:\n  %s N, where N > 0\n", argv[0]);
        return 1;
    }

    printf("fib(%ld) = %ld", atol(argv[1]), fib(atol(argv[1])));
    return 0;
}

long fib(long val) {
	if (val == 0) {
	return 0;
	}
	else if (val == 1) {
	return 1;
	}
	else {
	return fib(val-1) + fib(val-2);
	}
}



