#include <iostream>
#include <vector>
#include <limits>

using namespace std;

const int BOARD_SIZE = 6;
const int EMPTY = 0;
const int BLACK = 1;
const int WHITE = 2;

// 定義Othello棋盤類別
class OthelloBoard {
private:
    int board[BOARD_SIZE][BOARD_SIZE];

public:
    OthelloBoard(const string& gameboard) {
        // 初始化棋盤
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                char c = gameboard[i * BOARD_SIZE + j];
                if (c == 'O')
                    board[i][j] = WHITE;
                else if (c == 'X')
                    board[i][j] = BLACK;
                else
                    board[i][j] = EMPTY;
            }
        }
    }

    // 檢查座標(x, y)是否在合法範圍內
    bool isValidMove(int x, int y) const {
        return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
    }

    // 檢查座標(x, y)是否為空格
    bool isEmpty(int x, int y) const {
        return board[x][y] == EMPTY;
    }

    // 在座標(x, y)放置棋子
    bool makeMove(int x, int y, int player) {
        if (!isEmpty(x, y))
            return false;

        // 檢查座標周圍是否有其他棋子
        bool hasNeighbor = false;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0)
                    continue;

                int nx = x + dx;
                int ny = y + dy;

                if (isValidMove(nx, ny) && !isEmpty(nx, ny)) {
                    hasNeighbor = true;
                    break;
                }
            }
            if (hasNeighbor)
                break;
        }

        if (!hasNeighbor)
            return false;

        board[x][y] = player;
        return true;
    }




    // 反轉指定座標周圍的棋子
    void flipTiles(int x, int y, int player) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                if (dx == 0 && dy == 0)
                    continue;

                int nx = x + dx;
                int ny = y + dy;

                while (isValidMove(nx, ny) && board[nx][ny] != EMPTY && board[nx][ny] != player) {
                    nx += dx;
                    ny += dy;
                }

                if (isValidMove(nx, ny) && board[nx][ny] == player) {
                    int tx = x + dx;
                    int ty = y + dy;

                    while (tx != nx || ty != ny) {
                        board[tx][ty] = player;
                        tx += dx;
                        ty += dy;
                    }
                }
            }
        }
    }

    // 取得指定座標的棋子
    int getPiece(int x, int y) const {
        return board[x][y];
    }

    // 評估棋盤局勢
    int evaluate(int player) const {
        int score = 0;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (board[i][j] == player)
                    score++;
                else if (board[i][j] == 3 - player)
                    score--;
            }
        }
        return score;
    }

    // 取得所有合法移動
    vector<pair<int, int>> getValidMoves(int player) const {
        vector<pair<int, int>> validMoves;

        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (isEmpty(i, j)) {
                    OthelloBoard tempBoard(*this);
                    if (tempBoard.isValidMove(i, j) && tempBoard.makeMove(i, j, player))
                        validMoves.emplace_back(i, j);
                }
            }
        }

        return validMoves;
    }

    // NegaScout演算法
    int negaScout(int depth, int alpha, int beta, int player) {
        if (depth == 0 || isGameOver())
            return evaluate(player);

        vector<pair<int, int>> validMoves = getValidMoves(player);

        if (validMoves.empty())
            return negaScout(depth - 1, alpha, beta, 3 - player);

        int b = beta;
        for (const auto& move : validMoves) {
            OthelloBoard childBoard(*this);
            childBoard.makeMove(move.first, move.second, player);
            childBoard.flipTiles(move.first, move.second, player);

            int score = -childBoard.negaScout(depth - 1, -b, -alpha, 3 - player);

            if (score > alpha) {
                if (b == beta || depth <= 2) {
                    alpha = score;
                }
                else {
                    alpha = -childBoard.negaScout(depth - 1, -beta, -score, 3 - player);
                }
            }

            if (alpha >= beta)
                return alpha;

            b = alpha + 1;
        }

        return alpha;
    }

    // 檢查遊戲是否結束
    bool isGameOver() const {
        vector<pair<int, int>> validMovesBlack = getValidMoves(BLACK);
        vector<pair<int, int>> validMovesWhite = getValidMoves(WHITE);

        return validMovesBlack.empty() && validMovesWhite.empty();
    }

    // 找出最佳移動
    pair<int, int> findBestMove(int player, int depth) {
        vector<pair<int, int>> validMoves = getValidMoves(player);

        int bestScore = numeric_limits<int>::min();
        pair<int, int> bestMove;

        for (const auto& move : validMoves) {
            OthelloBoard childBoard(*this);
            childBoard.makeMove(move.first, move.second, player);
            childBoard.flipTiles(move.first, move.second, player);

            int score = -childBoard.negaScout(depth - 1, numeric_limits<int>::min(), numeric_limits<int>::max(), 3 - player);

            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }

        return bestMove;
    }
};

int main() {
    string gameboard;
    int currentPlayer;
    int depth;

    cout << "輸入棋盤局面:" << endl;
    cin >> gameboard;
    cout << "輸入執子玩家(黑方X為1、白方O為2):" << endl;
    cin >> currentPlayer;
    cout << "輸入搜尋深度:" << endl;
    cin >> depth;

    OthelloBoard board(gameboard);
    pair<int, int> bestMove = board.findBestMove(currentPlayer, depth);

    cout << "Best Move: (" << bestMove.first << "," << bestMove.second << ")" << endl;//first 為縱列，second為橫列

    // 將最佳移動應用到棋盤上
    board.makeMove(bestMove.first, bestMove.second, currentPlayer);
    board.flipTiles(bestMove.first, bestMove.second, currentPlayer);

    // 輸出更新後的棋盤情勢
    cout << "棋盤情勢：" << endl;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int piece = board.getPiece(i, j);
            if (piece == EMPTY)
                cout << "+";
            else if (piece == BLACK)
                cout << "X";
            else if (piece == WHITE)
                cout << "O";
        }
        cout << endl;
    }

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int piece = board.getPiece(i, j);
            if (piece == EMPTY)
                cout << "+";
            else if (piece == BLACK)
                cout << "X";
            else if (piece == WHITE)
                cout << "O";
        }
    }

    return 0;
}

