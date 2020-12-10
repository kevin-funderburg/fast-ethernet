daytime:
	 gcc -Wall -std=c99 daytime_svr.c -o daytime_svr
	 gcc -Wall -std=c99 daytime_cli.c -o daytime_cli
tcp:
	gcc -Wall -std=c99 tcpcli01.c -o tcpcli01
	gcc -Wall -std=c99 tcpserv01.c -o tcpserv01
clean:
	@rm -rf daytime_svr daytime_cli tcpcli01 tcpserv01