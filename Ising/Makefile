CODE_DIR=./src
INCDIR=./include ../include/ ../include/lib
OBJ_DIR=./bin

CC=c++
DEPFLAGS=-MD -MP
CPPFLAGS=-Wall -std=c++17 $(foreach D,$(INCDIR),-I$(D)) $(DEPFLAGS)
LIBS=
CPPFILES=$(foreach D,$(CODE_DIR),$(wildcard $(D)/*.cpp))
OBJFILES=$(subst ./src/,,$(patsubst %.cpp,$(OBJ_DIR)/%.o,$(CPPFILES)))
DEPFILES=$(subst ./src/,,$(patsubst %.cpp,$(OBJ_DIR)/%.d,$(CPPFILES)))

MAJVER=1
MINVER=0
PCHVER=0
LIBNAME=libising.so
SO_NAME=libising.so.$(MAJVER)
OG_NAME=libising.so.$(MAJVER).$(MINVER).$(PCHVER)

$(LIBNAME): $(SO_NAME)
	@echo -n "Making library \033[0;36m$@\033[0"
	@if [ ! -f "../lib/$@" ]; then \
		ln -s $^ ../lib/$@; \
	fi
	@echo "\t\033[1;32m√\033[0m"

$(SO_NAME): $(OG_NAME)
	@cp $^ ../lib/$^
	@echo -n "Making sym link \033[0;36m$@\033[0m"
	@if [ ! -f "../lib/$@" ]; then \
		ln -s $^ ../lib/$@; \
		if [ -f "../lib/$(LIBNAME)" ]; then \
			rm "../lib/$(LIBNAME)"; \
		fi \
	fi
	@echo "\t\033[1;32m√\033[0m"

$(OG_NAME): $(OBJFILES)
	@echo -n "Generating \033[0;36m$@\033[0m"
	$(CC) -fpic -o $@ $^ -shared -Wl,-soname,$(SO_NAME)
	@echo "\t\033[1;32m√\033[0m"

$(OBJ_DIR)/%.o: src/%.cpp
	@echo -n "Compiling \033[0;36m$<\033[0m"
	@$(CC) $(CPPFLAGS) -fpic -c -O3 -o $@ $<
	@echo "\t\033[1;32m√\033[0m"

copy:
	cp ./include/* ../include/lib/ -r

-include $(DEPFILES)
