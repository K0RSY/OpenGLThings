CC=g++
INPUT=./main.cpp ./libs/glad.c
OUTPUT=./exc

FLAGS=-lSDL2 -lGL

RMCOMMAND=rm -f

.PHONY: all run clean compile

all: clean
	$(CC) -o $(OUTPUT) $(INPUT) $(FLAGS)

run: all
	$(OUTPUT)

clean:
	$(RMCOMMAND) $(OUTPUT)
