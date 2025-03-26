CC = gcc
CFLAGS = -std=c11 -g2 -ggdb -pedantic -W -Wall -Wextra 

.SUFFIXES:
.SUFFIXES: .c .o

DEBUG   = build/debug
RELEASE = build/release
OUT_DIR = $(DEBUG)
OBJ_DIR = $(DEBUG)

ifeq ($(MODE), release)
  CFLAGS = -std=c11 -pedantic -W -Wall -Wextra -Wno-unused-parameter -Werror
  OUT_DIR = $(RELEASE)
  OBJ_DIR = $(RELEASE)
endif

SRC_DIR = src
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

parent = $(OUT_DIR)/parent
child = $(OUT_DIR)/child

all: $(parent) $(child) setenv

$(parent): $(OBJ_DIR)/parent.o
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/parent.o -o $@
	@echo "Compiled $(parent)"

$(child): $(OBJ_DIR)/child.o
	@mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/child.o -o $@
	@echo "Compiled $(child)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: setenv
setenv:
	@echo "Setting environment variables..."
	@if ! grep -q "export CHILD_PATH=" ~/.bashrc; then echo "export CHILD_PATH=$(OUT_DIR)/child" >> ~/.bashrc; fi
	@if ! grep -q "export ENV_PATH=" ~/.bashrc; then echo "export ENV_PATH=$(OUT_DIR)/env.txt" >> ~/.bashrc; fi
	@if ! grep -q "export LC_COLLATE=" ~/.bashrc; then echo "export LC_COLLATE=C" >> ~/.bashrc; fi
	@echo "Environment variables saved in ~/.bashrc. Run 'source ~/.bashrc' to apply changes."

.PHONY: unsetenv
unsetenv:
	@echo "Removing environment variables..."
	@sed -i '/export CHILD_PATH=/d' ~/.bashrc
	@sed -i '/export ENV_PATH=/d' ~/.bashrc
	@sed -i '/export LC_COLLATE=/d' ~/.bashrc
	@echo "Environment variables removed from ~/.bashrc. Run 'source ~/.bashrc' to apply changes."

.PHONY: clean
clean: unsetenv
	@rm -rf $(DEBUG) $(RELEASE)
	@echo "Cleaned build files."
