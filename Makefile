LIBS = -framework GLUT -framework OpenGL

CC = gcc
CFLAGS = $(INCLUDE) -DOPENGL -DCOCOA

editor: src/editor.o src/editormain.o src/search.o src/graphics.o src/textbox.o src/editordict.o
	$(CC) -o editor src/editormain.o src/search.o src/graphics.o \
		src/textbox.o src/editordict.o src/editor.o $(LIBS)
src/editor.o: src/lensbar.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c src/lensbar.c -o src/editor.o

src/editormain.o: src/main.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c src/main.c -o src/editormain.o
src/editordict.o: src/dict.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c src/dict.c -o src/editordict.o

yahoo: src/yahoo.o src/yahoomain.o src/search.o src/graphics.o src/textbox.o src/yahoodict.o
	$(CC) -o yahoo src/yahoomain.o src/search.o src/graphics.o \
		src/textbox.o src/yahoodict.o src/yahoo.o $(LIBS)
src/yahoo.o: src/lensbar.c
	$(CC) $(CFLAGS) -DYAHOO -g -c src/lensbar.c -o src/yahoo.o
src/yahoomain.o: src/main.c
	$(CC) $(CFLAGS) -DYAHOO -g -c src/main.c -o src/yahoomain.o
src/yahoodict.o: src/dict.c
	$(CC) $(CFLAGS) -DYAHOO -g -c src/dict.c -o src/yahoodict.o


push:
	git push

#src/%.o : src/%.c
#	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@
