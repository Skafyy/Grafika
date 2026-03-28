CC = gcc

INCDIR = include
LIBDIR = lib
SRCDIR = src
BINDIR = bin

CFLAGS = -I$(INCDIR) -Wall -O2

LIBS = -L$(LIBDIR) -lglfw3 -lopengl32 -lglu32 -lgdi32 -lwinmm

SOURCES = $(SRCDIR)/main.c $(SRCDIR)/physics.c
TARGET = $(BINDIR)/billiard.exe

all: $(TARGET)

$(TARGET): $(SOURCES)
	if not exist $(BINDIR) mkdir $(BINDIR)
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS) -o $(TARGET)

clean:
	rm -rf $(BINDIR)/*.exe

run:
	./$(TARGET)