SHELL = /bin/sh

CPPLIB_PATH = ../cpplib
CCC = gcc -g -c -I$(CPPLIB_PATH)/thirdparty/include/db4
CCO = gcc -g  -L$(CPPLIB_PATH)/thirdparty/lib64

maillist_daemon7:	yxlist.o maillist_daemon7.o commuteother7.o socket_tool.o maillist7.o daemon_conf.o writelog.o base64qp.o tool.o
	$(CCO)	-o maillist_daemon7 yxlist.o maillist_daemon7.o maillist7.o commuteother7.o socket_tool.o daemon_conf.o writelog.o base64qp.o tool.o -lpthread -ldb
maillist_daemon7.o:	maillist_daemon7.c
	$(CCC)	maillist_daemon7.c
commuteother7.o:		commuteother7.c
	$(CCC)	commuteother7.c
maillist7.o:		maillist7.c
	$(CCC)	maillist7.c
socket_tool.o:		socket_tool.c
	$(CCC)	socket_tool.c
daemon_conf.o:	daemon_conf.c
	$(CCC)	daemon_conf.c
writelog.o:	writelog.c
	$(CCC)	writelog.c
base64qp.o:	base64qp.c
	$(CCC)	base64qp.c
tool.o:		tool.c
	$(CCC)	tool.c
yxlist.o:	yxlist.c
	$(CCC)	yxlist.c

clean:
	rm  maillist_daemon7 *.o
