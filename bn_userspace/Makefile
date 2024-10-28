CC := gcc
CFLAGS := -Wall

OBJS := bn.o bn_pool.o
OBJS_O := bn_origin.o

test: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $@.c $^

fib: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $@.c $^

bench: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $@.c $^

fib_origin: $(OBJS_O)
	$(CC) $(LDFLAGS) -o $@ fib.c -DBN_ORIGIN $^

bench_origin: $(OBJS_O)
	$(CC) $(LDFLAGS) -o $@ bench.c -DBN_ORIGIN $^


%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c -MMD -MF .$@.d $<

clean: 
	rm $(OBJS) $(OBJS_B) fib test bench