# llkfinder
A tiny library for finding two linkable elements of 2D Lian Lian Kan game

## Usage

```C
// Please refer to llkfinder.h for the parameters information, and
// test/test.c for detailed usage
struct llk_info *info = llkfinder_init(rows, cols, 1, map, sizeof(map[0]),
                                       is_bg, is_same);

int row1, col1, row2, col2;
int result = llkfinder_find(info, &row1, &col1, &row2, &col2);

llkfinder_free(info);
```

## Reference

[http://blog.kery.top/tech/brief-analysis-of-lianliankan-algorithm.html](http://blog.kery.top/tech/brief-analysis-of-lianliankan-algorithm.html)
