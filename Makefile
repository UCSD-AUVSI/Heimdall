shell = /bin/sh

.suffixes:
.suffixes: .cpp .hpp .o

cc = g++
libs = -lpthread -lzmq
cflags = -g
std = -std=c++11

srcdir = ./src
bindir = ./bin
libdir = ./lib

includedir = $(srcdir)/include

backbonedir = $(srcdir)/backbone
displaydir = $(srcdir)/display 
grsaldir = $(srcdir)/grsaliency
guirecdir = $(srcdir)/guirecognition
guisaldir = $(srcdir)/guisaliency
imagepubdir = $(srcdir)/imagepub
imagerydir = $(srcdir)/imagery
orthorectdir = $(srcdir)/orthorectification
pubdir = $(srcdir)/publish
recdir = $(srcdir)/recognition
shareddir = $(srcdir)/shared
targetpubdir = $(srcdir)/targetpub
verifdir = $(srcdir)/verification

all: zServer zDistWorker zPushWorker

zServer: $(backbonedir)/server.cpp 
	$(cc) 

zDistWorker: 
	$(cc) 

zPushWorker:
	$(cc)

display.o: $(displaydir)/*.cpp $(includedir)/*.hpp
	$(cc) $(displaydir)/*.cpp -c $@	

clean:
	rm -rf $(bindir)
