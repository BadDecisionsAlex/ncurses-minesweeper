#include "field.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

    void
init_field (size_t r, size_t c, field_t *f)
{
    f->rows = r;
    f->cols = c;
    f->bombs = 0;
    f->flags = 0;
    f->matrix = (int *) calloc (r, c * sizeof (int));
    memset (f->matrix, 0, r*c);
    assert (f->matrix != NULL);
}

    void
free_field (field_t *f)
{
    assert (f != NULL);
    free (f->matrix);
}

    int
getat (coord_t c, field_t *f)
{
    assert (f != NULL);
    assert (in_bounds (c, f));
    return f->matrix[(c.r * f->cols) + c.c];
}

    void
setat (coord_t c, int v, field_t *f)
{
    assert (f != NULL);
    assert (in_bounds (c, f));
    f->matrix[(c.r * f->cols) + c.c] = v;
}

    void
add_bombs (int b, field_t *f)
{
    assert (f != NULL);
    srand (clock ());
    int i;
    for (i = 0; i < b; ++i)
    {
        coord_t p = { random () % f->rows, random () % f->cols};
        if (getat (p, f) != BOMB)
        {
            setat (p, BOMB, f);
            bump_at (p, f);
        }
        else
            --i;
    }
    f->bombs = b;
}
    bool
in_bounds (coord_t c, field_t *f)
{
    if (c.r < 0)
        return false;
    if (c.c < 0)
        return false;
    if (c.c >= f->cols)
        return false;
    if (c.r >= f->rows)
        return false;
    return true;
}

    void
bump_at (coord_t c, field_t *f)
{
   coord_t up = { c.r + 1, c.c }; 
   coord_t down = { c.r - 1, c.c }; 
   coord_t left = { c.r, c.c - 1 }; 
   coord_t right = { c.r, c.c + 1 }; 
   coord_t up_left = { c.r + 1, c.c - 1 }; 
   coord_t down_left = { c.r - 1, c.c - 1 }; 
   coord_t up_right = { c.r + 1, c.c + 1 }; 
   coord_t down_right = { c.r - 1, c.c + 1 }; 
   if (in_bounds (up, f) && getat (up, f) != BOMB)
       setat (up, getat (up, f) + 1, f);
   if (in_bounds (down, f) && getat (down, f) != BOMB)
       setat (down, getat (down, f) + 1, f);
   if (in_bounds (left, f) && getat (left, f) != BOMB)
       setat (left, getat (left, f) + 1, f);
   if (in_bounds (right, f) && getat (right, f) != BOMB)
       setat (right, getat (right, f) + 1, f);
   if (in_bounds (up_left, f) && getat (up_left, f) != BOMB)
       setat (up_left, getat (up_left, f) + 1, f);
   if (in_bounds (up_right, f) && getat (up_right, f) != BOMB)
       setat (up_right, getat (up_right, f) + 1, f);
   if (in_bounds (down_left, f) && getat (down_left, f) != BOMB)
       setat (down_left, getat (down_left, f) + 1, f);
   if (in_bounds (down_right, f) && getat (down_right, f) != BOMB)
       setat (down_right, getat (down_right, f) + 1, f);
}

    void
print_field (field_t *f)
{
    assert (f != NULL);
    attron (COLOR_PAIR (11));
    printw ("Rows: %d x Cols: %d\n", (int) f->rows, (int) f->cols);
    printw ("Flags: %d, Mines: %d\n", f->flags, f->bombs);
    attroff (COLOR_PAIR (11));

    int i, j;
    for (i = 0; i < f->rows; ++i)
    {
        for (j = 0; j < f->cols; ++j)
        {
            coord_t c = {i, j};
            int v = getat (c, f);
            switch (v)
            {
                case BOMB:
                    attron(COLOR_PAIR (8)); // Red-2
                    printw (" B");
                    break;
                case 0:
                    attron (COLOR_PAIR ((i + j)%2 + 1)); // White
                    printw ("  ");
                    break;
                case EMPTY:
                    attron (COLOR_PAIR ((i + j)%2 + 5)); // Green
                    printw ("  ");
                    break;
                case FLAG:
                    attron (COLOR_PAIR (7)); // Red
                    printw (" F");
                    break;
                default:
                    attron (COLOR_PAIR ((i + j)%2 + 5)); // Green
                    printw (" %d", v);
                    break;
            }
            attroff (COLOR_PAIR (1));
        }
        printw ("\n");
    }
}

    void
print_cursor (coord_t c, field_t *f)
{
    //printw ("Cursor Row: %d, Col: %d\n", c.r, c.c);
    move (c.r + 2, c.c * 2); 
    int v = getat (c, f);
    switch (v)
    {
        case BOMB:
            attron(COLOR_PAIR (7)); // Red
            printw (" B");
        case 0:
            attron (COLOR_PAIR (3)); // Blue
            printw ("  ");
            break;
        case EMPTY:
            attron (COLOR_PAIR (9)); // Magenta
            printw ("  ");
            break;
        case FLAG:
            attron (COLOR_PAIR (8)); // Red
            printw (" F");
            break;
        default:
            attron (COLOR_PAIR (10)); // Magenta
            printw (" %d", v);
            break;
    }
    attroff (COLOR_PAIR (1));
}

    bool
reveal (coord_t c, field_t *v, field_t *h)
{
    if (!in_bounds (c, h) || getat (c, v) != 0)
        return true;

    int val = getat (c, h);

    if (val == 0) {
        setat (c, EMPTY, v);
        coord_t up = { c.r + 1, c.c }; 
        coord_t down = { c.r - 1, c.c }; 
        coord_t left = { c.r, c.c - 1 }; 
        coord_t right = { c.r, c.c + 1 }; 
        coord_t up_left = { c.r + 1, c.c - 1 }; 
        coord_t down_left = { c.r - 1, c.c - 1 }; 
        coord_t up_right = { c.r + 1, c.c + 1 }; 
        coord_t down_right = { c.r - 1, c.c + 1 }; 
        reveal (up, v, h);
        reveal (down, v, h);
        reveal (left, v, h);
        reveal (right, v, h);
        reveal (up_left, v, h);
        reveal (down_left, v, h);
        reveal (up_right, v, h);
        reveal (down_right, v, h);
        return true;
    }

    setat (c, val, v);

    if (val == BOMB)
        return false;
    return true;
}

    void
setflag (coord_t c, field_t *f)
{
    assert (f != NULL);
    assert (in_bounds (c, f));
    int v = getat (c, f);
    if (v == FLAG)
        {
            f->flags --;
            setat (c, 0, f);
        }
    else if (v == 0)
        {
            f->flags++;
            setat (c, FLAG, f);
        }
}

    bool
winner (field_t *v, field_t *h)
{
    assert (v != NULL);
    assert (h != NULL);
    assert (v->cols == h->cols);
    assert (v->rows == h->rows);
    assert (v->bombs == h->bombs);
    if (v->flags != h->bombs)
        return false;
    int i, j;
    for (i = 0; i < v->cols; ++i)
    {
        for (j = 0; j < v->rows; ++j)
        {
            coord_t c = {i, j};
            if (getat (c, v) == FLAG && getat (c, h) != BOMB)
                return false;
        }
    }
    return true;
}

    void
print_failed (field_t *v, field_t *h)
{
    assert (v != NULL);
    assert (h != NULL);
    assert (v->cols == h->cols);
    assert (v->rows == h->rows);
    assert (v->bombs == h->bombs);
    clear ();
    print_field (v);
    int i, j;
    for (i = 0; i < v->cols; ++i)
    {
        for (j = 0; j < v->rows; ++j)
        {
            coord_t c = {i, j};
            move (c.r + 2, c.c * 2);
            int vis = getat (c, v);
            int hid = getat (c, h);
            switch (hid)
            {
                case BOMB:
                    if (vis == FLAG)
                        {
                            attron (COLOR_PAIR (4));
                            printw (" F");
                        }
                    else if (vis == 0)
                        {
                            attron (COLOR_PAIR (10));
                            printw (" B");
                        }
                    break;
                default:
                    if (vis == FLAG)
                        {
                            attron (COLOR_PAIR (7));
                            printw ("xF");
                        } 
                    break;
            }
            attroff (COLOR_PAIR (1));
        }
    }
}
