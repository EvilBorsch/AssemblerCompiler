#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

const size_t wordSize = 3;
size_t counter = 0;


typedef struct non_direct_info {
    int size;
    int info;
} non_direct_info;

typedef struct mnem_node {
    char *operator;
    bool is_direct;

    union {
        void (*worker)(size_t *, char *);

        non_direct_info *nonDirectInfo;
    };
} mnem_node;

/* Определяем элемент списка */
typedef struct list_node {
    struct list_node *next;
    void *data;
} list_node;

/* Определяем сам список */
typedef struct list {
    int size;
    list_node *el;
} list;


typedef struct Hash_Table {
    list **list;
    unsigned int size;
} Hash_Table;

list *create_list() {

    list *l = (list *) calloc(sizeof(list), 1);
    l->el = NULL;
    l->size = 0;
    return l;
}

void push_back(list *l, void *new_el) {
    list_node *node = l->el;
    if (node == NULL) {
        list_node *new_node = (list_node *) calloc(sizeof(list_node *), 1);
        new_node->data = new_el;
        new_node->next = NULL;
        l->el = new_node;
        return;
    }
    while (1) {
        if (node->next == NULL) {
            list_node *new_node = (list_node *) calloc(sizeof(list_node *), 1);
            new_node->data = new_el;
            new_node->next = NULL;
            node->next = new_node;
            break;
        }
        node = node->next;
    }

}


unsigned int WordCombination(const char *str) {
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

unsigned int hash(const char *str, const unsigned int tableSize) {
    unsigned K = WordCombination(str);
    unsigned res = K % tableSize;
    return res;
}

Hash_Table *create_hash_table(unsigned int size) {
    Hash_Table *table = (Hash_Table *) calloc(sizeof(Hash_Table *), 1);
    table->list = calloc(sizeof(list *), size);
    for (int i = 0; i < size; i++) {
        table->list[i] = create_list();
    }
    table->size = size;
    return table;
}


void push_to_table(Hash_Table *table, mnem_node *el) {
    unsigned int index = hash(el->operator, table->size);
    push_back(table->list[index], el);
}

void print_mnem_table(Hash_Table *table) {

    for (int i = 0; i < table->size; i++) {
        printf("%s", "index № ");
        printf(":%d ", i);
        list_node *node = table->list[i]->el;
        while (node != NULL) {
            mnem_node *data = (mnem_node *) node->data;
            printf("%s", data->operator);
            printf(" Директива? %d ", data->is_direct);
            if (data->is_direct==0){
                printf("Информация: %d ",data->nonDirectInfo->info);
                printf("Размер: %d ",data->nonDirectInfo->size);
            }
            else{
                printf("Есть функция для обработки");
            }
            if (node->next != NULL) {
                printf("%s", "->");
            }
            node = node->next;
        }
        printf("\n");
    }
}


mnem_node *find(Hash_Table *table, char *key) {
    for (int i = 0; i < table->size; i++) {
        list_node *node = table->list[i]->el;
        while (node != NULL) {
            mnem_node *data = (mnem_node *) node->data;
            if (data->operator == key) {
                return data;
            }
            node = node->next;
        }
        printf("\n");
    }
    return NULL;
}

// обработчик директивы START
void start(size_t *c, char *operand) {
    *c = strtol(operand, NULL, 0);
}

// обработчик директивы END
void end(size_t *c, char *operand) {
    printf("Первая исполняемая команда - %s\n", operand);
}

// обработчик директивы WORD
void word(size_t *c, char *operand) {
    *c += wordSize;
}

// обработчик директивы BYTE
void byte(size_t *c, char *operand) {
    size_t n = strtol(operand, NULL, 0);
    while (n) {
        n = n >> 8;
        ++(*c);
    }
}

// обработчик директивы RESB
void resb(size_t *c, char *operand) {
    *c += strtol(operand, NULL, 0);
}

// обработчик директивы RESW
void resw(size_t *c, char *operand) {
    *c += wordSize * strtol(operand, NULL, 0);
}


int main() {
    ////////////////////////// Создание таблицы мнемоник
    Hash_Table *table = create_hash_table(17);
    mnem_node node = {
            "START",
            1,
            start,
    };
    mnem_node node2 = {
            "END",
            1,
            end,
    };
    mnem_node node3 = {
            "BYTE",
            1,
            byte,
    };

    mnem_node node4 = {
            "WORD",
            1,
            word,
    };

    mnem_node node5 = {
            "RESW",
            1,
            resw,
    };

    mnem_node node6 = {
            "RESB",
            1,
            resb,
    };
    non_direct_info *info1 = calloc(sizeof(non_direct_info *), 1);
    info1->info = 1;
    info1->size = 3;
    mnem_node node7 = {
            "int",
            0,
            info1,
    };
    non_direct_info *info2 = calloc(sizeof(non_direct_info *), 1);
    info2->info = 2;
    info2->size = 3;
    mnem_node node8 = {
            "move",
            0,
            info2,
    };

    push_to_table(table, &node);
    push_to_table(table, &node2);
    push_to_table(table, &node3);
    push_to_table(table, &node4);
    push_to_table(table, &node5);
    push_to_table(table, &node6);
    push_to_table(table, &node7);
    push_to_table(table, &node8);
    ////////////////////////////////////

    print_mnem_table(table);




}