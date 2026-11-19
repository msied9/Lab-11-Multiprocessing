CC=gcc
CFLAGS=-c -Wall -g
LDFLAGS=-ljpeg
SOURCES= mandel.c jpegrw.c 
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=mandel
MOVIE=mandelmovie
MOVIE_OBJ=mandelmovie.o

all: $(SOURCES) $(EXECUTABLE) $(MOVIE)

# pull in dependency info for *existing* .o files
-include $(OBJECTS:.o=.d) $(MOVIE_OBJ:.o=.d)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(MOVIE): $(MOVIE_OBJ)
	$(CC) $(MOVIE_OBJ) -lm -o $@

.c.o: 
	$(CC) $(CFLAGS) $< -o $@
	$(CC) -MM $< > $*.d

clean:
	rm -rf $(OBJECTS) $(MOVIE_OBJ) $(EXECUTABLE) $(MOVIE) *.d