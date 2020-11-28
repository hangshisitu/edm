SHELL = /bin/sh

CCC = gcc -g -c
CCO = gcc -g -o 


sideside_web:	onesideloveonesidefriend.o socket_tool.o base64qp.o conf.o writelog.o
	$(CCO)	sideside_web onesideloveonesidefriend.o socket_tool.o base64qp.o conf.o writelog.o -lpthread
onesideloveonesidefriend.o:	onesideloveonesidefriend.c
	$(CCC)	onesideloveonesidefriend.c
socket_tool.o:	socket_tool.c
	$(CCC)	socket_tool.c
base64qp.o:	base64qp.c
	$(CCC)	base64qp.c
conf.o:	conf.c
	$(CCC)	conf.c
writelog.o:	writelog.c
	$(CCC)	writelog.c

clean:
	rm *.o
	rm onesideloveonesidefriend


