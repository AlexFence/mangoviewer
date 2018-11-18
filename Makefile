SHELL = /usr/bin/env bash

SRC = $(shell find ./src -name '*.c')
HEADERS = $(shell find ./src -name '*.h')
OBJ = $(shell find ./src -name '*.c' | sed -e s/src/build/g | sed -e "s/\.c/.o/g")

CSS = $(shell find ./src -name "*.css")
CSSHEADERS = $(shell find ./src -name "*.css" | sed -e "s/\.css/_style.h/g")

CFLAGS = $(shell pkg-config --cflags  gtk+-3.0 glib-2.0)
LIBS = $(shell pkg-config --libs  gtk+-3.0 glib-2.0) -lmango


default:  build

build: $(CSSHEADERS) mangoviewer

build/%.o: src/%.c $(HEADERS)
	mkdir -p build
	gcc -c -o $@ $< $(CFLAGS)

src/%_style.h: src/%.css
	xxd -i $< >> $@

mangoviewer: $(OBJ) 
	gcc -o $@ $^ $(CFLAGS) $(LIBS) 


.PHONY: clean

clean:
	rm -rf ./build
	rm -rf $(CSSHEADERS)
	rm -f mangoviewer
