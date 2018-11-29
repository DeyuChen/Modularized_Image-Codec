CC = g++
CFLAGS = -c -std=c++11
LDFLAGS = `pkg-config --cflags --libs opencv`
SOURCES = main.cpp dct_transformer.cpp matrix_quantizer.cpp
OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = a.out

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o $(EXECUTABLE)
