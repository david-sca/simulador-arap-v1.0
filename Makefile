# Makefile wrapper for waf

all:
	./waf

# free free to change this part to suit your requirements
configure:
	./waf --build-profile=optimized --enable-examples --enable-tests --out=build/optimized configure

PARAMETROS = parametros.txt
SHELL=/bin/bash
RUNS = 1
run:
	@n=0; \
	RANDOM=1; \
	while [ $${n} -lt $(RUNS) ]; \
	do \
		n=`expr $$n + 1`; \
		run=$$RANDOM; \
		date; \
		echo "Ejecucion $$n. Valor de RUN: $$run"; \
		./waf --cwd="arap-dir" --run="scratch/simulador $$run $(PARAMETROS)" 2>arap-dir/arap-logs-$$run.log; \
	done; \

debug:
	./waf --build-profile=debug --enable-examples --enable-tests --out=build/debug configure

.PHONY: build
build:
	./waf build

install:
	./waf install

clean:
	./waf clean

distclean:
	./waf distclean
