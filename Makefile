CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra -Werror -Wno-unused-parameter

# Режим сборки (по умолчанию debug)
MODE ?= debug

# Настройка флагов в зависимости от режима
ifeq ($(MODE),release)
    CFLAGS += -O2
    BUILD_DIR = build/release
else
    CFLAGS += -O0 -g3
    BUILD_DIR = build/debug
endif

CHILD = src/child.c
_PARENT = parent.c
PARENT = $(patsubst %,src/%,$(_PARENT))

# Цели по умолчанию
all: child parent

# Создаем каталог для сборки
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Правила для сборки
child: $(CHILD) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^

parent: $(PARENT) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@ $^

.PHONY: run
run: clean all
	CHILD_PATH=$(BUILD_DIR)/child $(BUILD_DIR)/parent "env.txt"

.PHONY: clean
clean:
	rm -rf build

.PHONY: all
