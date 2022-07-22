BINARY=Ising_model
CODEDIRS=. ./src ./src/*
INCDIRS=./include ./include/*
OBJDIR=./bin

CC=c++
LIBS=-lsfml-graphics -lsfml-window -lsfml-system
DEPFLAGS=-MP -MD
CFLAGS=-Wall $(foreach D,$(INCDIRS),-I$(D)) $(DEPFLAGS)
CFILES=$(foreach D,$(CODEDIRS),$(wildcard $(D)/*.cpp))
OBJECTS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(CFILES))
DEPFILES=$(patsubst %.cpp,$(OBJDIR)/%.d,$(CFILES))

$(BINARY): $(OBJECTS)
	$(CC) $^ $(LIBS) -o $@

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEPFILES)

clean:
	@rm $(OBJECTS) $(DEPFILES) $(BINARY)
