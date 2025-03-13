#ifndef BST_H
#define BST_H

// Подключение стандартных библиотек для управления памятью, обработки строк, работы с булевыми типами, целочисленными типами, вводом-выводом файлов и статусом файлов.
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

// Определение структуры узла дерева.
typedef struct tree_node_t
{
    void *data;              // Указатель на данные, хранящиеся в узле.
    struct tree_node_t *left; // Указатель на левый дочерний узел.
    struct tree_node_t *right; // Указатель на правый дочерний узел.
} tree_node_t;

// Определение структуры бинарного дерева поиска (BST).
typedef struct
{
    tree_node_t *root;       // Указатель на корневой узел дерева.
    size_t data_size;        // Размер данных, хранящихся в каждом узле.
} bst_t;

// Тип функции-компаратора, используемой для сравнения данных в BST.
typedef int (*comparator_func)(const void *, const void *);

// Тип функции, используемой для печати данных в BST.
typedef void (*print_datatype_func)(FILE *, void *);

// Функция для создания нового BST.
bst_t *bst_create(size_t);

// Функция для проверки, пусто ли BST.
bool bst_empty(bst_t *);

// Функция для проверки, содержит ли BST определённый элемент данных.
bool bst_contains(bst_t *, void *, comparator_func);

// Функция для получения количества узлов в BST.
size_t bst_length(bst_t *);

// Функция для вставки нового узла в BST.
void bst_insert_node(bst_t *, void *, comparator_func);

// Функция для удаления узла из BST.
void bst_delete_node(bst_t *, void *, comparator_func);

// Функция для вставки массива данных в BST.
void bst_insert_array(bst_t *, size_t, void *, comparator_func);

// Функция для уничтожения BST и освобождения его памяти.
void bst_destroy(bst_t *);

// Функция для печати BST в целочисленном формате.
void bst_print_int(bst_t *, FILE *);

// Функция для выполнения префиксного обхода BST.
void *bst_preorder_traversal(bst_t *);

// Функция для выполнения инфиксного обхода BST.
void *bst_inorder_traversal(bst_t *);

// Функция для выполнения постфиксного обхода BST.
void *bst_postorder_traversal(bst_t *);

// Функция для выполнения обхода BST по уровням.
void *bst_level_order_traversal(bst_t *);

// Функция для отображения BST с использованием функции печати.
void bst_render(bst_t *, print_datatype_func, bool);

#endif // BST_H
