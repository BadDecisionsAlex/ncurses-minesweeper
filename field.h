#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

#define BOMB INT_MAX
#define EMPTY -1
#define FLAG -2

typedef struct
    {
        size_t rows;
        size_t cols;
        int *matrix;
        int bombs;
        int flags;
    } field_t;

typedef struct
    {
        int r;
        int c;
    } coord_t;

void init_field (size_t, size_t, field_t *);
void free_field (field_t *);
void add_bombs (int, field_t *);
void print_field (field_t *);
void print_cursor (coord_t, field_t *);
void print_failed (field_t *visible, field_t *hidden);
int getat (coord_t, field_t *);
void setat (coord_t, int, field_t *);
void bump_at (coord_t, field_t *);
void setflag (coord_t, field_t *);
bool in_bounds (coord_t, field_t *);
bool reveal (coord_t, field_t *visible, field_t *hidden);
bool winner (field_t *visible, field_t *hidden);
