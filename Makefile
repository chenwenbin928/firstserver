object=server.o connect.o  signalex.o \
	msgprocess.o queue.o  log.o  mempool.o  file_mutex.o\
	event.o
server : $(object)
	cc -g $(object) -o  server

server.o : msgprocess.h  connect.h   signalex.h server.h  file_mutex.h  event.h
connect.o:  connect.h  server.h
msgprocess.o:  msgprocess.h signalex.h
queue.o :  queue.h mempool.h server.h
mempool.o : mempool.h
signalex.o :   signalex.h  msgprocess.h
log.o: log.h
file_mutex.o: file_mutex.h
event.o :  event.h  file_mutex.h
.PHONY : clean
clean:
	rm server $(object)


