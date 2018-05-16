


#Please change to your xercesc installation directory
XERCESC_HOME:=/home/lu/Documents/Xerces/install

CC=g++
CFLAGS=-std=c++11
INC=-I /opt/include -I $(XERCESC_HOME)/include/
LIB=-L /opt/lib -L $(XERCESC_HOME)/lib -lxerces-c 



ALL_SOURCE_FILES:=$(shell ls *.cpp)
ALL_OBJECTS:=$(ALL_SOURCE_FILES:.cpp=.o)

xpdl_compiler:$(ALL_OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIB)
	mkdir -p ../bin
	mv $@ ../bin

%.o:%.cpp %.hpp
	$(CC) -c $< $(CFLAGS) $(INC) 

%.o:%.cpp
	$(CC) -c $^ $(CFLAGS) $(INC) 


clean:
	-@rm -rf *.o

allclean:
	-@rm -rf *.o ../bin ./xpdl.hpp

run:
	xpdl_compiler ../example/system.xml && vim xpdl.hpp 
