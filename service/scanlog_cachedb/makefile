SHELL = /bin/sh


CCC = gcc -g -c

CCO = gcc -g -o


scanlogdaemon3:	scanlogdaemon3.o writelog.o conf.o iparea.o socket_tool.o
	$(CCO) scanlogdaemon3 scanlogdaemon3.o writelog.o conf.o iparea.o socket_tool.o -ldb -liconv -lpthread
scanlogdaemon3.o:	scanlogdaemon3.c
	$(CCC)	scanlogdaemon3.c
writelog.o:	writelog.c
	$(CCC)	writelog.c
conf.o:	conf.c
	$(CCC)	conf.c
iparea.o:	iparea.c
	$(CCC)	iparea.c
socket_tool.o:	socket_tool.c
	$(CCC)	socket_tool.c

clean:
	rm *.o
	rm scanlogdaemon3
