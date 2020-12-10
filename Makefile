csp:
	 gcc -Wall -std=c99 csp.c -o csp
	 gcc -Wall -std=c99 station.c -o station
clean:
	@rm -rf csp station