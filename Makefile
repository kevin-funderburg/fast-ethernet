csp:
	gcc -Wall -std=c99 src/csp.c -o obj/csp
	gcc -Wall -std=c99 src/station.c -o obj/station
clean:
	@rm -rf csp station