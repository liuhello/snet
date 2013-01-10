CXX = g++
CCFLAGS = -g -Wall
INCLUDE = .

#OBJCS = stream.o

test_stream : test_stream.o socket/stream.o socket/packet.o
	${CXX} -o test_stream test_stream.o socket/stream.o socket/packet.o

test_socket : test_socket.o socket/stream.o socket/packet.o socket/socket.o
	${CXX} -o test_socket test_socket.o socket/stream.o socket/packet.o socket/socket.o
	
socket/stream.o : config.h socket/stream.h socket/stream.cc
	${CXX} -I${INCLUDE} ${CCFLAGS} -o socket/stream.o -c socket/stream.cc
socket/packet.o : config.h socket/stream.o socket/packet.h socket/packet.cc
	${CXX} -I${INCLUDE} ${CCFLAGS} -o socket/packet.o -c socket/packet.cc
socket/socket.o : config.h socket/socket.h socket/socket.cc
	${CXX} -I${INCLUDE} ${CCFLAGS} -o socket/socket.o -c socket/socket.cc
test_stream.o : config.h test_stream.cc socket/stream.o
	${CXX} -I${INCLUDE} ${CCFLAGS} -c test_stream.cc
test_socket.o : config.h test_socket.cc
	${CXX} -I${INCLUDE} ${CCFLAGS} -c test_socket.cc
all	: test_stream test_socket
	
clean : 
	rm -rf *.o socket/*.o test_stream test_stream.exe




