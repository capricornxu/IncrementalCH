#
# makefile for drawing.cpp
# - modified from make_temp
# 
# Course: EN 47 
#       Updated June 3, 2019
#
# use leda61

LEDALib = /usr/cots/leda-6.1
LFLAG = -L$(LEDALib) -lleda -lX11 -lm
INC = -I$(LEDALib)/incl

default: draw

draw: drawing.o
	g++ -o  draw drawing.o $(INC) $(LFLAG)
	chmod 770 draw

drawing.o: drawing.cpp
	g++ -c -g drawing.cpp $(INC) $(LFLAG)
	chmod 660 drawing.o

clean:
	rm -f core a.out draw drawing.o
