#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#define NO_ZAPYAT 0
#define IS_DIGIT -1


const size_t wordSize = 3;
int counter = 0x0;
int counterArr[1000];
int counter_start = 0;
int counter_end = 0;

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


typedef struct name_node {
    char *name;
    bool tag;
    int addr;
} name_node;

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


char *concat(char *s1, char *s2) {


    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    char *result = malloc(len1 + len2 + 1);

    if (!result) {
        fprintf(stderr, "malloc() failed: insufficient memory!\n");
        return NULL;
    }

    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);

    return result;
}


char *int_to_16_str(int dig) {
    char *buffer = malloc(sizeof(char) * 100);
    sprintf(buffer, "%x", dig);
    return buffer;

}

char *int_to_10_str(int dig) {
    char *buffer = malloc(sizeof(char) * 1000);
    sprintf(buffer, "%d", dig);
    return buffer;
}

char *concat_with_translate(char *s1, char *s2) {

    // Перевод строки в 16 ричную
    char *buffer;
    int num = atoi(s2);
    sprintf(buffer, "%x", num);
    s2 = buffer;
    //////////

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);

    char *result = malloc(len1 + len2 + 1);

    if (!result) {
        fprintf(stderr, "malloc() failed: insufficient memory!\n");
        return NULL;
    }

    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);

    return result;
}

char *substr(const char *text, int beg, int end) {
    int i;
    char *sub = 0;
    int len = end - beg;
    if (text)//Проверяем не пустой ли ввод
        if (text + beg)//Проверяем существование в тексте позиции beg
            if (0 < len)//Проверяем корректность параметров конец должен быть больше начала
                if ((sub = (char *) malloc(1 + len)))//Если end превосходит последнюю
                    //позицию текста ничего страшного выделим чуть больше памяти чем надо
                {
                    //Примитивное компирование, даже текстовых библиотек не надо будет
                    for (i = beg; text[i] != '\0' && i < end; i++)
                        sub[i - beg] = text[i];
                    sub[i - beg] = '\0';//Ноль терминатор вконце строки
                }
    return sub;
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


bool checkIfIn(unsigned int index, Hash_Table *pTable, name_node *pNode);

void deletechar(char *str, int pos) {
    int i;
    for (i = pos; i < strlen(str); ++i)str[i] = str[i + 1];
}

void push_to_name_table(Hash_Table *table, name_node *el) {
    unsigned int index = hash(el->name, table->size);
    if (checkIfIn(index, table, el)) {

        return;
    }
    push_back(table->list[index], el);
}

bool checkIfIn(unsigned int index, Hash_Table *name_table, name_node *node) {

    if (name_table->list[index]->el != NULL) {
        name_node *el = (name_node *) name_table->list[index]->el->data;
        int equality = strcmp(el->name, node->name);
        if (equality == 0) {
            if (el->tag != node->tag) {
                el->tag = 1;
                el->addr = counter;
            }
            if (el->addr != node->addr) {
                printf("ОШИБКА Данная метка уже существует"); //TODO ЭТО ВЫВОД ОШИБКИ ПИСАТЬ В ФАЙЛ
            }
            return true;

        }
        return false;
    }
    return false;
}

void print_mnem_table(Hash_Table *table) {

    for (int i = 0; i < table->size; i++) {
        printf("%s", "index № ");
        printf(" %d :", i);
        list_node *node = table->list[i]->el;
        while (node != NULL) {
            mnem_node *data = (mnem_node *) node->data;
            printf("%s", data->operator);
            printf(" Директива? %d ", data->is_direct);
            if (data->is_direct == 0) {
                printf("Информация: %d ", data->nonDirectInfo->info);
                printf("Размер: %d ", data->nonDirectInfo->size);
            } else {
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


void print_name_table(Hash_Table *table) {

    for (int i = 0; i < table->size; i++) {
        printf("%s", "index № ");
        printf(" %d:", i);
        list_node *node = table->list[i]->el;
        while (node != NULL) {
            name_node *data = (name_node *) node->data;
            printf(" Имя %s ", data->name);
            printf(" Признак %d ", data->tag);
            printf(" Адресс %x ", data->addr);
            if (node->next != NULL) {
                printf("%s", "->");
            }
            node = node->next;
        }
        printf("\n");
    }
    printf("\n");
}


mnem_node *find(Hash_Table *table, char *key) {
    for (int i = 0; i < table->size; i++) {
        list_node *node = table->list[i]->el;
        while (node != NULL) {
            mnem_node *data = (mnem_node *) node->data;
            if (strcmp(data->operator, key) == 0) {
                return data;
            }
            node = node->next;
        }
    }
    return NULL;
}


int find_addr_in_name_table(Hash_Table *table, char *key) {
    for (int i = 0; i < table->size; i++) {
        list_node *node = table->list[i]->el;
        while (node != NULL) {
            name_node *data = (name_node *) node->data;
            if (strcmp(data->name, key) == 0) {
                return data->addr;
            }
            node = node->next;
        }
    }
    return 0;
}

// обработчик директивы START
void start(size_t *c, char *operand) {
    *c = strtol(operand, NULL, 16);
    counter_start = *c;
}

// обработчик директивы END
void end(size_t *c, char *operand) {
    counter_end = counter;
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


void parseString(char *buf, int numOfStr, Hash_Table *pTable, Hash_Table *name_table);

void printResult(char *metkaStr, char *operatorStr, char *operandStr, char *commentStr, int numOfStr);

bool checkIfOperatorCorrect(char *str);

bool get_x(char *operandStr);

int get_comp_command(bool x, int cop, int addr);

int check_if_operand_like_metka(char *operand);

void parse_file_second_time(Hash_Table *mnem_table, Hash_Table *name_table);

char *get_resw_command(char *str);

char *get_resb_command(char *str);

char *get_word_command(char *operand);

char *get_hex_command_dump(FILE *f, char *command, int command1, int counter, char *str, FILE *obj_code_f);

char commentSymbol = ';';


void appending(char *cArr, const char c) {
    unsigned long len = strlen(cArr);
    cArr[len] = c;

}

bool isEmpty(const char *str) {
    return str == NULL || str[0] == '\0';
}

void parseString(char *buf, int numOfStr, Hash_Table *mnem_table, Hash_Table *name_table) {
    int i = 0;
    const int lenOf = strlen(buf);
    char ch;
    if (buf[0] == '\n') {
        printResult(NULL, NULL, NULL, NULL, numOfStr);
        return;
    }
    //// нахождение метки
    char *metkaStr = (char *) malloc(lenOf * sizeof(char));
    if (buf[0] != ' ' && buf[0] != commentSymbol) {
        for (i; i < lenOf; i++) {
            ch = buf[i];
            if (ch == ' ') break;
            appending(metkaStr, ch);

        }
        i++;
    }

    //////////

    ch = buf[i];
    //// нахождение первого слова
    while (ch == ' ' && i < lenOf) {
        i++;
        ch = buf[i];
    }
    ////

    char *tempStr = (char *) malloc(lenOf * sizeof(char));
    char *operatorStr = (char *) malloc(lenOf * sizeof(char));
    char *operandStr = (char *) malloc(lenOf * sizeof(char));
    char *commentStr = (char *) malloc(lenOf * sizeof(char));
    bool nextOperand = false;
    for (i; i < lenOf; i++) {
        ch = buf[i];
        if (ch == ' ' || ch == '\n') {
            if (tempStr[0] == commentSymbol) {
                commentStr = ++tempStr;
                tempStr = (char *) malloc(lenOf * sizeof(char));
                continue;
            }
            if (nextOperand) {
                operandStr = tempStr;
                tempStr = (char *) malloc(lenOf * sizeof(char));
                continue;
            }

            if (checkIfOperatorCorrect(tempStr)) {
                operatorStr = tempStr;
                nextOperand = true;
            } else {
                printf("Данный оператор неккорректен -->"); //TODO ЭТО ВЫВОД СООБЩЕНИЯ ОБ ОШИБКЕ ПИСАТЬ ЕГО В ФАЙЛ
                printf("%s", tempStr);
            }
            tempStr = (char *) malloc(lenOf * sizeof(char));
            continue;

        }
        appending(tempStr, ch);
    }
    if (isEmpty(operatorStr) && isEmpty(operandStr)) {
        operatorStr = tempStr;
    }
    if (!isEmpty(operatorStr) && isEmpty(operandStr)) operandStr = tempStr;
    if (operatorStr == operandStr) operandStr = NULL;
    printResult(metkaStr, operatorStr, operandStr, commentStr, numOfStr);

    mnem_node *res = find(mnem_table, operatorStr);
    if (strcmp(metkaStr, "") != 0) {
        deletechar(metkaStr, strlen(metkaStr) - 1);
        name_node *nameNode = calloc(sizeof(name_node), 1);
        nameNode->name = metkaStr;
        nameNode->addr = counter;
        nameNode->tag = 1;
        push_to_name_table(name_table, nameNode);
        //print_name_table(name_table);
    }
    int zapyat_pos = check_if_operand_like_metka(operandStr);

    if (zapyat_pos != IS_DIGIT && strcmp(operatorStr, "END") != 0) {
        name_node *nameNode = calloc(sizeof(name_node), 1);
        if (zapyat_pos == NO_ZAPYAT) {
            nameNode->name = operandStr;
        } else {
            nameNode->name = substr(operandStr, 0, zapyat_pos);
        }
        nameNode->addr = counter;
        nameNode->tag = 0;
        push_to_name_table(name_table, nameNode);
        //print_name_table(name_table);
    }


    counterArr[numOfStr] = counter;

    if (res->is_direct == 1) {
        res->worker(&counter, operandStr);
    } else {
        counter += 0x3;
    }


    metkaStr = NULL;
    free(metkaStr);
    operatorStr = NULL;
    free(operatorStr);
    tempStr = NULL;
    free(tempStr);
    commentStr = NULL;
    free(commentStr);


}


int combine(int a, int b) {
    a = a << 16;
    return a + b;
}

int get_comp_command(bool x, int cop, int addr) {
    if (x == 0) {
        return combine(cop, addr);
    }
    return combine(cop, addr) + 0x8000;
}


bool checkIfOperatorCorrect(char *str) {
    if (strcmp(str, "START") == 0 || strcmp(str, "END") == 0 || strcmp(str, "BYTE") == 0 || strcmp(str, "WORD") == 0 ||
        strcmp(str, "RESB") == 0 || strcmp(str, "RESW") == 0 || strcmp(str, "lda") == 0 || strcmp(str, "ldx") == 0 ||
        strcmp(str, "add") == 0 || strcmp(str, "addx") == 0 || strcmp(str, "hlt") == 0 ||
        strcmp(str, "sta") == 0) {
        return true;
    }
    return false;
}


void printDataWithMessage(const char *message, char *data) {
    printf("%s", message);
    if (data != NULL) {
        printf("%s", data);
    }

}

void printResult(char *metkaStr, char *operatorStr, char *operandStr, char *commentStr, int numOfStr) {
    printf("%d", numOfStr);
    printf("%s", ") ");
    printDataWithMessage("Метка: ", metkaStr);
    printDataWithMessage(" Оператор: ", operatorStr);
    printDataWithMessage(" Операнд: ", operandStr);
    printDataWithMessage(" Комментарий: ", commentStr);
    printf("\n");
}

int check_if_operand_like_metka(char *operand) {

    char ch = operand[0];
    if (isdigit(ch) || strcmp(operand, "") == 0) {
        return IS_DIGIT;
    }
    int i = 1;
    while (ch != '\0') {
        ch = operand[i];
        if (ch == ',') {
            return i;
        }
        i++;
    }
    return NO_ZAPYAT;
}


void parse_file(Hash_Table *table, Hash_Table *name_table) {
    int numberOfStr = 1;
    const char *pathToFile = "../program.txt";
    char *line_buf = (char *) malloc(255 * sizeof(char));
    size_t line_buf_size = 0;
    ssize_t line_size;
    FILE *fp = fopen(pathToFile, "r");
    if (!fp) {
        fprintf(stderr, "Error opening file '%s'\n", "program.txt");
        return;
    }
    line_size = getline(&line_buf, &line_buf_size, fp);
    while (line_size >= 0) {
        parseString(line_buf, numberOfStr, table, name_table);
        numberOfStr++;
        line_size = getline(&line_buf, &line_buf_size, fp);
    }
    line_buf = NULL;
    free(line_buf);
    fclose(fp);
}


void parseStringWithNameTable(char *buf, int numOfStr, Hash_Table *mnem_table, Hash_Table *name_table, FILE *pFile,
                              FILE *obj_code_file) {
    int i = 0;
    const int lenOf = strlen(buf);
    char ch;
    if (buf[0] == '\n') {
        printResult(NULL, NULL, NULL, NULL, numOfStr);
        return;
    }
    //// нахождение метки
    char *metkaStr = (char *) malloc(lenOf * sizeof(char));
    if (buf[0] != ' ' && buf[0] != commentSymbol) {
        for (i; i < lenOf; i++) {
            ch = buf[i];
            if (ch == ' ') break;
            appending(metkaStr, ch);

        }
        i++;
    }

    //////////

    ch = buf[i];
    //// нахождение первого слова
    while (ch == ' ' && i < lenOf) {
        i++;
        ch = buf[i];
    }
    ////

    char *tempStr = (char *) malloc(lenOf * sizeof(char));
    char *operatorStr = (char *) malloc(lenOf * sizeof(char));
    char *operandStr = (char *) malloc(lenOf * sizeof(char));
    char *commentStr = (char *) malloc(lenOf * sizeof(char));
    bool nextOperand = false;
    for (i; i < lenOf; i++) {
        ch = buf[i];
        if (ch == ' ' || ch == '\n') {
            if (tempStr[0] == commentSymbol) {
                commentStr = ++tempStr;
                tempStr = (char *) malloc(lenOf * sizeof(char));
                continue;
            }
            if (nextOperand) {
                operandStr = tempStr;
                tempStr = (char *) malloc(lenOf * sizeof(char));
                continue;
            }

            if (checkIfOperatorCorrect(tempStr)) {
                operatorStr = tempStr;
                nextOperand = true;
            } else {
                printf("Данный оператор неккорректен -->"); //TODO ЭТО ВЫВОД СООБЩЕНИЯ ОБ ОШИБКЕ ПИСАТЬ ЕГО В ФАЙЛ
                printf("%s", tempStr);
            }
            tempStr = (char *) malloc(lenOf * sizeof(char));
            continue;

        }
        appending(tempStr, ch);
    }
    if (isEmpty(operatorStr) && isEmpty(operandStr)) {
        operatorStr = tempStr;
    }
    if (!isEmpty(operatorStr) && isEmpty(operandStr)) operandStr = tempStr;
    if (operatorStr == operandStr) operandStr = NULL;
    printResult(metkaStr, operatorStr, operandStr, commentStr, numOfStr);


    mnem_node *res = find(mnem_table, operatorStr);
    int int_command = 0;
    char *str_command = "";

    if (res->is_direct == 0) {
        if (strcmp(res->operator, "hlt") == 0) {
            fprintf(pFile, "Команда hlt: 0f0000\n");
            str_command = "0f0000";

        } else {
            int x = 0;
            int zapyat_pos = check_if_operand_like_metka(operandStr);
            if (zapyat_pos != NO_ZAPYAT && zapyat_pos != IS_DIGIT) {
                x = 1;
                operandStr = substr(operandStr, 0, zapyat_pos);
            }
            int COP = res->nonDirectInfo->info;

            int addr = find_addr_in_name_table(name_table, operandStr);
            int_command = get_comp_command(x, COP, addr);
            fprintf(pFile, "Команда %s: %x\n", operatorStr, int_command);
        }
    } else {
        if (strcmp(operatorStr, "RESW") == 0) {
            str_command = get_resw_command(operandStr);
            fprintf(pFile, "Команда %s: %s\n", operatorStr, str_command);
        }

        if (strcmp(operatorStr, "RESB") == 0) {
            str_command = get_resb_command(operandStr);
            fprintf(pFile, "Команда %s: %s\n", operatorStr, str_command);
        }

        if (strcmp(operatorStr, "WORD") == 0) {
            str_command = get_word_command(operandStr);
            fprintf(pFile, "Команда %s: %s\n ", operatorStr, str_command);
        }

    }
    int old_counter = counterArr[numOfStr];

    char *dump = get_hex_command_dump(pFile, str_command, int_command, old_counter, operatorStr, obj_code_file);
    //printf("%d %d",counter_start,counter_end);
    metkaStr = NULL;
    free(metkaStr);
    operatorStr = NULL;
    free(operatorStr);
    tempStr = NULL;
    free(tempStr);
    commentStr = NULL;
    free(commentStr);
    str_command = NULL;
    free(str_command);


}


char *
get_hex_command_dump(FILE *f, char *str_command, int int_command, int count, char *operator, FILE *obj_code_file) {
    if (int_command != 0) {
        str_command = int_to_16_str(int_command);
    }
    char *counter_str = int_to_16_str(count);
    char *hex_dump = "";
    if (strcmp(operator, "START") == 0) {
        int program_size = counter_end - counter_start;
        counter_str = int_to_16_str(counter_start);
        char *program_size_str = concat("00", int_to_16_str(program_size)); //TODO cal zeros
        char *hex_dump_raw = concat(concat(":02", counter_str), "02");
        hex_dump = concat(concat(hex_dump_raw, program_size_str), "XX");

    } else if (strcmp(operator, "END") == 0) {
        char *counter_start_str = int_to_16_str(counter_start);
        hex_dump = concat(concat(":00", counter_start_str), "01XX");

    } else {
        int size = strlen(str_command) / 2;
        char *size_str = concat("0", int_to_10_str(size));
        char *hex_dump_raw = concat(concat(concat(":", size_str), counter_str), "00");
        hex_dump = concat(concat(hex_dump_raw, str_command), "XX");

    }

    fprintf(obj_code_file, "%s\n", hex_dump);
    return NULL;
}

char *get_word_command(char *operand) {
    const int command_size = wordSize * 2;
    char *nulls = calloc(sizeof(char *), 1);
    for (int i = 0; i < (command_size - 1); i++) {
        appending(nulls, '0');
    }
    return concat_with_translate(nulls, operand);
}

char *get_resb_command(char *operand) {
    int size = strtol(operand, NULL, 10);
    char *res = calloc(sizeof(char *), 1);
    for (int i = 0; i < size; i++) {
        res = concat(res, "00");
    }
    return res;
}

char *get_resw_command(char *operand) {
    int size = strtol(operand, NULL, 10);
    char *res = calloc(sizeof(char *), 1);
    for (int i = 0; i < size; i++) {
        res = concat(res, "000000");
    }
    return res;
}


void parse_file_second_time(Hash_Table *mnem_table, Hash_Table *name_table) {
    int numberOfStr = 1;
    const char *pathToFile = "../program.txt";
    const char *path_to_out_file = "../out.txt";
    const char *path_to_obj_code_file = "../obj_code.txt";
    char *line_buf = (char *) malloc(255 * sizeof(char));
    size_t line_buf_size = 0;
    ssize_t line_size;
    FILE *fp = fopen(pathToFile, "r");
    if (!fp) {
        fprintf(stderr, "Error opening file '%s'\n", "program.txt");
        return;
    }
    FILE *fout = fopen(path_to_out_file, "w");
    if (!fout) {
        fprintf(stderr, "Error opening file '%s'\n", "program.txt");
        return;
    }

    FILE *fobj = fopen(path_to_obj_code_file, "w");
    if (!fout) {
        fprintf(stderr, "Error opening file '%s'\n", "program.txt");
        return;
    }

    line_size = getline(&line_buf, &line_buf_size, fp);
    while (line_size >= 0) {
        parseStringWithNameTable(line_buf, numberOfStr, mnem_table, name_table, fout, fobj);
        numberOfStr++;
        line_size = getline(&line_buf, &line_buf_size, fp);
    }
    line_buf = NULL;
    free(line_buf);
    fclose(fp);
}


int main(void) {
    ////////////////////////// Создание таблицы мнемоник
    Hash_Table *table = create_hash_table(41);
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
    info1->info = 0x11;
    info1->size = 3;
    mnem_node node7 = {
            "add",
            0,
            info1,
    };
    non_direct_info *info2 = calloc(sizeof(non_direct_info *), 1);
    info2->info = 0x12;
    info2->size = 3;
    mnem_node node8 = {
            "addx",
            0,
            info2,
    };

    non_direct_info *info3 = calloc(sizeof(non_direct_info *), 1);
    info3->info = 0x21;
    info3->size = 3;
    mnem_node node9 = {
            "lda",
            0,
            info3,
    };

    non_direct_info *info4 = calloc(sizeof(non_direct_info *), 1);
    info4->info = 0x22;
    info4->size = 3;
    mnem_node node10 = {
            "ldx",
            0,
            info4,
    };

    non_direct_info *info5 = calloc(sizeof(non_direct_info *), 1);
    info5->info = 0x31;
    info5->size = 3;
    mnem_node node11 = {
            "sta",
            0,
            info5,
    };

    non_direct_info *info6 = calloc(sizeof(non_direct_info *), 1);
    info6->info = 0xf;
    info6->size = 3;
    mnem_node node12 = {
            "hlt",
            0,
            info6,
    };

    push_to_table(table, &node);
    push_to_table(table, &node2);
    push_to_table(table, &node3);
    push_to_table(table, &node4);
    push_to_table(table, &node5);
    push_to_table(table, &node6);
    push_to_table(table, &node7);
    push_to_table(table, &node8);
    push_to_table(table, &node9);
    push_to_table(table, &node10);
    push_to_table(table, &node11);
    push_to_table(table, &node12);
    //print_mnem_table(table);
    ////////////////////////////////////

    Hash_Table *name_table = create_hash_table(11);


    parse_file(table, name_table);
    parse_file_second_time(table, name_table);

    return EXIT_SUCCESS;
}

