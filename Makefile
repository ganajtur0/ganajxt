main:
	cc -o ganajxt ganajxt.c
gdb:
	cc -g -o gdbxt ganajxt.c && gdb --args gdbxt -d test.gxt
debug:
	cc -o ganajxt ganajxt.c && cp test.bkp.txt test.txt && ./ganajxt -c test.txt && ./ganajxt -d test.gxt
