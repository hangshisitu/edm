SHELL = /bin/sh

CCC = gcc -g -c
CCO = gcc -g -o

findyouallovertheworld:	findyouallovertheworld.o writelog.o socket_tool.o conf.o iparea.o
	$(CCO)	findyouallovertheworld findyouallovertheworld.o writelog.o socket_tool.o conf.o iparea.o -liconv -ldb -lpthread
findyouallovertheworld.o:	findyouallovertheworld.c
	$(CCC)	findyouallovertheworld.c
writelog.o:	writelog.c
	$(CCC)	writelog.c
conf.o:	conf.c
	$(CCC)	conf.c
socket_tool.o:	socket_tool.c
	$(CCC)	socket_tool.c
iparea.o:	iparea.c
	$(CCC)	iparea.c

clean:
	rm *.o
	rm findyouallovertheworld
