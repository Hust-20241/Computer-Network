#include <stdio.h>

#define SIZE 3 // Kích thước của bàn cờ

// Khởi tạo bàn cờ với các giá trị rỗng
void initializeBoard(char board[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            board[i][j] = ' ';
        }
    }
}

// Hiển thị bàn cờ Tic-Tac-Toe
void printBoard(char board[SIZE][SIZE])
{
    printf("\n");
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            printf(" %c ", board[i][j]);
            if (j < SIZE - 1)
            {
                printf("|"); // Vẽ vạch dọc
            }
        }
        printf("\n");
        if (i < SIZE - 1)
        {
            printf("---+---+---\n"); // Vẽ vạch ngang
        }
    }
    printf("\n");
}

// Kiểm tra xem có người thắng hay không
char checkWinner(char board[SIZE][SIZE])
{
    // Kiểm tra các hàng
    for (int i = 0; i < SIZE; i++)
    {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != ' ')
        {
            return board[i][0];
        }
    }

    // Kiểm tra các cột
    for (int i = 0; i < SIZE; i++)
    {
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != ' ')
        {
            return board[0][i];
        }
    }

    // Kiểm tra các đường chéo
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != ' ')
    {
        return board[0][0];
    }
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != ' ')
    {
        return board[0][2];
    }

    return ' '; // Không có ai thắng
}

// Kiểm tra xem bàn cờ có đầy không (hòa)
int isBoardFull(char board[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == ' ')
            {
                return 0; // Vẫn còn ô trống
            }
        }
    }
    return 1; // Bàn cờ đầy
}

// Chơi trò chơi Tic-Tac-Toe giữa hai người chơi
void playGame()
{
    char board[SIZE][SIZE];
    char currentPlayer = 'X'; // Người chơi đầu tiên là X
    int row, col;
    char winner = ' ';

    // Khởi tạo bàn cờ
    initializeBoard(board);

    // Vòng lặp chơi game
    while (1)
    {
        printBoard(board); // Hiển thị bàn cờ

        // Nhập nước đi của người chơi
        printf("Player %c, enter row (1-3) and column (1-3): ", currentPlayer);
        scanf("%d %d", &row, &col);
        row--; // Giảm hàng và cột để khớp với chỉ số mảng
        col--;

        // Kiểm tra tính hợp lệ của nước đi
        if (row < 0 || row >= SIZE || col < 0 || col >= SIZE || board[row][col] != ' ')
        {
            printf("Invalid move. Try again.\n");
            continue;
        }

        // Đặt dấu của người chơi trên bàn cờ
        board[row][col] = currentPlayer;

        // Kiểm tra có ai thắng chưa
        winner = checkWinner(board);
        if (winner != ' ')
        {
            printBoard(board);
            printf("Player %c wins!\n", winner);
            break;
        }

        // Kiểm tra nếu bàn cờ đầy (hòa)
        if (isBoardFull(board))
        {
            printBoard(board);
            printf("It's a tie!\n");
            break;
        }

        // Đổi lượt chơi
        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
    }
}

int main()
{
    playGame();
    return 0;
}
