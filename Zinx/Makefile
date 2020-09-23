TARGET=libzinx.so

$(TARGET):*.cpp
	g++ -std=c++11 -fPIC -shared $^ -o $@
	strip $@

install:$(TARGET)
	cp $^ /usr/lib
	cp *.h /usr/include