LIBS = -framework GLUT -framework OpenGL

CC = gcc
CFLAGS = $(INCLUDE) -DOPENGL -DCOCOA

editor: editor.o editormain.o search.o graphics.o textbox.o editordict.o
	$(CC) -o editor editormain.o search.o graphics.o \
		textbox.o editordict.o editor.o $(LIBS)
editor.o: lensbar.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c lensbar.c -o editor.o
editormain.o: main.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c main.c -o editormain.o
editordict.o: dict.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c dict.c -o editordict.o

yahoo: yahoo.o yahoomain.o search.o graphics.o textbox.o yahoodict.o
	$(CC) -o yahoo yahoomain.o search.o graphics.o \
		textbox.o yahoodict.o yahoo.o $(LIBS)
yahoo.o: lensbar.c
	$(CC) $(CFLAGS) -DYAHOO -g -c lensbar.c -o yahoo.o
yahoomain.o: main.c
	$(CC) $(CFLAGS) -DYAHOO -g -c main.c -o yahoomain.o
yahoodict.o: dict.c
	$(CC) $(CFLAGS) -DYAHOO -g -c dict.c -o yahoodict.o


push:
	git push
