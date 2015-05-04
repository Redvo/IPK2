#
# IPK Project 1 - 18.3.2015
# Makefile
# Author: Lukáš Rendvanský
# E-mail: xrendv00@stud.fit.vutbr.cz
#	

# Names of output executable files

NAME_CLIENT=client
NAME_SERVER=server

# Names of object files

OBJECT_CLIENT=$(NAME_CLIENT).o arguments.class.o connection.class.o exception.class.o
OBJECT_SERVER=$(NAME_SERVER).o arguments.class.o connection.class.o exception.class.o

# Compiler

CXX=g++

# Standard

STD=-std=c++11

# Compiler flags

CXXFLAGS=-Wall -Wextra -pedantic -g -W -O3

# Libraries

LIBS=-lm -lrt

# Start rule

all: $(NAME_CLIENT) $(NAME_SERVER)

# Universal rule to generate obejct files	

%.o : %.cpp
	$(CXX) $(STD) $(CXXFLAGS) -c $< -o $@

# Linking required object files to runnable binary files

$(NAME_CLIENT): $(OBJECT_CLIENT)
	$(CXX) $(STD) $(OBJECT_CLIENT) -o $@ $(LIBS)

# Linking required object files to runnable binary files

$(NAME_SERVER): $(OBJECT_SERVER)
	$(CXX) $(STD) $(OBJECT_SERVER) -o $@ $(LIBS)

# Removing all object and binary files

rmall: pack clean
	rm -f *.c *.h protokol.pdf

clean:
	rm -f *~ *.o 
	rm -f $(NAME_CLIENT) $(NAME_SERVER)

pack:
	tar -cvzf xrendv00.tgz *.cpp *.hpp Makefile readme test.sh

test: all
	chmod u+x test.sh
	bash test.sh

.PHONY: clean all rmall pack test

# End of Makefile