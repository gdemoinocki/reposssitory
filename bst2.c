#include "bst.h"  // Подключение заголовочного файла bst.h, содержащего объявления структур и функций

// Функция для вывода узлов дерева в формате DOT (для визуализации дерева)
static void output_nodes_to_dotfile(tree_node_t *node, FILE *file, print_datatype_func print_data)
{
    if (!node)  // Если узел пустой, завершаем функцию
        return;
    
    // Вывод информации о текущем узле в формате DOT
    fprintf(file, "%zu [label=<<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\"><TR>\n", (size_t)node);
    if (node->left)  // Если есть левый потомок, добавляем порт для него
        fprintf(file, "<TD port=\"left\"><SUB><i>l</i></SUB></TD>\n"); 
    
    // Вывод данных узла
    fprintf(file, "<TD CELLPADDING=\"7\"><b>");
    print_data(file, node->data);  // Вызов функции для печати данных узла
    fprintf(file, "</b></TD>\n");
    
    if (node->right)  // Если есть правый потомок, добавляем порт для него
        fprintf(file, "<TD port=\"right\"><SUB><i>r</i></SUB></TD>\n");
    
    fprintf(file, "</TR></TABLE>>]\n");  // Завершение таблицы для узла
    
    // Добавление связей между узлами
    if (node->left)
        fprintf(file, "%zu:left -> %zu\n", (size_t)node, (size_t)node->left);
    if (node->right)
        fprintf(file, "%zu:right -> %zu\n", (size_t)node, (size_t)node->right);
    
    // Рекурсивный вызов для левого и правого поддеревьев
    output_nodes_to_dotfile(node->left, file, print_data);
    output_nodes_to_dotfile(node->right, file, print_data);
}

// Функция для визуализации дерева в формате DOT и создания PNG-изображения
void bst_render(bst_t *bst, print_datatype_func print_data, bool open)
{
    struct stat st = {0};
    const char *pics_folder = "./pics";  // Папка для сохранения изображений
    
    // Проверка существования папки и создание, если её нет
    if (stat(pics_folder, &st) == -1)
        if (mkdir(pics_folder, 0777) == -1)  // Создание папки с правами 0777
        {
            printf("Error: Failed to create folder '%s'\n", pics_folder);
            exit(EXIT_FAILURE);  // Выход с ошибкой, если не удалось создать папку
        }

    // Создание имени файла для DOT-файла
    char *filename = calloc(64, 1);
    sprintf(filename, "%s/bst_%zu",pics_folder, (size_t)bst);
    
    // Открытие файла для записи
    FILE *file = fopen(filename, "w");
    if (!file)  // Проверка успешности открытия файла
    {
        printf("Error: unable to open file '%s' for writing\n", filename);
        free(filename);
        exit(EXIT_FAILURE);  // Выход с ошибкой, если не удалось открыть файл
    }
    
    // Запись заголовка DOT-файла
    fprintf(file, "digraph bst_%zu {\nnode [shape=ellipse]\n", (size_t)bst);
    
    // Рекурсивный вывод узлов дерева в файл
    output_nodes_to_dotfile(bst->root, file, print_data);
    
    // Завершение DOT-файла
    fprintf(file, "}\n");
    fclose(file);  // Закрытие файла
    
    // Создание команды для генерации PNG-изображения из DOT-файла
    char *command = calloc(64, 1);
    sprintf(command, "dot -Tpng %s -o %s.png", filename, filename);
    
    // Выполнение команды для генерации PNG
    if (system(command) == -1) {
        fprintf(stderr, "Error executing command: %s\n", command);
        free(command);
        return;
    }
    
    remove(filename);  // Удаление временного DOT-файла
    free(command);  // Освобождение памяти, выделенной под команду
    
    // Если нужно открыть изображение
    if (open)
    {
        command = calloc(64, 1);
        sprintf(command, "xdg-open %s.png", filename);  // Команда для открытия PNG-файла
        if (system(command) == -1) {
            fprintf(stderr, "Error executing command: %s\n", command);
            free(command);
            return;
        }
        free(command);  // Освобождение памяти, выделенной под команду
    }
    free(filename);  // Освобождение памяти, выделенной под имя файла
}

// Функция для создания нового двоичного дерева поиска
bst_t *bst_create(size_t data_size) {
    bst_t *tree = (bst_t *)malloc(sizeof(bst_t));  // Выделение памяти под структуру дерева
    if (tree == NULL) {
        return NULL;  // Возврат NULL при неудачном выделении памяти
    }
    tree->root = NULL;  // Инициализация корня дерева как NULL
    tree->data_size = data_size;  // Установка размера данных
    return tree;  // Возвращение указателя на новое дерево
}

// Функция для проверки, пусто ли дерево
bool bst_empty(bst_t *tree) {
    return tree->root == NULL;  // Возвращает true, если корень дерева равен NULL
}

// Функция для проверки, содержит ли дерево элемент с данными data
bool bst_contains(bst_t *tree, void *data, comparator_func comp) {
    tree_node_t *current = tree->root;  // Начало с корня дерева
    while (current != NULL) {  // Продолжать, пока есть узлы для проверки
        int compare_result = comp(data, current->data);  // Сравнение данных
        if (compare_result < 0) {  // Если data меньше данных узла
            current = current->left;  // Перейти к левому поддереву
        } else if (compare_result > 0) {  // Если data больше данных узла
            current = current->right;  // Перейти к правому поддереву
        } else {
            return true;  // Найдено совпадение, возвращает true
        }
    }
    return false;  // Данные не найдены в дереве
}

// Вспомогательная функция для подсчета узлов в дереве
static size_t count_nodes(tree_node_t *node) {
    if (node == NULL) {
        return 0;  // Если узел не существует, возвращает 0
    }
    // Рекурсивно считает узлы в левом и правом поддереве, добавляя 1 за текущий узел
    return 1 + count_nodes(node->left) + count_nodes(node->right);
}

// Функция для получения количества узлов в дереве
size_t bst_length(bst_t *tree) {
    return count_nodes(tree->root);  // Возвращает количество узлов в дереве
}

// Рекурсивная функция для вставки нового узла в дерево
static tree_node_t *insert_node_recursive(tree_node_t *node, void *data, size_t data_size, comparator_func comp) {
    if (node == NULL) {  // Если узел не существует, создаем новый
        tree_node_t *new_node = (tree_node_t *)malloc(sizeof(tree_node_t));
        new_node->data = malloc(data_size);  // Выделение памяти под данные
        memcpy(new_node->data, data, data_size);  // Копирование данных
        new_node->left = NULL;  // Инициализация левого поддерева как NULL
        new_node->right = NULL;  // Инициализация правого поддерева как NULL
        return new_node;  // Возвращение нового узла
    }
    // Если значение данных меньше текущего узла, вставляем в левое поддерево
    if (comp(data, node->data) < 0) {
        node->left = insert_node_recursive(node->left, data, data_size, comp);
    } else {  // Иначе вставляем в правое поддерево
        node->right = insert_node_recursive(node->right, data, data_size, comp);
    }
    return node;  // Возвращение текущего узла после возможной вставки
}

// Функция для вставки узла в двоичное дерево поиска
void bst_insert_node(bst_t *tree, void *data, comparator_func comp) {
    // Вызов рекурсивной функции вставки с началом от корня дерева
    tree->root = insert_node_recursive(tree->root, data, tree->data_size, comp);
}

// Функция для поиска узла с минимальным значением, начиная от заданного узла
static tree_node_t *find_min(tree_node_t *node) {
    tree_node_t *current = node;
    // Перемещение к крайнему левому узлу
    while (current && current->left != NULL) {
        current = current->left;
    }
    return current;  // Возвращение узла с минимальным значением
}

// Рекурсивная функция для удаления узла из дерева
static tree_node_t *delete_node_recursive(tree_node_t *node, void *data, size_t data_size, comparator_func comp) {
    if (node == NULL) return NULL;  // Если узел отсутствует, ничего не делаем

    int compare_result = comp(data, node->data);
    // Определяем, в каком направлении продолжить поиск
    if (compare_result < 0) {  // Данные меньше данных узла
        node->left = delete_node_recursive(node->left, data, data_size, comp);
    } else if (compare_result > 0) {  // Данные больше данных узла
        node->right = delete_node_recursive(node->right, data, data_size, comp);
    } else {  // Найден узел для удаления
        // Если у узла нет левого поддерева, возвращаем правое
        if (node->left == NULL) {
            tree_node_t *temp = node->right;
            free(node->data);
            free(node);
            return temp;
        // Если у узла нет правого поддерева, возвращаем левое
        } else if (node->right == NULL) {
            tree_node_t *temp = node->left;
            free(node->data);
            free(node);
            return temp;
        }

        // Если у узла есть оба поддерева, находим минимум в правом поддереве
        tree_node_t *temp = find_min(node->right);

        // Копируем данные минимального узла в текущий узел
        memcpy(node->data, temp->data, data_size);

        // Удаляем узел, который теперь дублируется
        node->right = delete_node_recursive(node->right, temp->data, data_size, comp);
    }
    return node;  // Возвращаем текущий узел после возможного удаления
}

// Функция для удаления узла из дерева
void bst_delete_node(bst_t *tree, void *data, comparator_func comp) {
    // Вызов рекурсивной функции удаления с началом от корня дерева
    tree->root = delete_node_recursive(tree->root, data, tree->data_size, comp);
}

// Рекурсивная функция для прямого обхода дерева
static void preorder_traversal(tree_node_t *node, int *array, int *index) {
    if (node == NULL) return;  // Если узел отсутствует, ничего не делаем

    array[(*index)++] = *(int *)node->data;  // Сохраняем данные узла
    preorder_traversal(node->left, array, index);  // Левое поддерево
    preorder_traversal(node->right, array, index);  // Правое поддерево
}

// Основная функция прямого обхода дерева
void *bst_preorder_traversal(bst_t *tree) {
    if (tree == NULL || tree->root == NULL) return NULL;  // Проверка на пустое дерево

    size_t num_nodes = bst_length(tree);  // Получаем количество узлов в дереве
    if (num_nodes == 0) return NULL;  // Если узлов нет, возвращаем NULL

    int *result = malloc(num_nodes * sizeof(int));  // Выделяем память под результат
    if (result == NULL) return NULL;  // Не удалось выделить память

    int index = 0;
    preorder_traversal(tree->root, result, &index);  // Заполняем массив
    return result;  // Возвращаем массив
}

// Рекурсивная функция для центрированного обхода дерева
static void inorder_traversal(tree_node_t *node, int *array, int *index) {
    if (node == NULL) return;

    inorder_traversal(node->left, array, index);  // Обход левого поддерева
    array[(*index)++] = *(int *)node->data;  // Сохраняем данные узла
    inorder_traversal(node->right, array, index);  // Обход правого поддерева
}

// Основная функция центрированного обхода дерева
void *bst_inorder_traversal(bst_t *tree) {
    if (tree == NULL || tree->root == NULL) return NULL;  // Проверка на пустое дерево

    size_t num_nodes = bst_length(tree);
    if (num_nodes == 0) return NULL;  // Пустое дерево

    int *result = malloc(num_nodes * sizeof(int));  // Выделяем память под результат
    if (result == NULL) return NULL;  // Не удалось выделить память

    int index = 0;
    inorder_traversal(tree->root, result, &index);  // Заполняем массив
    return result;  // Возвращаем массив
}

// Рекурсивная функция для обратного обхода дерева
static void postorder_traversal(tree_node_t *node, int *array, int *index) {
    if (node == NULL) return;

    postorder_traversal(node->left, array, index);  // Обход левого поддерева
    postorder_traversal(node->right, array, index);  // Обход правого поддерева
    array[(*index)++] = *(int *)node->data;  // Сохраняем данные узла
}

// Основная функция обратного обхода дерева
void *bst_postorder_traversal(bst_t *tree) {
    if (tree == NULL || tree->root == NULL) return NULL;  // Проверка на пустое дерево

    size_t num_nodes = bst_length(tree);
    if (num_nodes == 0) return NULL;  // Пустое дерево

    int *result = malloc(num_nodes * sizeof(int));  // Выделяем память под результат
    if (result == NULL) return NULL;  // Не удалось выделить память

    int index = 0;
    postorder_traversal(tree->root, result, &index);  // Заполняем массив
    return result;  // Возвращаем массив
}

// Функция для вычисления высоты дерева
static int tree_height(tree_node_t *node) {
    if (node == NULL) return 0;  // Если узел пустой, высота равна 0
    int leftHeight = tree_height(node->left);  // Высота левого поддерева
    int rightHeight = tree_height(node->right);  // Высота правого поддерева
    return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;  // Возвращаем максимальную высоту + 1
}

// Функция для сбора данных на определенном уровне дерева
static void collect_level_data(tree_node_t *node, int level, void *array, int *index, size_t data_size) {
    if (node == NULL) return;  // Если узел пустой, завершаем функцию
    if (level == 0) {  // Если достигли нужного уровня
        memcpy((char *)array + (size_t)(*index) * data_size, node->data, data_size);  // Копируем данные узла в массив
        (*index)++;  // Увеличиваем индекс
    } else {  // Если уровень не достигнут, продолжаем рекурсивный обход
        collect_level_data(node->left, level - 1, array, index, data_size);
        collect_level_data(node->right, level - 1, array, index, data_size);
    }
}

// Основная функция для обхода дерева в ширину (уровневый обход)
void *bst_level_order_traversal(bst_t *tree) {
    if (tree == NULL || tree->root == NULL) return NULL;  // Проверка на пустое дерево

    int height = tree_height(tree->root);  // Получаем высоту дерева
    size_t total_nodes = bst_length(tree);  // Получаем общее количество узлов

    // Выделяем память под результаты
    void *results = malloc(total_nodes * tree->data_size);
    if (results == NULL) return NULL;  // Не удалось выделить память

    int index = 0;
    for (int i = 0; i < height; i++) {  // Обход каждого уровня дерева
        collect_level_data(tree->root, i, results, &index, tree->data_size);
    }

    return results;  // Возвращаем массив с данными
}

// Функция для вставки массива данных в дерево
void bst_insert_array(bst_t *tree, size_t size, void *a, comparator_func cmp) {
    char *array = (char *)a;  // Приведение типа указателя для индексации
    for (size_t i = 0; i < size; i++) {
        // Для каждого элемента массива вызываем bst_insert_node
        bst_insert_node(tree, array + i * tree->data_size, cmp);
    }
}

// Рекурсивная функция для печати узла дерева
static void print_node(FILE *f, tree_node_t *node) {
    if (node == NULL) {  // Если узел пустой, печатаем "()"
        fprintf(f, "()");
        return;
    }

    // Печатаем значение текущего узла
    fprintf(f, "(%d", *((int*)node->data));

    // Проверяем наличие детей
    if (node->left || node->right) {
        // Печатаем левое поддерево или ()
        if (node->left) {
            fprintf(f, ", l -> ");
            print_node(f, node->left);
        }

        // Печатаем правое поддерево или ()
        if (node->right) {
            fprintf(f, ", r -> ");
            print_node(f, node->right);
        }
    }

    fprintf(f, ")");  // Завершение печати узла
}

// Основная функция для печати дерева
void bst_print_int(bst_t *tree, FILE *f) {
    if (tree == NULL || tree->root == NULL) {  // Если дерево пустое, печатаем "()"
        fprintf(f, "()");
    } else {
        print_node(f, tree->root);  // Рекурсивная печать дерева
    }
}

// Вспомогательная функция для рекурсивного освобождения узлов дерева
static void destroy_nodes(tree_node_t *node) {
    if (node == NULL) {
        return;  // Базовый случай: узел не существует
    }
    destroy_nodes(node->left);  // Рекурсивно освобождаем левое поддерево
    destroy_nodes(node->right);  // Рекурсивно освобождаем правое поддерево
    
    free(node->data);  // Освобождаем память данных узла
    free(node);  // Освобождаем сам узел
}

// Основная функция для уничтожения дерева
void bst_destroy(bst_t *tree) {
    if (tree != NULL) {
        destroy_nodes(tree->root);  // Освобождаем узлы дерева
        free(tree);  // Освобождаем структуру дерева
    }
}
