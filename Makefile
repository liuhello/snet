OPT ?= -O2 -DNDEBUG

$(shell ./build_config.sh build_config.mk ./)
include build_config.mk

CXX = g++
CCFLAGS = -g -Wall -lpthread -D WINDOWS_OS
INCLUDE = .

LIBOBJECTS = $(SOURCES:.cc=.o)
LIBRARY=libsnet.a

#OBJCS = stream.o

all	: test_stream test_socket test_file ${LIBRARY}

test_stream : test/test_stream.o ${LIBRARY}
	${CXX} ${CCFLAGS} -o test_stream test/test_stream.o ${LIBRARY}

test_socket : test/test_socket.o ${LIBRARY}
	${CXX} ${CCFLAGS} -o test_socket test/test_socket.o ${LIBRARY}

test_file : test/test_file.o ${LIBRARY}
	${CXX} ${CCFLAGS} -o test_file test/test_file.o ${LIBRARY}
	
${LIBRARY}: ${LIBOBJECTS}
	rm -f $@
	${AR} -rs $@ ${LIBOBJECTS}	
.cc.o:
	${CXX} -I${INCLUDE} ${CCFLAGS} -c $< -o $@

	
clean : 
	rm -rf *.o */*.o test_stream* test_socket* test_file* build_config.mk ${LIBRARY}




