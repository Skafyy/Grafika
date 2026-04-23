CC = gcc

INCDIR = /mingw64/include
LIBDIR = /mingw64/lib
SRCDIR = src
BINDIR = bin

CFLAGS = -I$(INCDIR) -Wall -O2

LIBS = -L$(LIBDIR) -lglfw3 -lglu32 -lopengl32 -lgdi32 -lwinmm

SOURCES = $(SRCDIR)/main.c \
          $(SRCDIR)/physics.c \
          $(SRCDIR)/input.c \
          $(SRCDIR)/renderer.c

TARGET = $(BINDIR)/billiard.exe

all: $(TARGET)

$(TARGET): $(SOURCES)
	@if [ ! -d "$(BINDIR)" ]; then mkdir $(BINDIR); fi
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS) -o $(TARGET)

clean:
	@if [ -d "$(BINDIR)" ]; then rm -rf $(BINDIR); fi

run: all
	./$(TARGET)