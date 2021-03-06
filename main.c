#include <stddef.h>
#include <stdlib.h>
#include "field.h"
#include <ncurses.h>
#include <string.h>

    void
print_instr (void)
{
    attron (COLOR_PAIR (11));
    printw ("'h', 'j', 'k', 'l' to move. \n'r' or <space> to Reveal.\n'f' to Flag.\n'q' to quit.\n");
    attroff (COLOR_PAIR (11));
}

    void
print_help (void)
{
    printf ("Usage: minesweeper [OPTION] [Rows Columns Mines]\n");
    printf ("\tMinesweeper made with ncurses and terminal colors.\n");
    printf ("Example: minesweeper 20 20 60\n");
    printf ("Options:\n");
    printf ("\tRows Columns Mines,\t\tDefaults to 16x16 with 40 mines.\n");
    printf ("\t-h, --help\t\t\tShows this super helpful information.\n");
}

    int
main (int argc, char *argv[])
{
    int rows = 16;
    int cols = 16;
    int bombs = 40;

    if (argc > 1)
    {
        if (strcmp (argv[1], "-h") == 0 || strcmp (argv[1], "--help") == 0)
        {
            print_help ();
            return EXIT_SUCCESS;
        }
        rows = atoi (argv[1]);
        cols = atoi (argv[2]);
        bombs = atoi (argv[3]);
    }

    /* Initialize Curses. */
    initscr ();
    clear ();
    cbreak ();
    keypad (stdscr, TRUE);
    start_color ();
    noecho ();
    curs_set (0);

    /* Define Colors. */
    init_pair (1, COLOR_BLACK, COLOR_WHITE);
    init_pair (2, COLOR_BLACK, COLOR_WHITE + 8);
    init_pair (3, COLOR_BLACK, COLOR_BLUE); 
    init_pair (4, COLOR_BLACK, COLOR_BLUE + 8); 
    init_pair (5, COLOR_BLACK, COLOR_GREEN);
    init_pair (6, COLOR_BLACK, COLOR_GREEN + 8);
    init_pair (7, COLOR_BLACK, COLOR_RED);
    init_pair (8, COLOR_BLACK, COLOR_RED + 8);
    init_pair (9, COLOR_BLACK, COLOR_MAGENTA);
    init_pair (10, COLOR_BLACK, COLOR_MAGENTA + 8);
    init_pair (11, COLOR_WHITE, COLOR_BLACK);

Play:

    /* Welcome the User. */
    attron (COLOR_PAIR (1));
    printw ("Welcome to Minesweeper!\n");
    attroff (COLOR_PAIR (1));

    /* Build Minefield. */
    field_t field;
    field_t visible;
    init_field (rows, cols, &field);
    init_field (rows, cols, &visible);
    add_bombs (bombs, &field);
    visible.bombs = bombs;

    coord_t cursor = { 0, 0 };

    /* Draw Field. */
    clear ();
    print_field (&visible);
    print_instr ();
    print_cursor (cursor, &visible);
    refresh ();

    /* Wait for Input. */
    bool running = true;
    bool alive = true;
    bool won = false;
    int ch;
    ch = getchar ();
    while (running)
    {
        coord_t old_cursor = cursor;
        switch (ch)
        {
            case 'h':
                cursor.c--; 
                break;
            case 'j':
                cursor.r++;
                break;
            case 'k':
                cursor.r--;
                break;
            case 'l':
                cursor.c++;
                break;
            case 'r':
            case ' ':
                alive = reveal (cursor, &visible, &field);
                break;
            case 'f':
                setflag (cursor, &visible);
                if (visible.flags == field.bombs)
                    won = winner (&visible, &field);
                break;
            case 'q': 
                alive = false;
                break;
        }
        if (!in_bounds (cursor, &field))
            cursor = old_cursor;
        clear ();
        print_field (&visible);
        if (!alive)
            {
                print_failed (&visible, &field);
                attron (COLOR_PAIR (11));
                move (2 + rows, 0);
                printw ("YOU LOSE!\n'q' to quit.\nAny key to play again.\n");
                running = false;
                attroff (COLOR_PAIR (11));
            }
        else if (won)
            {
                print_failed (&visible, &field);
                attron (COLOR_PAIR (11));
                move (2 + rows, 0);
                printw ("YOU WIN!\n'q' to quit.\nAny key to play again.\n");
                running = false;
                attroff (COLOR_PAIR (11));
            }
        else
            {
                print_instr ();
                print_cursor (cursor, &visible);
            }
        refresh ();
        ch = getchar ();
    }

    /* Bye! */
    clear ();
    free_field (&visible);
    free_field (&field);
    if (ch != 'q')
        goto Play;
    endwin ();
    return EXIT_SUCCESS; 
}
