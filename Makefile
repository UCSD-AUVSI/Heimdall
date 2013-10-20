CC = g++
LIBS = -lpthread -lzmq
ALGDEPS = Publish.hpp GUISaliency.hpp GUIRec.hpp ImagePub.hpp Display.hpp Algorithm.hpp  Algs.hpp Orthorect.hpp GeoRef.hpp Saliency.hpp Segmentation.hpp ShapeSegmentation.hpp CharacterSegmentation.hpp ShapeRecognition.hpp CharacterRecognition.cpp Verif.hpp
COMMONDEPS = Backbone.hpp IMGData.hpp Algorithm.hpp
SERVERDEPS = BackStore.hpp
SERVERSRCS = BackStore.cpp
COMMONSRCS = IMGData.cpp Maps.cpp
ALGS = Publish.cpp GUISaliency.cpp GUIRec.cpp ImagePub.cpp Display.cpp Orthorect.cpp GeoRef.cpp Saliency.cpp Segmentation.cpp ShapeSegmentation.cpp CharacterSegmentation.cpp ShapeRecognition.cpp CharacterRecognition.cpp Verif.cpp 
GDBLIBS = -g
CFLAGS = -o
STD = -std=c++11

ifneq ($(OS),Windows_NT)
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		STDLIB = -stdlib=libc++
	endif
endif

all: zServer zDistWorker zPushWorker

debug: dServer dDistWorker dPushWorker

zServer: Server.cpp $(ALGS) $(ALGDEPS) $(SERVERSRCS) $(COMMONSRCS) $(SERVERDEPS) $(COMMONDEPS)
	$(CC) Server.cpp $(ALGS) $(COMMONSRCS) $(SERVERSRCS) $(CFLAGS) $@ $(LIBS) $(STD) $(STDLIB)

zDistWorker: DistWorker.cpp $(ALGS) $(COMMONSRCS) $(COMMONDEPS) $(ALGDEPS) 
	$(CC) DistWorker.cpp $(ALGS) $(COMMONSRCS) $(CFLAGS) $@ $(LIBS) $(STD) $(STDLIB)

zPushWorker: PushWorker.cpp $(ALGS) $(ALGDEPS) $(COMMONSRCS) $(COMMONDEPS)
	$(CC) PushWorker.cpp $(ALGS) $(COMMONSRCS) $(CFLAGS) $@ $(LIBS) $(STD) $(STDLIB)


dServer: Server.cpp $(ALGS) $(ALGDEPS) $(SERVERSRCS) $(COMMONSRCS) $(SERVERDEPS) $(COMMONDEPS)
	$(CC) Server.cpp $(ALGS) $(COMMONSRCS) $(SERVERSRCS) $(CFLAGS) $@ $(LIBS) $(STD) $(STDLIB) $(GDBLIBS)

dDistWorker: DistWorker.cpp $(ALGS) $(COMMONSRCS) $(COMMONDEPS) $(ALGDEPS) 
	$(CC) DistWorker.cpp $(ALGS) $(COMMONSRCS) $(CFLAGS) $@ $(LIBS) $(STD) $(STDLIB) $(GDBLIBS)

dPushWorker: PushWorker.cpp $(ALGS) $(ALGDEPS) $(COMMONSRCS) $(COMMONDEPS)
	$(CC) PushWorker.cpp $(ALGS) $(COMMONSRCS) $(CFLAGS) $@ $(LIBS) $(STD) $(STDLIB) $(GDBLIBS)


clean:
	rm -f *.o zServer zDistWorker zPushWorker dServer dDistWorker dPushWorker

