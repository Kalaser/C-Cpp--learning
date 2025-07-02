#include <graphics.h>
#include <stdio.h>

// 棋盘数据结构 - Use constants for better performance
constexpr int BOARD_SIZE = 3;
constexpr int CELL_SIZE = 200;
constexpr int WINDOW_SIZE = 600;
constexpr int TARGET_FPS = 60;
constexpr int FRAME_TIME = 1000 / TARGET_FPS;

char chboard_date[BOARD_SIZE][BOARD_SIZE] = {
    {'_', '_', '_'},
    {'_', '_', '_'},
    {'_', '_', '_'},
};

// 标识当前玩家棋子
char current_piece = 'O';

// 优化的胜利检查函数 - Consolidated logic for better performance
bool Checkwin(char c) {
    // Check rows
    for (int i = 0; i < BOARD_SIZE; ++i) {
        if (chboard_date[i][0] == c && chboard_date[i][1] == c && chboard_date[i][2] == c) {
            return true;
        }
    }
    
    // Check columns
    for (int j = 0; j < BOARD_SIZE; ++j) {
        if (chboard_date[0][j] == c && chboard_date[1][j] == c && chboard_date[2][j] == c) {
            return true;
        }
    }
    
    // Check diagonals
    if ((chboard_date[0][0] == c && chboard_date[1][1] == c && chboard_date[2][2] == c) ||
        (chboard_date[0][2] == c && chboard_date[1][1] == c && chboard_date[2][0] == c)) {
        return true;
    }
    
    return false;
}

// 检查是否平局 - Optimized with early return
bool CheckDraw() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (chboard_date[i][j] == '_') {
                return false; // Found empty cell, not a draw
            }
        }
    }
    return true;
}

// 绘制棋盘网格 - Use constants for better maintainability
void DrawBoard() {
    // Horizontal lines
    for (int i = 1; i < BOARD_SIZE; ++i) {
        const int y = i * CELL_SIZE;
        line(0, y, WINDOW_SIZE, y);
    }
    
    // Vertical lines  
    for (int i = 1; i < BOARD_SIZE; ++i) {
        const int x = i * CELL_SIZE;
        line(x, 0, x, WINDOW_SIZE);
    }
}

// 绘制棋子 - Optimized with constants and reduced calculations
void DrawPiece() {
    constexpr int RADIUS = CELL_SIZE / 2 - 10; // Circle radius
    constexpr int CENTER_OFFSET = CELL_SIZE / 2; // Center offset
    
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            const int centerX = j * CELL_SIZE + CENTER_OFFSET;
            const int centerY = i * CELL_SIZE + CENTER_OFFSET;
            
            switch (chboard_date[i][j]) {
            case 'O':
                circle(centerX, centerY, RADIUS);
                break;
            case 'X': {
                const int margin = 20;
                const int left = j * CELL_SIZE + margin;
                const int top = i * CELL_SIZE + margin;
                const int right = (j + 1) * CELL_SIZE - margin;
                const int bottom = (i + 1) * CELL_SIZE - margin;
                
                line(left, top, right, bottom);
                line(right, top, left, bottom);
                break;
            }
            case '_':
                // Empty cell, no drawing needed
                break;
            }
        }
    }
}

// 绘制提示信息 - Optimized string handling
void DrawTipText() {
    static TCHAR str[64];
    _stprintf_s(str, _T("当前玩家棋子: %c"), current_piece);

    settextcolor(RGB(224, 175, 45));
    outtextxy(0, 0, str);
}

// 切换玩家 - Extract function to reduce code duplication
void SwitchPlayer() {
    current_piece = (current_piece == 'O') ? 'X' : 'O';
}

// 检查游戏结束状态
enum class GameState {
    CONTINUE,
    X_WINS,
    O_WINS,
    DRAW
};

GameState CheckGameState() {
    if (Checkwin('X')) return GameState::X_WINS;
    if (Checkwin('O')) return GameState::O_WINS;
    if (CheckDraw()) return GameState::DRAW;
    return GameState::CONTINUE;
}

int main() {
    initgraph(WINDOW_SIZE, WINDOW_SIZE);

    bool running = true;
    ExMessage msg;

    BeginBatchDraw();
    DWORD start_time = GetTickCount();

    while (running) {
        // 处理消息队列中的所有消息
        while (peekmessage(&msg)) {
            if (msg.message == WM_LBUTTONDOWN) {
                // 计算点击位置对应的棋盘索引
                const int index_x = msg.x / CELL_SIZE;
                const int index_y = msg.y / CELL_SIZE;

                // 边界检查和空位检查
                if (index_x >= 0 && index_x < BOARD_SIZE && 
                    index_y >= 0 && index_y < BOARD_SIZE &&
                    chboard_date[index_y][index_x] == '_') {
                    
                    chboard_date[index_y][index_x] = current_piece;
                    SwitchPlayer();
                }
            }
        }

        // 清屏和绘制
        cleardevice();
        DrawBoard();
        DrawPiece();
        DrawTipText();
        FlushBatchDraw();

        // 检查游戏状态
        const GameState state = CheckGameState();
        switch (state) {
        case GameState::X_WINS:
            MessageBox(GetHWnd(), _T("X 玩家获胜!"), _T("游戏结束"), MB_OK);
            running = false;
            break;
        case GameState::O_WINS:
            MessageBox(GetHWnd(), _T("O 玩家获胜!"), _T("游戏结束"), MB_OK);
            running = false;
            break;
        case GameState::DRAW:
            MessageBox(GetHWnd(), _T("平局!"), _T("游戏结束"), MB_OK);
            running = false;
            break;
        case GameState::CONTINUE:
            // Game continues
            break;
        }

        // 帧率控制 - Optimized timing
        const DWORD end_time = GetTickCount();
        const DWORD delta_time = end_time - start_time;
        
        if (delta_time < FRAME_TIME) {
            Sleep(FRAME_TIME - delta_time);
        }
        start_time = GetTickCount();
    }

    EndBatchDraw();
    closegraph();
    return 0;
}