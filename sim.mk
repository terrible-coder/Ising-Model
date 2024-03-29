BIN=Ising_sim

CODEDIR=./src ./src/io ./apps/simulation
INCDIRS=./include ./include/io ./include/lib /usr/local/openmpi/include
LIBDIRS=./lib
OBJDIR=./bin

LFLAGS=$(foreach D,$(LIBDIRS),-L$(D))
IFLAGS=$(foreach D,$(INCDIRS),-I$(D))

CC=mpic++
DEPFLAGS=-MP -MD
CPPFLAGS=-Wall -std=c++17 $(IFLAGS) $(LFLAGS) $(DEPFLAGS)

LIBS=-lmcmc -lising
CPPFILES=$(foreach D,$(CODEDIR),$(wildcard $(D)/*.cpp))
OBJECTS=$(subst ./src/,, $(patsubst %.cpp,$(OBJDIR)/%.o,$(CPPFILES)))
DEPFILES=$(subst ./src/,, $(patsubst %.cpp,$(OBJDIR)/%.d,$(CPPFILES)))

# hi:
# 	@echo $(OBJECTS)
#		@echo $(CPPFLAGS)

$(BIN): $(OBJECTS)
	@echo -n "Generating \033[0;36msim binary\033[0m"
	@$(CC) $^ $(LIBS) $(LFLAGS) -O3 -o $(BIN)
	@echo "\t\033[1;32m√\033[0m"

$(OBJDIR)/%.o: ./src/%.cpp
	@echo -n "Compiling \033[0;36m$<\033[0m"
	@$(CC) $(CPPFLAGS) -c -O3 -o $@ $<
	@echo "\t\033[1;32m√\033[0m"

bin/./apps/simulation/sim.o: apps/simulation/sim.cpp
	@echo -n "Compiling \033[0;36m$<\033[0m"
	@$(CC) $(CPPFLAGS) -c -O3 -o $@ $<
	@echo "\t\033[1;32m√\033[0m"

-include $(DEPFILES)

clean:
	rm $(BIN) $(OBJECTS) $(DEPFILES)
