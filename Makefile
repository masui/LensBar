LIBS = -framework GLUT -framework OpenGL

CC = gcc
CFLAGS = $(INCLUDE) -DOPENGL -DCOCOA

EDITOROBJ = src/editormain.o src/search.o src/graphics.o src/textbox.o src/editordict.o src/editor.o

editor: $(EDITOROBJ)
	$(CC) -o editor $(EDITOROBJ) $(LIBS)
src/editor.o: src/lensbar.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c src/lensbar.c -o src/editor.o
src/editormain.o: src/main.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c src/main.c -o src/editormain.o
src/editordict.o: src/dict.c
	$(CC) $(CFLAGS) -DCPROGRAM -g -c src/dict.c -o src/editordict.o
editorapp: editor
	-/bin/rm -r -f Editor.app
	mkdir Editor.app
	mkdir Editor.app/Contents
	echo '<?xml version="1.0" encoding="UTF-8"?>' > Editor.app/Contents/Info.plist
	echo '<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">' >> Editor.app/Contents/Info.plist
	echo '<plist version="1.0">' >> Editor.app/Contents/Info.plist
	echo '  <dict>' >> Editor.app/Contents/Info.plist
	echo '    <key>CFBundleExecutable</key>' >> Editor.app/Contents/Info.plist
	echo '    <string>editor</string>' >> Editor.app/Contents/Info.plist
	echo '    <key>CFBundleIconFile</key>' >> Editor.app/Contents/Info.plist
	echo '    <string>icon.icns</string>' >> Editor.app/Contents/Info.plist
	echo '    <key>CFBundlePackageType</key>' >> Editor.app/Contents/Info.plist
	echo '    <string>APPL</string>' >> Editor.app/Contents/Info.plist
	echo '    <key>CFBundleSignature</key>' >> Editor.app/Contents/Info.plist
	echo '    <string>????</string>' >> Editor.app/Contents/Info.plist
	echo '  </dict>' >> Editor.app/Contents/Info.plist
	echo '</plist>' >> Editor.app/Contents/Info.plist
	mkdir Editor.app/Contents/MacOS
	cp editor Editor.app/Contents/MacOS
	mkdir Editor.app/Contents/Resources
	mkdir Editor.app/Contents/Resources/data
	cp data/lensbar.txt Editor.app/Contents/Resources/data
	cp res/editor.icns Editor.app/Contents/Resources/icon.icns

editordmg: editorapp
	/bin/rm -f Editor.dmg
	hdiutil create -srcfolder Editor.app -volname Editor Editor.dmg

YAHOOOBJ = src/yahoo.o src/yahoomain.o src/search.o src/graphics.o src/textbox.o src/yahoodict.o
yahoo: $(YAHOOOBJ)
	$(CC) -o yahoo $(YAHOOOBJ) $(LIBS)
src/yahoo.o: src/lensbar.c
	$(CC) $(CFLAGS) -DYAHOO -g -c src/lensbar.c -o src/yahoo.o
src/yahoomain.o: src/main.c
	$(CC) $(CFLAGS) -DYAHOO -g -c src/main.c -o src/yahoomain.o
src/yahoodict.o: src/dict.c
	$(CC) $(CFLAGS) -DYAHOO -g -c src/dict.c -o src/yahoodict.o


push:
	git push

clean:
	-/bin/rm src/*.o
	-/bin/rm -r *.app
	-/bin/rm *.dmg
	-/bin/rm editor
