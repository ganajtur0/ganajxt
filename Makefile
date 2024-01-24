main:
	cc -o ganajxt ganajxt.c
gdb:
	cc -g -o gdbxt ganajxt.c && gdb --args gdbxt -d test.gxt
