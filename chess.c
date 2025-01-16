#include <ncurses.h>
#include <ctype.h>

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
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

// draw board
void draw_board(WINDOW *win, int cursor_x, int cursor_y, int selected_x, int selected_y, int is_piece_selected)
{
    wclear(win);

    // Draw column labels (A-H)
    mvwprintw(win, 0, 4, "A  B  C  D  E  F  G  H");

    // Draw the chessboard with row labels (1-8)
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        mvwprintw(win, y + 1, 0, "%d ", 8 - y); // Row labels (8-1)

        for (int x = 0; x < BOARD_SIZE; x++)
        {
            // Determine square color
            if ((x + y) % 2 == 0)
            {
                wattron(win, COLOR_PAIR(1)); // Light squares
            }
            else
            {
                wattron(win, COLOR_PAIR(2)); // Dark squares
            }
            // Highlight the selected square
            if (is_piece_selected && y == selected_y && x == selected_x)
            {
                wattron(win, A_BOLD | COLOR_PAIR(3));
            }

            // Highlight the cursor position (distinctly if a piece is selected)
            if (y == cursor_y && x == cursor_x)
            {
                if (is_piece_selected)
                {
                    wattron(win, COLOR_PAIR(4));
                }
                else
                {
                    wattron(win, A_REVERSE);
                }
            }

            // Draw the piece with colors for white and black pieces
            if (chessboard[y][x] != ' ')
            {
                if (isupper(chessboard[y][x]))
                    wattron(win, COLOR_PAIR(5)); // White pieces
                else
                    wattron(win, COLOR_PAIR(6)); // Black pieces
                mvwprintw(win, y + 1, x * 3 + 3, " %c ", chessboard[y][x]);
                wattroff(win, COLOR_PAIR(5));
                wattroff(win, COLOR_PAIR(6));
            }
            else
            {
                mvwprintw(win, y + 1, x * 3 + 3, "   ");
            }

            wattroff(win, A_BOLD | A_REVERSE | COLOR_PAIR(1) | COLOR_PAIR(2) | COLOR_PAIR(3) | COLOR_PAIR(4));
        }
    }
    wrefresh(win);
}

int main()
{
    int cursor_x = 0, cursor_y = 0;
    int selected_x = -1, selected_y = -1; // No piece selected initially
    int is_piece_selected = 0;

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();

    // Initialize color pairs
     init_pair(1, COLOR_BLACK, COLOR_WHITE);  // Light squares
    init_pair(2, COLOR_WHITE, COLOR_BLACK);  // Dark squares
    init_pair(3, COLOR_BLACK, COLOR_YELLOW); // Highlighted piece
    init_pair(4, COLOR_BLACK, COLOR_RED);    // Highlighted square (cursor)
    init_pair(5, COLOR_CYAN, COLOR_BLACK);   // White pieces
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK); // Black pieces

    // Calculate terminal size and center the board
    int term_height, term_width;
    getmaxyx(stdscr, term_height, term_width);

    int board_height = BOARD_SIZE + 2;    // Rows + labels
    int board_width = BOARD_SIZE * 3 + 4; // Columns + labels
    int start_y = (term_height - board_height) / 2;
    int start_x = (term_width - board_width) / 2;

    // Create a window for the chessboard
    WINDOW *board_win = newwin(board_height, board_width, start_y, start_x);

    // main loop
    draw_board(board_win, cursor_x, cursor_y, selected_x, selected_y, is_piece_selected);

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
        case '\n': // Enter key
            if (!is_piece_selected && chessboard[cursor_y][cursor_x] != ' ')
            {
                // Select the piece
                selected_x = cursor_x;
                selected_y = cursor_y;
                is_piece_selected = 1;
            }
            else if (is_piece_selected)
            {
                // Move the piece
                chessboard[cursor_y][cursor_x] = chessboard[selected_y][selected_x];
                chessboard[selected_y][selected_x] = ' ';
                is_piece_selected = 0;
            }
            break;
        }
        draw_board(board_win, cursor_x, cursor_y, selected_x, selected_y, is_piece_selected);
    }

    // cleanup
    delwin(board_win);
    endwin();
    return 0;
}