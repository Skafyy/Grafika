CC = gcc
CFLAGS = -I$(INCDIR) -Wall -O2
LIBS = -Llib -lglfw3 -lglu32 -lopengl32 -lgdi32 -lwinmm

SRCDIR = src
INCDIR = include
# Új mappák a rendszerezéshez
OBJDIR = obj
BINDIR = bin
ASSETDIR = assets

# Fájlok listázása
SOURCES = $(wildcard $(SRCDIR)/*.c)
# Az objektum fájlok most már az /obj mappába kerülnek
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/billiard.exe

all: $(TARGET) copy_assets

# 1. Linkelés
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

# 2. Fordítás (.c -> .o) az /obj mappába
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 3. Assetek másolása (hogy a bin/assets mindig friss legyen)
copy_assets:
	@mkdir -p $(BINDIR)/assets
	@cp -r $(ASSETDIR)/* $(BINDIR)/assets/ 2>/dev/null || xcopy /E /I /Y $(ASSETDIR) bin\assets

clean:
	rm -rf $(OBJDIR) $(BINDIR)

run: all
	cd $(BINDIR) && ./billiard.exe

.PHONY: all clean run copy_assets