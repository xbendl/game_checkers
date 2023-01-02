CC = g++

.SUFFIXES:	.o .cc

.cc.o:
	$(CC) -c `wx-config-2.8 --cxxflags` -o $@ $<

all: dama2008 damaserver2008

dama2008: clientcom.o xmlcom.o xmlconfig.o clientboard.o tinyxml.o tinyxmlparser.o \
        tinystr.o tinyxmlerror.o clientframe.o clientgame.o
	$(CC) -g -o ../dama2008 clientcom.o xmlcom.o xmlconfig.o clientboard.o tinyxml.o \
        tinyxmlparser.o tinystr.o tinyxmlerror.o clientframe.o clientgame.o `wx-config-2.8 --libs`

damaserver2008: servercom.o xmlcom.o tinyxml.o tinyxmlparser.o tinystr.o tinyxmlerror.o server.o
	$(CC) -g -o ../damaserver2008 servercom.o xmlcom.o tinyxml.o tinyxmlparser.o \
        tinystr.o tinyxmlerror.o server.o `wx-config-2.8 --libs`

clean:
	rm -f *.o
