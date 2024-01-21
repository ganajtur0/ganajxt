main:
	cc -o ganajxt ganajxt.c
gdb:
	cc -g -o ganajxt ganajxt.c && gdb ./ganajxt
