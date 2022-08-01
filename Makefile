SIM_BIN=Ising_sim
GPH_BIN=Ising_draw

CODE_SIM=./src ./src/Ising ./src/io ./apps/simulation
CODE_GPH=./src ./src/Ising ./src/io ./src/graphics ./apps/analysis
INCDIRS=./include ./include/*
OBJDIR=./bin

CC=c++
DEPFLAGS=-MP -MD
CFLAGS=-Wall -std=c++17 $(foreach D,$(INCDIRS),-I$(D)) $(DEPFLAGS)

GLIBS=-lsfml-graphics -lsfml-window -lsfml-system
GCFILES=$(foreach D,$(CODE_GPH),$(wildcard $(D)/*.cpp))
GOBJECTS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(GCFILES))
GDEPFILES=$(patsubst %.cpp,$(OBJDIR)/%.d,$(GCFILES))

SLIBS=
SCFILES=$(foreach D,$(CODE_SIM),$(wildcard $(D)/*.cpp))
SOBJECTS=$(patsubst %.cpp,$(OBJDIR)/%.o,$(SCFILES))
SDEPFILES=$(patsubst %.cpp,$(OBJDIR)/%.d,$(SCFILES))

sim: $(SOBJECTS)
	$(CC) $^ $(SLIBS) -o $(SIM_BIN)

draw: $(GOBJECTS)
	$(CC) $^ $(GLIBS) -o $(GPH_BIN)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(DEPFILES)

clean:
	@rm $(SOBJECTS) $(GOBJECTS) $(SDEPFILES) $(GDEPFILES) $(SIM_BIN) $(GPH_BIN) null.d
