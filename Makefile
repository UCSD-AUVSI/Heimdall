CC = g++
LIBS = -lpthread -lzmq
ALGDEPS = Algorithm.hpp  Algs.hpp Orthorect.hpp GeoRef.hpp Saliency.hpp Segmentation.hpp ShapeSegmentation.hpp CharacterSegmentation.hpp ShapeRecognition.hpp CharacterRecognition.cpp Verif.hpp
COMMONDEPS = Backbone.hpp IMGData.hpp Algorithm.hpp
SERVERDEPS = BackStore.hpp
SERVERSRCS = BackStore.cpp
COMMONSRCS = IMGData.cpp Backbone.cpp
ALGS =  Orthorect.cpp GeoRef.cpp Saliency.cpp Segmentation.cpp ShapeSegmentation.cpp CharacterSegmentation.cpp ShapeRecognition.cpp CharacterRecognition.cpp Verif.cpp 
GDBLIBS = -g
CFLAGS = -o
STD = -std=c++11

all: zServer zDistWorker zPushWorker

debug: dServer dDistWorker dPushWorker

zServer: Server.cpp $(ALGS) $(ALGDEPS) $(SERVERSRCS) $(COMMONSRCS) $(SERVERDEPS) $(COMMONDEPS)
	$(CC) Server.cpp $(ALGS) $(COMMONSRCS) $(SERVERSRCS) $(CFLAGS) $@ $(LIBS) $(STD)

zDistWorker: DistWorker.cpp $(ALGS) $(COMMONSRCS) $(COMMONDEPS) $(ALGDEPS) 
	$(CC) DistWorker.cpp $(ALGS) $(COMMONSRCS) $(CFLAGS) $@ $(LIBS) $(STD)

zPushWorker: PushWorker.cpp $(ALGS) $(ALGDEPS) $(COMMONSRCS) $(COMMONDEPS)
	$(CC) PushWorker.cpp $(ALGS) $(COMMONSRCS) $(CFLAGS) $@ $(LIBS) $(STD)


dServer: Server.cpp $(ALGS) $(ALGDEPS) $(SERVERSRCS) $(COMMONSRCS) $(SERVERDEPS) $(COMMONDEPS)
	$(CC) Server.cpp $(ALGS) $(COMMONSRCS) $(SERVERSRCS) $(CFLAGS) $@ $(LIBS) $(STD) $(GDBLIBS)

dDistWorker: DistWorker.cpp $(ALGS) $(COMMONSRCS) $(COMMONDEPS) $(ALGDEPS) 
	$(CC) DistWorker.cpp $(ALGS) $(COMMONSRCS) $(CFLAGS) $@ $(LIBS) $(STD) $(GDBLIBS)

dPushWorker: PushWorker.cpp $(ALGS) $(ALGDEPS) $(COMMONSRCS) $(COMMONDEPS)
	$(CC) PushWorker.cpp $(ALGS) $(COMMONSRCS) $(CFLAGS) $@ $(LIBS) $(STD) $(GDBLIBS)


clean:
	rm -f *.o zServer zDistWorker zPushWorker dServer dDistWorker dPushWorker
