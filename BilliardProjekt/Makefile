# Operációs rendszer detektálása
ifeq ($(OS),Windows_NT)
    # Windows (CMD/PowerShell) beállítások
    # A 'subst' azért kell, mert a Windows mkdir nem szereti a perjelet (/)
    MKDIR = if not exist $(subst /,\,$1) mkdir $(subst /,\,$1)
    RM = if exist $(subst /,\,$1) rmdir /s /q $(subst /,\,$1)
    COPY_DIR = xcopy /E /I /Y
    EXE = .exe
else
    # Linux / macOS / Git Bash beállítások
    MKDIR = mkdir -p $1
    RM = rm -rf $1
    COPY_DIR = cp -r
    EXE =
endif

CC = gcc
CFLAGS = -I$(INCDIR) -Wall -O2
LIBS = -Llib -lglfw3 -lglu32 -lopengl32 -lgdi32 -lwinmm

SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
ASSETDIR = assets

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/billiard$(EXE)

all: $(TARGET) copy_assets

# 1. Linkelés
$(TARGET): $(OBJECTS)
	@$(call MKDIR, $(BINDIR))
	$(CC) $(OBJECTS) -o $(TARGET) $(LIBS)

# 2. Fordítás (.c -> .o)
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@$(call MKDIR, $(OBJDIR))
	$(CC) $(CFLAGS) -c $< -o $@

# 3. Assetek másolása
copy_assets:
	@$(call MKDIR, $(BINDIR)/assets)
	@$(COPY_DIR) $(ASSETDIR) $(subst /,\,$(BINDIR)/assets)

clean:
	@$(call RM, $(OBJDIR))
	@$(call RM, $(BINDIR))

run: all
	cd $(BINDIR) && .$(subst /,\,/billiard$(EXE))

.PHONY: all clean run copy_assets