DEP = mp3.cc mp3.hh

all: mp3client mp3server

mp3client: mp3client.o
	g++ mp3client.o -lpthread -o mp3client

mp3server: mp3server.o
	g++ mp3server.o -lpthread -o mp3server

mp3client.o: mp3client.cc mp3client.hh $(DEP)
	g++ -c -g mp3client.cc

mp3server.o: mp3server.cc mp3server.hh $(DEP)
	g++ -c -g mp3server.cc


clean: 
	rm -f *.o mp3client mp3server
