#include <stdio.h>
#include "../src/llkfinder.h"

static int is_bg(void *elem)
{
    return *(int*)elem == 0x00;
}

static int is_same(void *elem1, void *elem2)
{
    return *(int*)elem1 == *(int*)elem2;
}

static void print_map(int *map, int size, int cols)
{
    int i, c;

    for (i = 0, c = 0; i < size; ++i, ++c) {
        if (c % cols == 0) {
            printf("\n");
        }
        printf("0x%02x, ", map[i]);
    }

    printf("\n");
}

static void remove_elem(int *map, int cols, int row, int col)
{
    map[row * cols + col] = 0x00;
}

static void play_game(struct llk_info *info,
                      int *map, int rows, int cols)
{
    int i, result;
    int max_try = rows * cols / 2 + 1;

    for (i = 0; i < max_try; ++i) {
        int row1, col1, row2, col2;

        printf("\ntry %d: ", i);
        result = llkfinder_find(info, &row1, &col1, &row2, &col2);

        switch (result) {
        case FIND_OK:
            printf("found: (%d, %d), (%d, %d):\n", row1, col1, row2, col2);
            print_map(map, rows * cols, cols);
            remove_elem(map, cols, row1, col1);
            remove_elem(map, cols, row2, col2);
            break;
        case FIND_FAILED:
            printf("found failed\n");
            break;
        case FIND_WIN:
            printf("win!\n");
            print_map(map, rows * cols, cols);
            break;
        default:
            printf("Invalid result\n");
        }
    }

    if (result == FIND_FAILED) {
        printf("\nmap is in error state\n");
    }
}

int main(int argc, char *argv[])
{
    int rows = 8, cols = 10;
    int map[] = {
        0x19, 0x16, 0x03, 0x17, 0x03, 0x02, 0x17, 0x08, 0x16, 0x05,
        0x11, 0x16, 0x16, 0x20, 0x06, 0x08, 0x16, 0x01, 0x03, 0x01,
        0x12, 0x20, 0x16, 0x02, 0x14, 0x07, 0x16, 0x18, 0x16, 0x13,
        0x08, 0x17, 0x19, 0x11, 0x14, 0x01, 0x12, 0x11, 0x09, 0x01,
        0x15, 0x19, 0x16, 0x11, 0x19, 0x01, 0x18, 0x06, 0x16, 0x01,
        0x16, 0x15, 0x20, 0x16, 0x17, 0x13, 0x16, 0x16, 0x11, 0x19,
        0x16, 0x19, 0x01, 0x02, 0x03, 0x16, 0x16, 0x16, 0x20, 0x21,
        0x08, 0x01, 0x18, 0x07, 0x11, 0x21, 0x05, 0x02, 0x18, 0x09
    };

    printf("Initial map:\n");
    print_map(map, rows * cols, cols);

    struct llk_info *info = llkfinder_init(rows, cols, 1, map, sizeof(map[0]),
                                           is_bg, is_same);

    play_game(info, map, rows, cols);

    llkfinder_free(info);

    return 0;
}
