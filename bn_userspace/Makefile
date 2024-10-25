CC := gcc -O2
CFLAGS := -Wall 

OBJS := bn.o

fib: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $@.c $^

test: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $@.c $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -MMD -MF .$@.d $<

clean: 
	rm bn.o fib