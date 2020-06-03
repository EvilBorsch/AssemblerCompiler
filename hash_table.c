#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define NOT_FOUND_IN_TABLE -1


/* Определяем элемент списка */
typedef struct list_node {
    struct list_node *next;
    char *data;
} list_node_t;

/* Определяем сам список */
typedef struct list {
    int size;
    list_node_t *head;
    list_node_t *tail;
} list_t;


typedef struct Hash_Table {
    list_t **list;
    unsigned int size;
} Hash_Table;

/* Инициализация массива */
list_t *create_list(void) {
    list_t *lt = malloc(sizeof(list_t));

    lt->size = 0;
    lt->head = NULL;
    lt->tail = lt->head;

    return lt;
}


bool is_strings_equal(const char *str1, const char *str2) {
    return strcmp(str1, str2) == 0;
}

bool is_el_in_list(list_t *lt, const char *data) {
    list_node_t *node = lt->head;
    while (node != NULL) {

        if (is_strings_equal(node->data, data)) {
            return true;
        }
        node = node->next;
    }
    return false;
}

void print_list(list_t *lt) {
    list_node_t *node = lt->head;
    while (node != NULL) {
        printf("%s", node->data);

        node = node->next;
        if (node != NULL) {
            printf("%s", " -> ");
        }
    }
    printf("\n");
}

/* Добавляем элемент в конец списка */
void list_push_back(list_t *lt, void *data) {
    list_node_t *node = malloc(sizeof(list_node_t));
    node->data = data;
    if (lt->tail != NULL)
        lt->tail->next = node;
    else {
        lt->head = node;
    }

    lt->tail = node;
    lt->size += 1;
}


int WordCombination(const char *str) {
    const unsigned VALUE_BYTES = 4;
    unsigned length = strlen(str);
    unsigned int result = 0;

    bool first_part = true;
    union w {
        char w[4];
        unsigned long value;
    };
    union w word;

    for (unsigned i = 0; i < length; i += VALUE_BYTES) {
        memmove(word.w, str + i, VALUE_BYTES * sizeof(char));
        if (first_part) {
            word.value = word.value >> 1;
            first_part = false;
            result ^= word.value;
            word.value = 0;
            continue;
        }
        result ^= word.value;
        word.value = 0;
        first_part = true;
    }

    return result;
}

int hash(const char *str, const unsigned int tableSize) {
    int A = 7;
    int w = 11;
    int m = 1;
    int K = WordCombination(str);
    int res = ((A * K) >> (w - m)) % tableSize;
    return res;
}


Hash_Table generate_hash_table(int size) {
    list_t **list = malloc(sizeof(list_t) * size);
    for (int i = 0; i < size; i++) {
        list[i] = create_list();
    }
    Hash_Table table;
    table.list = list;
    table.size = size;
    return table;
}

void print_hash_table(Hash_Table *table) {
    for (unsigned int i = 0; i < table->size; i++) {
        printf("%s", "index № ");
        printf("%d", i);
        printf(": ");
        print_list(table->list[i]);
    }
    printf("\n");
}

void pushEl(Hash_Table *table, const char *el) {
    int index =(int) hash(el, table->size);
    list_push_back(table->list[index], el);
}

int searchEl(Hash_Table *table, const char *searching_el) {
    int index = (int) hash(searching_el, table->size);
    if (is_el_in_list(table->list[index], searching_el)) {
        return index;
    }
    return NOT_FOUND_IN_TABLE;
}

int main() {
    int size = 17;
    Hash_Table hashTable = generate_hash_table(size);
    char keys[10][100] = {
            "GG",
            "START",
            "END",
            "BYTE",
            "WORD",
            "RESB",
            "RESW",
            "ADD",
            "CMP",
            "PP",
    };

    for (size_t c = 0; c < 10; ++c) {
        pushEl(&hashTable, keys[c]);
    }

    print_hash_table(&hashTable);

}