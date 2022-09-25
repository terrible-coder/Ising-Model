INCDIRS=./include ./include/io ./include/lib

sim:
	@make -f sim.mk

draw:
	@make -f gph.mk

ising:
	@make -C Ising/

mcmc:
	@make -C MCMC/

copy:
	@make -C Ising/ copy
	@make -C MCMC/ copy

clean:
	@make -f sim.mk clean
	@make -f gph.mk clean

# This does not work, export the variable in the shell separately
# env:
# 	@echo "Setting environment variable"
# 	export LD_LIBRARY_PATH=$(value LD_LIBRARY_PATH):./lib;
# 	@echo "$(value LD_LIBRARY_PATH)"
