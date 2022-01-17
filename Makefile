CC = gcc
SRC = src/*.c
OBJ = *.o
OUT = editor
CONF = .editor

LIBS = -lSDL2main -lSDL2 -lSDL2_ttf

build:
	$(CC) -c $(SRC)
	$(CC) -o $(OUT) $(OBJ) $(LIBS)

	rm *.o

install:
	$(MAKE) build
	cp -r $(CONF) ~/.config/
	sudo mv $(OUT) /usr/bin/

uninstall:
	rm -r ~/.config/$(CONF)
	sudo rm /usr/bin/$(OUT)
