main:
	cc -o ganajxt ganajxt.c
gdb:
	cc -g -o gdbxt ganajxt.c
debug:
	cc -o ganajxt ganajxt.c && cp test.bkp.txt test.txt && ./ganajxt -c test.txt && ./ganajxt -d test.gxt
