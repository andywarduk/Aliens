SRC=../../src
OBJ=Aliens.o StringWrap.o
DEBUG=-DDEBUG

all: Aliens.js Aliens.html

Aliens.js: $(OBJ)
	emcc $(DEBUG) -O2 $^ -o $@

Aliens.html: $(OBJ)
	emcc $(DEBUG) -O2 $^ -o $@

%.o: $(SRC)/%.cpp
	emcc $(DEBUG) -O2 $^ -o $@

clean:
	rm -f *.o *.html *.js*
