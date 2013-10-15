CC = g++
LIBS = -lpthread -lzmq
DEPS = Backbone.hpp Orthorect.hpp GeoRef.hpp Saliency.hpp Segmentation.hpp Rec.hpp Verif.hpp
ALGS = Orthorect.cpp GeoRef.cpp Saliency.cpp Segmentation.cpp Rec.cpp Verif.cpp 
GDBLIBS = -g
CFLAGS = -o
STD = -std=c++11

all: zServer zDistWorker zPushWorker

debug: dServer dDistWorker dPushWorker

zServer: Server.cpp BackStore.cpp BackStore.hpp $(DEPS)
	$(CC) Server.cpp BackStore.cpp $(CFLAGS) $@ $(LIBS) $(STD)

zDistWorker: DistWorker.cpp $(DEPS) 
	$(CC) DistWorker.cpp $(ALGS) $(CFLAGS) $@ $(LIBS) $(STD)

zPushWorker: PushWorker.cpp $(DEPS)
	$(CC) PushWorker.cpp $(ALGS) $(CFLAGS) $@ $(LIBS) $(STD)

dServer: Server.cpp $(DEPS) 
	$(CC) Server.cpp $(CFLAGS) $@ $(LIBS) $(STD) $(GDBLIBS)

dDistWorker: DistWorker.cpp $(DEPS) 
	$(CC) DistWorker.cpp $(ALGS) $(CFLAGS) $@ $(LIBS) $(STD) $(GDBLIBS)

dPushWorker: PushWorker.cpp $(DEPS)
	$(CC) PushWorker.cpp $(ALGS) $(CFLAGS) $@ $(LIBS) $(STD) $(GDBLIBS)

clean:
	rm -f zServer zDistWorker zPushWorker dServer dDistWorker dPushWorker
