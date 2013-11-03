LIBS = -framework GLUT -framework OpenGL

CC = gcc
CFLAGS = $(INCLUDE) -DOPENGL -DCOCOA

editor.o: lensbar.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c lensbar.c -o editor.o
editormain.o: main.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c main.c -o editormain.o
editordict.o: dict.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c dict.c -o editordict.o
editor: editor.o editormain.o search.o graphics.o textbox.o editordict.o
	$(CC) -o editor editormain.o search.o graphics.o \
		textbox.o editordict.o editor.o $(LIBS)
