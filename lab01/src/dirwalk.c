#define _XOPEN_SOURCE 700  // Для использования POSIX-функций

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <locale.h>
#include <errno.h>

// Структура для хранения динамического массива путей файлов
typedef struct {
    char **paths;       // Массив строк (путей)
    size_t size;        // Текущее количество элементов
    size_t allocated;   // Выделенный размер массива
} FileArray;

// Структура для хранения опций фильтрации и сортировки
typedef struct {
    int symlink_only;   // Только символические ссылки
    int dir_only;       // Только каталоги
    int file_only;      // Только обычные файлы
    int sorted;         // Сортировать вывод
} FilterOptions;

// Добавляет новый путь в динамический массив FileArray
void append_path(FileArray *fa, const char *path) {
    // Если массив заполнен, увеличиваем его размер
    if (fa->size == fa->allocated) {
        size_t new_alloc = fa->allocated ? fa->allocated * 2 : 32;
        char **tmp = realloc(fa->paths, new_alloc * sizeof(char *));
        if (!tmp) {
            fprintf(stderr, "Ошибка выделения памяти\n");
            exit(EXIT_FAILURE);
        }
        fa->paths = tmp;
        fa->allocated = new_alloc;
    }
    // Копируем путь и добавляем в массив
    fa->paths[fa->size] = strdup(path);
    if (!fa->paths[fa->size]) {
        fprintf(stderr, "Ошибка копирования строки\n");
        exit(EXIT_FAILURE);
    }
    fa->size++;
}

// Освобождает память, выделенную под FileArray
void clear_file_array(FileArray *fa) {
    for (size_t i = 0; i < fa->size; i++)
        free(fa->paths[i]);
    free(fa->paths);
}

// Функция сравнения строк для сортировки с учетом локали
int locale_compare(const void *a, const void *b) {
    return strcoll(*(const char **)a, *(const char **)b);
}

// Проверяет, соответствует ли файл заданным фильтрам
int check_filter(const struct stat *st, const FilterOptions *opts) {
    // Если заданы фильтры, проверяем соответствие типу файла
    if (opts->symlink_only || opts->dir_only || opts->file_only) {
        if (opts->symlink_only && S_ISLNK(st->st_mode))
            return 1;
        if (opts->dir_only && S_ISDIR(st->st_mode))
            return 1;
        if (opts->file_only && S_ISREG(st->st_mode))
            return 1;
        return 0; // Не соответствует ни одному фильтру
    }
    return 1; // Если фильтры не заданы, подходит любой файл
}

// Рекурсивно обходит каталог и добавляет подходящие файлы в массив
void scan_directory(const char *dirpath, const FilterOptions *opts, FileArray *fa) {
    DIR *dp = opendir(dirpath);
    if (!dp) {
        fprintf(stderr, "Не удалось открыть каталог '%s': %s\n", dirpath, strerror(errno));
        return;
    }

    struct dirent *entry;
    char fullpath[PATH_MAX];
    struct stat st;

    while ((entry = readdir(dp))) {
        // Пропускаем текущий и родительский каталоги
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
            continue;

        // Формируем полный путь к файлу
        snprintf(fullpath, PATH_MAX, "%s/%s", dirpath, entry->d_name);

        // Получаем информацию о файле (не следуя по ссылкам)
        if (lstat(fullpath, &st) == -1) {
            fprintf(stderr, "Ошибка lstat для '%s': %s\n", fullpath, strerror(errno));
            continue;
        }

        // Если файл соответствует фильтрам, добавляем его в массив
        if (check_filter(&st, opts))
            append_path(fa, fullpath);

        // Если это каталог, рекурсивно обходим его содержимое
        if (S_ISDIR(st.st_mode))
            scan_directory(fullpath, opts, fa);
    }

    closedir(dp);
}

// Выводит справку по использованию программы
void print_usage(const char *progname) {
    fprintf(stderr, "Использование: %s [-l] [-d] [-f] [-s] [каталог]\n", progname);
    fprintf(stderr, "Опции:\n");
    fprintf(stderr, "  -l  только символические ссылки\n");
    fprintf(stderr, "  -d  только каталоги\n");
    fprintf(stderr, "  -f  только обычные файлы\n");
    fprintf(stderr, "  -s  сортировать вывод\n");
}

int main(int argc, char **argv) {
    // Устанавливаем локаль для корректной сортировки
    setlocale(LC_COLLATE, "");

    FilterOptions opts = {0};
    int opt;

    // Обрабатываем аргументы командной строки
    while ((opt = getopt(argc, argv, "ldfs")) != -1) {
        switch (opt) {
            case 'l': opts.symlink_only = 1; break;
            case 'd': opts.dir_only = 1; break;
            case 'f': opts.file_only = 1; break;
            case 's': opts.sorted = 1; break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Определяем начальный каталог (по умолчанию текущий)
    const char *start_dir = (optind < argc) ? argv[optind] : ".";

    FileArray fa = {0};

    struct stat st;
    // Проверяем начальный каталог отдельно
    if (lstat(start_dir, &st) == -1) {
        fprintf(stderr, "Ошибка lstat для '%s': %s\n", start_dir, strerror(errno));
        return EXIT_FAILURE;
    }

    // Если начальный каталог соответствует фильтрам, добавляем его
    if (check_filter(&st, &opts))
        append_path(&fa, start_dir);

    // Если это каталог, начинаем рекурсивный обход
    if (S_ISDIR(st.st_mode))
        scan_directory(start_dir, &opts, &fa);

    // Если задана сортировка, сортируем массив путей
    if (opts.sorted)
        qsort(fa.paths, fa.size, sizeof(char *), locale_compare);

    // Выводим все найденные пути
    for (size_t i = 0; i < fa.size; i++)
        puts(fa.paths[i]);

    // Освобождаем выделенную память
    clear_file_array(&fa);
    return EXIT_SUCCESS;
}
