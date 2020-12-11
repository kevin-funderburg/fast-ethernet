CC = gcc
CFLAGS = -I./src -std=gnu99 -g

obj/csp: obj/csp.o obj/frame.o | obj
	$(CC) $(CFLAGS) -o $@ $^

obj/station: obj/station.o obj/frame.o | obj
	$(CC) $(CFLAGS) -o $@ $^

obj/csp.o: src/csp.c src/csp.h | obj
	$(CC)  $(CFLAGS) -c $< -o $@

obj/station.o: src/station.c src/station.h | obj
	$(CC)  $(CFLAGS) -c $< -o $@

obj/frame.o: src/frame.c src/frame.h
	$(CC)  $(CFLAGS) -c $< -o $@

.PHONY: clean test objdir

obj:
	@mkdir -p obj

clean:
	@rm -rf obj