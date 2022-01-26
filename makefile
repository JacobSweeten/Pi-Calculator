all: main.c
	gcc main.c -lm -pthread -o pi