CC=g++
INPUT=./main.cpp ./libs/glad.c
OUTPUT=./exc

FLAGS=-lSDL2 -lGL

RMCOMMAND=rm -f

.PHONY: all run clean full

all: clean
	$(CC) -o $(OUTPUT) $(INPUT) $(FLAGS)

run:
	$(OUTPUT)

full: all run

clean:
	$(RMCOMMAND) $(OUTPUT)
