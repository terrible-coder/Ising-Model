CC=c++
LIBS=-lsfml-graphics -lsfml-window -lsfml-system

Ising_model: params.o Ising.o MC.o main.o
	$(CC) *.o $(LIBS) -o Ising_model

main.o: main.cpp Ising.hpp
	$(CC) -c main.cpp

Ising.o: Ising.cpp Ising.hpp
	$(CC) -c Ising.cpp

params.o: params.cpp params.hpp
	$(CC) -c params.cpp

MC.o: MC.cpp MC.hpp
	$(CC) -c MC.cpp

clean:
	rm *.o Ising_model
