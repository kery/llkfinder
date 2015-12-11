#ifndef __LLKFINDER_H
#define __LLKFINDER_H

// Type of user defined callback function, which
// is used to check if an element is background
typedef int (*is_bg_cb)(void *elem);

// Type of user defined callback function, which
// is used to check if two elements are the same
typedef int (*is_same_cb)(void *elem1, void *elem2);

struct llk_info;

// "map" is the user supplied address where the data
// structure of parsed picture stored
// "elem_size" is the size (in byte) of the data structure
struct llk_info * llkfinder_init(int rows, int cols, int across_border,
                                 void *map, int elem_size,
                                 is_bg_cb is_bg, is_same_cb is_same);

#define FIND_FAILED -1
#define FIND_OK 0
#define FIND_WIN 1

int llkfinder_find(struct llk_info *info,
                   int *row1, int *col1, int *row2, int *col2);

void llkfinder_free(struct llk_info *info);

#endif // __LLKFINDER_H
