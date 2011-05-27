all: main.o
	gcc -o tsanalyzer main.o

clean:
	rm -fv tsanalyzer main.o
