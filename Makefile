CC=c++
LIBS=-lsfml-graphics -lsfml-window -lsfml-system

Ising_model: specs.o Ising.o MC.o main.o
	$(CC) *.o $(LIBS) -o Ising_model

main.o: main.cpp MC.hpp
	$(CC) -c main.cpp

Ising.o: Ising.cpp Ising.hpp
	$(CC) -c Ising.cpp

specs.o: specs.cpp specs.hpp
	$(CC) -c specs.cpp

MC.o: MC.cpp MC.hpp
	$(CC) -c MC.cpp

clean:
	rm *.o Ising_model
