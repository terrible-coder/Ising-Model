SIM_BIN=Ising_sim
GPH_BIN=Ising_draw

CODE_SIM=./src ./src/io ./apps/simulation
CODE_GPH=./apps/analysis
INCDIRS=./include ./include/* ./include/lib
LIBDIRS=./lib
OBJDIR=./bin

LIBS=$(foreach D,$(LIBDIRS),-L$(D))
INCS=$(foreach D,$(INCDIRS),-I$(D))

CC=c++
DEPFLAGS=-MP -MD
CFLAGS=-Wall -std=c++17 $(INCS) $(LIBS) $(DEPFLAGS)

GLIBS=-lmcmc -lising -lsfml-graphics -lsfml-window -lsfml-system
GCFILES=$(foreach D,$(CODE_GPH),$(wildcard $(D)/*.cpp))
GOBJECTS=$(subst ./src/,, $(patsubst %.cpp,$(OBJDIR)/%.o,$(GCFILES)))
GDEPFILES=$(subst ./src/,, $(patsubst %.cpp,$(OBJDIR)/%.d,$(GCFILES)))

SLIBS=-lmcmc -lising
SCFILES=$(foreach D,$(CODE_SIM),$(wildcard $(D)/*.cpp))
SOBJECTS=$(subst ./src/,, $(patsubst %.cpp,$(OBJDIR)/%.o,$(SCFILES)))
SDEPFILES=$(subst ./src/,, $(patsubst %.cpp,$(OBJDIR)/%.d,$(SCFILES)))

sim: $(SOBJECTS)
	@echo -n "Compiling \033[0;36msim binary\033[0m"
	@$(CC) $^ $(LIBS) $(SLIBS) -O3 -o $(SIM_BIN)
	@echo "\t\033[1;32m√\033[0m"

draw: $(GOBJECTS)
	@echo -n "Compiling \033[0;36mrender binary\033[0m"
	@$(CC) $^ $(LIBS) $(GLIBS) -o $(GPH_BIN)
	@echo "\t\033[1;32m√\033[0m"

$(OBJDIR)/%.o: src/%.cpp
	@echo -n "Compiling \033[0;36m$<\033[0m"
	@$(CC) $(CFLAGS) -c -O3 -o $@ $<
	@echo "\t\033[1;32m√\033[0m"

bin/./apps/simulation/sim.o: ./apps/simulation/sim.cpp
	@$(CC) $(CFLAGS) -c -O3 -o $@ $^

bin/./apps/analysis/render.o: ./apps/analysis/render.cpp
	@$(CC) $(CFLAGS) -c -O3 -o $@ $^

-include $(DEPFILES)

clean:
	@rm $(SOBJECTS) $(GOBJECTS) $(SDEPFILES) $(GDEPFILES) $(SIM_BIN) $(GPH_BIN) null.d .vscode/*.log

# This does not work, export the variable in the shell separately
# env:
# 	@echo "Setting environment variable"
# 	export LD_LIBRARY_PATH=$(value LD_LIBRARY_PATH):./lib;
# 	@echo "$(value LD_LIBRARY_PATH)"
