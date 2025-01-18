#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define BOARD_SIZE 8

// Chessboard definition
char chessboard[BOARD_SIZE][BOARD_SIZE] = {
    {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'},
    {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
    {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
    {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}};

// Function prototypes
int is_valid_move(int from_x, int from_y, int to_x, int to_y, char current_turn);
int is_path_clear(int from_x, int from_y, int to_x, int to_y);
void draw_board(WINDOW *win, int cursor_x, int cursor_y, int selected_x, int selected_y, int is_piece_selected, char current_turn, int valid_moves[BOARD_SIZE][BOARD_SIZE], int valid_paths[BOARD_SIZE][BOARD_SIZE]);
void calculate_valid_moves(int from_x, int from_y, char current_turn, int valid_moves[BOARD_SIZE][BOARD_SIZE], int valid_paths[BOARD_SIZE][BOARD_SIZE]);
void highlight_paths(int from_x, int from_y, char piece, int valid_paths[BOARD_SIZE][BOARD_SIZE]);

// Draw chessboard
void draw_board(WINDOW *win, int cursor_x, int cursor_y, int selected_x, int selected_y, int is_piece_selected, char current_turn, int valid_moves[BOARD_SIZE][BOARD_SIZE], int valid_paths[BOARD_SIZE][BOARD_SIZE])
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

            // Draw the square's background
            mvwprintw(win, y + 1, x * 3 + 3, "   ");

            // Draw the piece with colors
            if (chessboard[y][x] != ' ')
            {
                // Apply the foreground color for the pieces
                if (isupper(chessboard[y][x]))
                {
                    wattron(win, COLOR_PAIR(5)); // White pieces
                }
                else
                {
                    wattron(win, COLOR_PAIR(6)); // Black pieces
                }
                mvwprintw(win, y + 1, x * 3 + 3, " %c ", chessboard[y][x]);
                
                // Turn off piece color
                wattroff(win, COLOR_PAIR(5) | COLOR_PAIR(6));
            }
            // Highlight the selected square
            if (is_piece_selected && y == selected_y && x == selected_x)
            {
                wattron(win, COLOR_PAIR(3) | A_BOLD);
                mvwprintw(win, y + 1, x * 3 + 3, " %c ", chessboard[y][x] ? chessboard[y][x] : ' ');
                wattroff(win, COLOR_PAIR(3) | A_BOLD);
            }

            // Highlight the cursor position
            if (y == cursor_y && x == cursor_x)
            {
                wattron(win, COLOR_PAIR(3));
                mvwprintw(win, y + 1, x * 3 + 3, " %c ", chessboard[y][x] ? chessboard[y][x] : ' ');
                wattroff(win, COLOR_PAIR(3));
            }

            wattroff(win, A_BOLD | COLOR_PAIR(1) | COLOR_PAIR(2));
        }
    }

    // Display current turn
    mvwprintw(win, BOARD_SIZE + 2, 0, "Turn: %s", current_turn == 'w' ? "White" : "Black");
    wrefresh(win);
}

// Check if the path is clear for sliding pieces (rooks, bishops, queens)
int is_path_clear(int from_x, int from_y, int to_x, int to_y)
{
    int dx = (to_x > from_x) - (to_x < from_x); // Step in x direction
    int dy = (to_y > from_y) - (to_y < from_y); // Step in y direction

    int x = from_x + dx, y = from_y + dy;
    while (x != to_x || y != to_y)
    {
        if (chessboard[y][x] != ' ')
        {
            return 0; // Blocked path
        }
        x += dx;
        y += dy;
    }
    return 1;
}

// Validate moves for each piece
int is_valid_move(int from_x, int from_y, int to_x, int to_y, char current_turn)
{
    char piece = chessboard[from_y][from_x];

    if ((current_turn == 'w' && islower(piece)) || (current_turn == 'b' && isupper(piece)))
    {
        return 0; // Cannot move opponent's pieces
    }

    char target = chessboard[to_y][to_x];
    if ((current_turn == 'w' && isupper(target)) || (current_turn == 'b' && islower(target)))
    {
        return 0; // Cannot capture own pieces
    }

    int dx = abs(to_x - from_x);
    int dy = abs(to_y - from_y);

    switch (tolower(piece))
    {
    case 'p': // Pawn movement
        if (current_turn == 'w')
        {
            if (to_x == from_x && to_y == from_y - 1 && target == ' ')
                return 1; // Forward move
            if (to_x == from_x && to_y == from_y - 2 && from_y == 6 && target == ' ' && chessboard[from_y - 1][from_x] == ' ')
                return 1; // Two-square move
            if (dx == 1 && to_y == from_y - 1 && target != ' ')
                return 1; // Capture
        }
        else
        {
            if (to_x == from_x && to_y == from_y + 1 && target == ' ')
                return 1; // Forward move
            if (to_x == from_x && to_y == from_y + 2 && from_y == 1 && target == ' ' && chessboard[from_y + 1][from_x] == ' ')
                return 1; // Two-square move
            if (dx == 1 && to_y == from_y + 1 && target != ' ')
                return 1; // Capture
        }
        break;

    case 'r': // Rook
        if ((dx == 0 || dy == 0) && is_path_clear(from_x, from_y, to_x, to_y))
            return 1;
        break;

    case 'n': // Knight
        if ((dx == 2 && dy == 1) || (dx == 1 && dy == 2))
            return 1;
        break;

    case 'b': // Bishop
        if (dx == dy && is_path_clear(from_x, from_y, to_x, to_y))
            return 1;
        break;

    case 'q': // Queen
        if (((dx == 0 || dy == 0) || dx == dy) && is_path_clear(from_x, from_y, to_x, to_y))
            return 1;
        break;

    case 'k': // King
        if (dx <= 1 && dy <= 1)
            return 1;
        break;
    }

    return 0; // Invalid move
}

// Calculate valid moves for a piece
void calculate_valid_moves(int from_x, int from_y, char current_turn, int valid_moves[BOARD_SIZE][BOARD_SIZE], int valid_paths[BOARD_SIZE][BOARD_SIZE])
{
    // First, reset the valid paths array
    memset(valid_paths, 0, sizeof(int) * BOARD_SIZE * BOARD_SIZE);

    // Loop through the board and calculate valid moves for each square
    for (int y = 0; y < BOARD_SIZE; y++)
    {
        for (int x = 0; x < BOARD_SIZE; x++)
        {
            valid_moves[y][x] = is_valid_move(from_x, from_y, x, y, current_turn);
        }
    }

    // Highlight the valid paths for sliding pieces (rooks, bishops, queens)
    char piece = chessboard[from_y][from_x];
    highlight_paths(from_x, from_y, piece, valid_paths);
}

// Highlight the paths for sliding pieces
void highlight_paths(int from_x, int from_y, char piece, int valid_paths[BOARD_SIZE][BOARD_SIZE])
{
    int dx, dy;

    if (tolower(piece) == 'r' || tolower(piece) == 'q')
    {
        // Rook and Queen: Horizontal and vertical lines
        for (dy = -1; dy <= 1; dy++)
        {
            if (dy == 0)
                continue;
            for (int i = from_y + dy; i >= 0 && i < BOARD_SIZE; i += dy)
            {
                valid_paths[i][from_x] = 1;
                if (chessboard[i][from_x] != ' ')
                    break;
            }
        }
    }
    if (tolower(piece) == 'b' || tolower(piece) == 'q')
    {
        // Bishop and Queen: Diagonal lines
        for (dx = -1; dx <= 1; dx++)
        {
            if (dx == 0)
                continue;
            for (int i = from_x + dx, j = from_y + dx; i >= 0 && j >= 0 && i < BOARD_SIZE && j < BOARD_SIZE; i += dx, j += dx)
            {
                valid_paths[j][i] = 1;
                if (chessboard[j][i] != ' ')
                    break;
            }
        }
    }
}

int main()
{
    int cursor_x = 0, cursor_y = 0;
    int selected_x = -1, selected_y = -1; // No piece selected initially
    int is_piece_selected = 0;
    char current_turn = 'w'; // 'w' for White, 'b' for Black
    int valid_moves[BOARD_SIZE][BOARD_SIZE] = {{0}};
    int valid_paths[BOARD_SIZE][BOARD_SIZE] = {{0}};

    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();

    // Initialize color pairs
    init_pair(1, COLOR_BLACK, COLOR_WHITE);   // Light squares
    init_pair(2, COLOR_WHITE, COLOR_BLACK);   // Dark squares
    init_pair(3, COLOR_BLACK, COLOR_YELLOW);  // Highlighted piece
    init_pair(4, COLOR_RED, COLOR_BLACK);     // Highlight valid paths
    init_pair(5, COLOR_CYAN, COLOR_BLACK);    // White pieces
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK); // Black pieces

    int term_height, term_width;
    getmaxyx(stdscr, term_height, term_width);
    int start_y = (term_height - BOARD_SIZE - 2) / 2;
    int start_x = (term_width - BOARD_SIZE * 3 - 4) / 2;

    WINDOW *board_win = newwin(BOARD_SIZE + 3, BOARD_SIZE * 3 + 5, start_y, start_x);

    draw_board(board_win, cursor_x, cursor_y, selected_x, selected_y, is_piece_selected, current_turn, valid_moves, valid_paths);

    int ch;
    while ((ch = getch()) != 'q')
    {
        switch (ch)
        {
        case KEY_UP:
            if (cursor_y > 0)
                cursor_y--;
            break;
        case KEY_DOWN:
            if (cursor_y < BOARD_SIZE - 1)
                cursor_y++;
            break;
        case KEY_LEFT:
            if (cursor_x > 0)
                cursor_x--;
            break;
        case KEY_RIGHT:
            if (cursor_x < BOARD_SIZE - 1)
                cursor_x++;
            break;
        case '\n': // Enter key
            if (!is_piece_selected && chessboard[cursor_y][cursor_x] != ' ')
            {
                selected_x = cursor_x;
                selected_y = cursor_y;
                is_piece_selected = 1;
                calculate_valid_moves(selected_x, selected_y, current_turn, valid_moves, valid_paths);
            }
            else if (is_piece_selected)
            {
                if (cursor_y == selected_y && cursor_x == selected_x)
                {
                    // Deselect the piece if the same square is selected again
                    is_piece_selected = 0;
                    memset(valid_moves, 0, sizeof(valid_moves)); // Clear valid moves
                    memset(valid_paths, 0, sizeof(valid_paths)); // Clear valid paths
                }
                else if (valid_moves[cursor_y][cursor_x])
                {
                    chessboard[cursor_y][cursor_x] = chessboard[selected_y][selected_x];
                    chessboard[selected_y][selected_x] = ' ';
                    current_turn = (current_turn == 'w') ? 'b' : 'w'; // Switch turns
                }
                is_piece_selected = 0;
                memset(valid_moves, 0, sizeof(valid_moves)); // Clear valid moves
                memset(valid_paths, 0, sizeof(valid_paths)); // Clear valid paths
            }
            break;
        }
        draw_board(board_win, cursor_x, cursor_y, selected_x, selected_y, is_piece_selected, current_turn, valid_moves, valid_paths);
    }

    delwin(board_win);
    endwin();
    return 0;
}
