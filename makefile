
###########################################################################################
# Dependencies generated by g++ -MM

Video.o : Video.c Video.h
Audio.o : Audio.c Audio.h
main.o : main.c main.h Video.h Audio.h

###########################################################################################
# Variables to define the compiler and compiler options

CC = g++
COPTS = -O0 -g
Link =

###########################################################################################
# Definitions for all
all : Helmet

# An explicit rule to link each of the projects that we will building (All the .o files) #

Helmet : Video.o Audio.o main.o
	$(CC) $(LINK) -o Video.o Audio.o main.o

##########################################################################################
# Implicit rules for compilation

%.o : %.c
	# Compile the given files
	$(CC) $(COPTS) -c $<

#########################################################################################
# A clean rule, clean everything

clean :
	rm -f *.o
	rm -f *.s
	rm -f *.i
	rm -f Helmet

##########################################################################################
