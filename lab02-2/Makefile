# Компилятор и флаги
CC = gcc
CFLAGS = -std=c11 -pedantic -Wall -Wextra -Werror -Wno-unused-parameter

# Исходные файлы
CHILD = src/child.c
PARENT = src/parent.c

# Каталоги сборки
BUILD_DIR = build
DEBUG_DIR = $(BUILD_DIR)/debug
RELEASE_DIR = $(BUILD_DIR)/release

# Режим сборки (debug по умолчанию)
MODE ?= debug

ifeq ($(MODE), release)
    OUT_DIR = $(RELEASE_DIR)
    CFLAGS += -O2
else
    OUT_DIR = $(DEBUG_DIR)
    CFLAGS += -g
endif

# Исполняемые файлы
CHILD_BIN = $(OUT_DIR)/child
PARENT_BIN = $(OUT_DIR)/parent
ENV_FILE = $(OUT_DIR)/env.txt

# Создание директорий
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Сборка исполняемых файлов
$(CHILD_BIN): $(CHILD) | $(OUT_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(PARENT_BIN): $(PARENT) | $(OUT_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Основное правило сборки
.PHONY: all
all: $(CHILD_BIN) $(PARENT_BIN)
	@echo "Build complete. Binaries in $(OUT_DIR)"

# Запуск программы
.PHONY: run
run: all
	echo "ENV_FILE=env.txt" > $(ENV_FILE)
	CHILD_PATH=$(CHILD_BIN) $(PARENT_BIN) $(ENV_FILE)

# Очистка сборки
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
