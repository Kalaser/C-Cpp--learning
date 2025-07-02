#include <graphics.h>
#include <conio.h>
#include <ctime>
#include <vector>
#include <algorithm>

constexpr int ROWS = 40;  // 行数
constexpr int COLS = 60;  // 列数
constexpr int CELL_SIZE = 10;  // 细胞大小
constexpr int DELAY_TIME = 50;  // 更快的帧间隔

// 使用vector替代原生数组以获得更好的内存管理
class GameOfLife {
private:
    std::vector<std::vector<bool>> grid;
    std::vector<std::vector<bool>> newGrid;
    std::vector<std::vector<bool>> dirtyRegions; // 脏区域追踪
    
    // 邻居计算的偏移量 - 预计算以提高性能
    static constexpr int NEIGHBOR_OFFSETS[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},          { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1}
    };

public:
    GameOfLife() : grid(ROWS, std::vector<bool>(COLS, false)),
                   newGrid(ROWS, std::vector<bool>(COLS, false)),
                   dirtyRegions(ROWS, std::vector<bool>(COLS, true)) {}

    // 随机初始化细胞状态 - 优化版本
    void randomizeGrid() {
        std::srand(static_cast<unsigned int>(std::time(nullptr)));
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                grid[i][j] = (std::rand() % 2) == 1;
                dirtyRegions[i][j] = true; // 标记为需要重绘
            }
        }
    }

    // 优化的绘制函数 - 仅绘制脏区域
    void drawGrid() {
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                if (dirtyRegions[i][j]) {
                    const int x = j * CELL_SIZE;
                    const int y = i * CELL_SIZE;
                    
                    if (grid[i][j]) {
                        setfillcolor(YELLOW);
                    } else {
                        setfillcolor(BLACK);
                    }
                    
                    fillrectangle(x, y, x + CELL_SIZE, y + CELL_SIZE);
                    dirtyRegions[i][j] = false; // 重置脏标记
                }
            }
        }
    }

    // 优化的邻居计算 - 使用内联函数和边界检查优化
    inline int countLiveNeighbors(int row, int col) const noexcept {
        int count = 0;
        
        // 使用预计算的偏移量和边界检查优化
        for (int k = 0; k < 8; ++k) {
            const int newRow = row + NEIGHBOR_OFFSETS[k][0];
            const int newCol = col + NEIGHBOR_OFFSETS[k][1];
            
            // 边界检查 - 使用环绕模式
            if (newRow >= 0 && newRow < ROWS && newCol >= 0 && newCol < COLS) {
                if (grid[newRow][newCol]) {
                    ++count;
                }
            }
        }
        
        return count;
    }

    // 高度优化的下一代计算
    void computeNextGeneration() {
        bool anyChange = false;
        
        // 重置脏区域
        for (auto& row : dirtyRegions) {
            std::fill(row.begin(), row.end(), false);
        }
        
        // 计算下一代状态
        for (int i = 0; i < ROWS; ++i) {
            for (int j = 0; j < COLS; ++j) {
                const int liveNeighbors = countLiveNeighbors(i, j);
                const bool currentState = grid[i][j];
                bool newState = currentState;
                
                if (currentState) {
                    // 细胞存活情况：2或3个邻居时存活
                    newState = (liveNeighbors == 2 || liveNeighbors == 3);
                } else {
                    // 细胞繁衍情况：恰好3个邻居时繁衍
                    newState = (liveNeighbors == 3);
                }
                
                newGrid[i][j] = newState;
                
                // 检查是否有变化
                if (newState != currentState) {
                    anyChange = true;
                    // 标记变化的细胞及其邻居为脏区域
                    markDirtyRegion(i, j);
                }
            }
        }
        
        // 仅在有变化时交换网格
        if (anyChange) {
            grid.swap(newGrid);
        }
    }

private:
    // 标记脏区域 - 包括邻居细胞
    void markDirtyRegion(int row, int col) {
        for (int i = std::max(0, row - 1); i <= std::min(ROWS - 1, row + 1); ++i) {
            for (int j = std::max(0, col - 1); j <= std::min(COLS - 1, col + 1); ++j) {
                dirtyRegions[i][j] = true;
            }
        }
    }

public:
    void clearGrid() {
        for (auto& row : grid) {
            std::fill(row.begin(), row.end(), false);
        }
        for (auto& row : dirtyRegions) {
            std::fill(row.begin(), row.end(), true);
        }
    }

    void setInitialPattern() {
        clearGrid();

        // 示例：设置一个滑翔机初始模式
        if (ROWS > 5 && COLS > 5) {
            grid[1][2] = true;
            grid[2][3] = true;
            grid[3][1] = true;
            grid[3][2] = true;
            grid[3][3] = true;
            
            // 标记初始模式区域为脏
            for (int i = 0; i < 5; ++i) {
                for (int j = 0; j < 5; ++j) {
                    dirtyRegions[i][j] = true;
                }
            }
        }
    }
    
    // 性能统计
    void printStats() const {
        int liveCount = 0;
        for (const auto& row : grid) {
            liveCount += static_cast<int>(std::count(row.begin(), row.end(), true));
        }
        
        // 在控制台输出统计信息（如果需要）
        // printf("Live cells: %d\n", liveCount);
    }
};

int main() {
    initgraph(COLS * CELL_SIZE, ROWS * CELL_SIZE);
    setbkcolor(BLACK);
    cleardevice();
    
    GameOfLife game;
    game.setInitialPattern();
    game.randomizeGrid();
    
    // 性能计时
    DWORD lastTime = GetTickCount();
    int frameCount = 0;
    
    while (!kbhit()) {
        const DWORD currentTime = GetTickCount();
        
        game.drawGrid();
        game.computeNextGeneration();
        
        // 帧率控制
        const DWORD frameTime = GetTickCount() - currentTime;
        if (frameTime < DELAY_TIME) {
            Sleep(DELAY_TIME - frameTime);
        }
        
        // FPS 计算（每秒更新一次）
        ++frameCount;
        if (currentTime - lastTime >= 1000) {
            // 可以在标题栏显示FPS
            // printf("FPS: %d\n", frameCount);
            frameCount = 0;
            lastTime = currentTime;
        }
    }

    closegraph();
    return 0;
}
