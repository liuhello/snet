CXX = g++
CCFLAGS = -g -Wall
INCLUDE = .

#OBJCS = stream.o

test_stream : test_stream.o socket/stream.o
	${CXX} -o test_stream test_stream.o socket/stream.o
    
socket/stream.o : config.h socket/stream.h socket/stream.cc
	${CXX} -I${INCLUDE} ${CCFLAGS} -o socket/stream.o -c socket/stream.cc
    
test_stream.o : config.h test_stream.cc socket/stream.o
	${CXX} -I${INCLUDE} ${CCFLAGS} -c test_stream.cc

clean : 
	rm -rf *.o socket/*.o test_stream test_stream.exe




