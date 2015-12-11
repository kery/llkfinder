#include <stdlib.h>
#include "llkfinder.h"

struct llk_info {
    int rows;
    int cols;
    int across_border;
    char *map;
    int elem_size;
    is_bg_cb is_bg;
    is_same_cb is_same;
};

static void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

static void get_pos(struct llk_info *info,
                    int i, int *row, int *col)
{
    *row = i / info->cols;
    *col = i % info->cols;
}

static void * get_elem(struct llk_info *info, int row, int col)
{
    return info->map + (row * info->cols + col) * info->elem_size;
}

static int no_elem_top(struct llk_info *info, int row, int col)
{
    int r;

    for (r = row - 1; r >= 0; --r) {
        if (!info->is_bg(get_elem(info, r, col))) {
            return 0;
        }
    }
    return 1;
}

static int no_elem_bottom(struct llk_info *info, int row, int col)
{
    int r;

    for (r = row + 1; r < info->rows; ++r) {
        if (!info->is_bg(get_elem(info, r, col))) {
            return 0;
        }
    }
    return 1;
}

static int no_elem_left(struct llk_info *info, int row, int col)
{
    int c;

    for (c = col - 1; c >= 0; --c) {
        if (!info->is_bg(get_elem(info, row, c))) {
            return 0;
        }
    }
    return 1;
}

static int no_elem_right(struct llk_info *info, int row, int col)
{
    int c;

    for (c = col + 1; c < info->cols; ++c) {
        if (!info->is_bg(get_elem(info, row, c))) {
            return 0;
        }
    }
    return 1;
}

static int direct_link_hor(struct llk_info *info, int row, int col1, int col2)
{
    int c;

    if (col1 > col2) {
        swap(&col1, &col2);
    }

    for (c = col1 + 1; c < col2; ++c) {
        if (!info->is_bg(get_elem(info, row, c))) {
            return 0;
        }
    }
    return 1;
}

static int direct_link_ver(struct llk_info *info, int row1, int row2, int col)
{
    int r;

    if (row1 > row2) {
        swap(&row1, &row2);
    }

    for (r = row1 + 1; r < row2; ++r) {
        if (!info->is_bg(get_elem(info, r, col))) {
            return 0;
        }
    }
    return 1;
}

static int direct_link(struct llk_info *info,
                       int row1, int col1, int row2, int col2)
{
    if (row1 == row2) {
        return direct_link_hor(info, row1, col1, col2);
    }
    if (col1 == col2) {
        return direct_link_ver(info, row1, row2, col1);
    }
    return 0;
}

static int one_corner_link(struct llk_info *info,
                           int row1, int col1, int row2, int col2)
{
    if (row1 == row2 || col1 == col2) {
        return 0;
    }
    if (info->is_bg(get_elem(info, row1, col2)) &&
        direct_link_hor(info, row1, col1, col2) &&
        direct_link_ver(info, row1, row2, col2))
    {
        return 1;
    }
    if (info->is_bg(get_elem(info, row2, col1)) &&
        direct_link_ver(info, row1, row2, col1) &&
        direct_link_hor(info, row2, col1, col2))
    {
        return 1;
    }
    return 0;
}

static int two_corner_link(struct llk_info *info,
                           int row1, int col1, int row2, int col2)
{
    int r, c;

    for (r = row1 - 1; r >= 0; --r) {
        if (!info->is_bg(get_elem(info, r, col1))) {
            break;
        }
        if (one_corner_link(info, r, col1, row2, col2)) {
            return 1;
        }
    }

    for (r = row1 + 1; r < info->rows; ++r) {
        if (!info->is_bg(get_elem(info, r, col1))) {
            break;
        }
        if (one_corner_link(info, r, col1, row2, col2)) {
            return 1;
        }
    }

    for (c = col1 - 1; c >= 0; --c) {
        if (!info->is_bg(get_elem(info, row1, c))) {
            break;
        }
        if (one_corner_link(info, row1, c, row2, col2)) {
            return 1;
        }
    }

    for (c = col1 + 1; c < info->cols; ++c) {
        if (!info->is_bg(get_elem(info, row1, c))) {
            break;
        }
        if (one_corner_link(info, row1, c, row2, col2)) {
            return 1;
        }
    }

    if (info->across_border) {
        if (no_elem_top(info, row1, col1) &&
            no_elem_top(info, row2, col2))
        {
            return 1;
        }
        if (no_elem_bottom(info, row1, col1) &&
            no_elem_bottom(info, row2, col2))
        {
            return 1;
        }
        if (no_elem_left(info, row1, col1) &&
            no_elem_left(info, row2, col2))
        {
            return 1;
        }
        if (no_elem_right(info, row1, col1) &&
            no_elem_right(info, row2, col2))
        {
            return 1;
        }
    }

    return 0;
}

static int is_linkable(struct llk_info *info,
                       int row1, int col1, int row2, int col2)
{
    if (direct_link(info, row1, col1, row2, col2) ||
        one_corner_link(info, row1, col1, row2, col2) ||
        two_corner_link(info, row1, col1, row2, col2))
    {
        return 1;
    }
    return 0;
}

struct llk_info * llkfinder_init(int rows, int cols, int across_border,
                                 void *map, int elem_size,
                                 is_bg_cb is_bg, is_same_cb is_same)
{
    struct llk_info *info = malloc(sizeof(*info));

    if (info) {
        info->rows = rows;
        info->cols = cols;
        info->across_border = across_border;
        info->map = map;
        info->elem_size = elem_size;
        info->is_bg = is_bg;
        info->is_same = is_same;
    }

    return info;
}

int llkfinder_find(struct llk_info *info,
                   int *row1, int *col1, int *row2, int *col2)
{
    int i, j, elem_count, all_bg;

    all_bg = 1; // Assume all elements are background
    elem_count = info->rows * info->cols;
    for (i = 0; i < elem_count; ++i) {
        void *elem1 = info->map + i * info->elem_size;
        if (info->is_bg(elem1)) {
            continue;
        }

        all_bg = 0;
        for (j = i + 1; j < elem_count; ++j) {
            void *elem2 = info->map + j * info->elem_size;
            if (info->is_bg(elem2) || !info->is_same(elem1, elem2)) {
                continue;
            }

            get_pos(info, i, row1, col1);
            get_pos(info, j, row2, col2);
            if (is_linkable(info, *row1, *col1, *row2, *col2)) {
                return FIND_OK;
            }
        }
    }

    return all_bg ? FIND_WIN : FIND_FAILED;
}

void llkfinder_free(struct llk_info *info)
{
    free(info);
}
