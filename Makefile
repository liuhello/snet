OPT ?= -O2 -DNDEBUG

$(shell ./build_config.sh build_config.mk ./)
include build_config.mk

CXX = g++
CCFLAGS = -g -Wall -lpthread -D WINDOWS_OS
INCLUDE = .

LIBOBJECTS = $(SOURCES:.cc=.o)

#OBJCS = stream.o

all	: test_stream test_socket

test_stream : test/test_stream.o ${LIBOBJECTS}
	${CXX} ${CCFLAGS} -o test_stream test/test_stream.o ${LIBOBJECTS}

test_socket : test/test_socket.o ${LIBOBJECTS}
	${CXX} ${CCFLAGS} -o test_socket test/test_socket.o ${LIBOBJECTS}
	
#socket/stream.o : config.h socket/stream.h socket/stream.cc
#	${CXX} -I${INCLUDE} ${CCFLAGS} -o socket/stream.o -c socket/stream.cc
#socket/packet.o : config.h socket/stream.o socket/packet.h socket/packet.cc
#	${CXX} -I${INCLUDE} ${CCFLAGS} -o socket/packet.o -c socket/packet.cc
#socket/socket.o : config.h socket/socket.h socket/socket.cc
#	${CXX} -I${INCLUDE} ${CCFLAGS} -o socket/socket.o -c socket/socket.cc
#test_stream.o : config.h test_stream.cc socket/stream.o
#	${CXX} -I${INCLUDE} ${CCFLAGS} -c test_stream.cc
#test_socket.o : config.h test_socket.cc
#	${CXX} -I${INCLUDE} ${CCFLAGS} -c test_socket.cc

.cc.o:
	${CXX} -I${INCLUDE} ${CCFLAGS} -c $< -o $@

	
clean : 
	rm -rf *.o */*.o test_stream test_stream.exe* test_socket test_socket.exe* build_config.mk




