#include <ncurses.h>

#define BOARD_SIZE 8

// define chessboard with Unicode piece
char chessboard[BOARD_SIZE][BOARD_SIZE] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}
};


// draw board
void draw_board(WINDOW *win, int cursor_x, int cursor_y)
{
    wclear(win);

    // Draw column labels (A-H)
    mvwprintw(win, 0, 4, "  A  B  C  D  E  F  G  H");

// Draw the chessboard with row labels (1-8)
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        mvwprintw(win, y + 1, 0, "%d ", 8 - y); // Row labels (8-1)

        for (int x = 0; x < BOARD_SIZE; x++)
        {
            // Determine square color
            if ((x + y) % 2 == 0) {
                wattron(win, COLOR_PAIR(1)); // Light squares
            } else {
                wattron(win, COLOR_PAIR(2)); // Dark squares
            }
            // Highlight the cursor position
            if (y == cursor_y && x == cursor_x)
            {
                wattron(win, A_REVERSE); 
            }
            // Draw the piece or empty square
            mvwprintw(win, y + 1, x * 3 + 3, " %c ", chessboard[y][x]);
            wattroff(win, A_REVERSE);
            wattroff(win, COLOR_PAIR(1));
            wattroff(win, COLOR_PAIR(2));
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
    start_color();

    // Initialize color pairs
    init_pair(1, COLOR_BLACK, COLOR_WHITE); // Light squares
    init_pair(2, COLOR_WHITE, COLOR_BLACK); // Dark squares


    // create a window for the chessboard
    WINDOW *board_win = newwin(BOARD_SIZE + 2, BOARD_SIZE * 3 + 4, 1, 1);

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