#include <ncurses.h>

#define BOARD_SIZE 8

// define chessboard
char chessboard[BOARD_SIZE][BOARD_SIZE] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

// draw board
void draw_board(WINDOW *win, int cursor_x, int cursor_y)
{
    wclear(win);
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            if (y == cursor_y && x == cursor_x)
            {
                wattron(win, A_REVERSE); // highlight the cursor position
            }
            mvwprintw(win, y, x * 2, "%c", chessboard[y][x]);
            wattroff(win, A_REVERSE);
        }
    }
    wrefresh(win);
}

int main()
{
    int cursor_x = 0, cursor_y = 0;

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    // create a window for the chessboard
    WINDOW *board_win = newwin(BOARD_SIZE, BOARD_SIZE * 2, 1, 1);

    // main loop
    int ch;
    while ((ch = getch()) != 'q')
    {
        switch (ch)
        {
        case KEY_UP:
            if (cursor_y > 0)
            {
                cursor_y--;
            }
            break;
        case KEY_DOWN:
            if (cursor_y < BOARD_SIZE - 1)
            {
                cursor_y++;
            }
            break;
        case KEY_LEFT:
            if (cursor_x > 0)
            {
                cursor_x--;
            }
            break;
        case KEY_RIGHT:
            if (cursor_x < BOARD_SIZE - 1)
            {
                cursor_x++;
            }
            break;
        }
        draw_board(board_win, cursor_x, cursor_y);
    }

    // cleanup
    delwin(board_win);
    endwin();
    return 0;
}