BIN=Ising_draw

CODEDIR=./apps/analysis
INCDIRS=./include ./include/*
LIBDIRS=./lib
OBJDIR=./bin

LFLAGS=$(foreach D,$(LIBDIRS),-L$(D))
IFLAGS=$(foreach D,$(INCDIRS),-I$(D))

CC=c++
DEPFLAGS=-MP -MD
CPPFLAGS=-Wall -std=c++17 $(IFLAGS) $(LFLAGS) $(DEPFLAGS)

LIBS=-lmcmc -lising -lsfml-graphics -lsfml-window -lsfml-system
CPPFILES=$(foreach D,$(CODEDIR),$(wildcard $(D)/*.cpp))
OBJECTS=$(subst ./src/,, $(patsubst %.cpp,$(OBJDIR)/%.o,$(CPPFILES)))
DEPFILES=$(subst ./src/,, $(patsubst %.cpp,$(OBJDIR)/%.d,$(CPPFILES)))

$(BIN): $(OBJECTS)
	@echo -n "Generating \033[0;36mdraw binary\033[0m"
	@$(CC) $^ $(LIBS) $(LFLAGS) -O3 -o $(BIN)
	@echo "\t\033[1;32m√\033[0m"

$(OBJDIR)/%.o: ./src/%.cpp
	@echo -n "Compiling \033[0;36m$<\033[0m"
	@$(CC) $(CPPFLAGS) -c -O3 -o $@ $<
	@echo "\t\033[1;32m√\033[0m"

-include $(DEPFILES)

clean:
	rm $(BIN) $(OBJECTS) $(DEPFILES)
