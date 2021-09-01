CFLAGS:= -g
LDLIBS:= -lbsd

tokens: tokens.o tokenizer.o svec.o 
	gcc $(CFLAGS) -o $@ $^ $(LDLIBS)

tokens.o: tokens.c tokenizer.h svec.h
	gcc $(CFLAGS) -c -o $@ $<

tokenizer.o: tokenizer.c  svec.h
	gcc $(CFLAGS) -c -o $@ $<

svec.o: svec.c svec.h
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o tokens tokenizer list

.PHONY: all clean

