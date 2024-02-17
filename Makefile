INCLUDE = -lSDL2 -lSDL2_image -lSDL2_ttf -I/usr/include/SDL2 -lm
FILES = main.c intro.c minijeux/*.c

dev :
	gcc main.c -o main -Wall -Wextra $(INCLUDE)

clang :
	clang main.c -o main -Wall -Wextra $(INCLUDE)

release :
	gcc main.c -o main -O3 $(INCLUDE)

debug :
	gcc main.c -o main -g -Wall -Wextra -fsanitize=address $(INCLUDE)