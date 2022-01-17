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
	sudo mv $(OUT) /usr/bin/
	cp -r $(CONF) ~/.config/

uninstall:
	sudo rm /usr/bin/$(OUT)
	rm -r ~/.config/$(CONF)
