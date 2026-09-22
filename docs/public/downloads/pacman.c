// 吃豆人小游戏
// 操作说明：
//      W  上移
//      S  下移
//      A  左移
//      D  右移
//      空格  暂停 / 继续
//      ESC  退出
// 游戏规则：
//    吃一个小豆子 .    +10 分
//    吃一个大豆子 o    +50 分，进入"climax"状态，可以吃幽灵
//    吃掉一只幽灵      +200 分
//    吃完全部豆子      胜利
//    被幽灵碰到        失去一条命，共 3 条命

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include <windows.h>

// 游戏参数
#define MAP_W 31  // 地图宽
#define MAP_H 18  // 地图高

#define MAX_GHOSTS 4  // 幽灵数量
#define INIT_LIVES 3  // 初始生命

#define FRIGHT_TICKS 60  // 受惊持续 tick 数
#define GAME_TICK 110  // 每逻辑帧毫秒

#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3
#define DIR_NONE 4

#define PAC_START_R 16
#define PAC_START_C 15

// 方向向量：dR[方向] 对应行变化，dC[方向] 对应列变化
int dR[4] = { -1, 1,  0, 0 };
int dC[4] = {  0, 0, -1, 1 };

// 地图模板
// # : 墙     . : 小豆子     o : 大豆子     空格 : 空地
const char *mazeTemplate[MAP_H] = {
    "###############################",
    "#.............................#",
    "#.###.###.###.###.###.###.###.#",
    "#.###.###.###.###.###.###.###.#",
    "#.............................#",
    "#.###.#.#####.###.#####.#.###.#",
    "#.....#.....#.....#.....#.....#",
    "#.###.#####.#..o..#.#####.###.#",
    "#...........#.....#...........#",
    "#.#######.#.........#.#######.#",
    "#.............................#",
    "#.###.###.###.###.###.###.###.#",
    "#.###.###.###.###.###.###.###.#",
    "#.............................#",
    "#.###.###.###.###.###.###.###.#",
    "#.###.###.###.###.###.###.###.#",
    "#o...........................o#",
    "###############################"
};

// 全局变量
char maze[MAP_H][MAP_W + 1];  // 当前地图

// 吃豆人
int pacR, pacC;  // 当前位置
int pacDir, pacWantDir;  // 当前方向、想要的方向

// 幽灵
int ghostR[MAX_GHOSTS], ghostC[MAX_GHOSTS];
int ghostDir[MAX_GHOSTS];
int ghostHomeR[MAX_GHOSTS] = { 6, 6, 7, 7 };
int ghostHomeC[MAX_GHOSTS] = { 14, 16, 14, 16 };
int ghostColor[MAX_GHOSTS] = { 12, 13, 10, 11 };

// 游戏状态
int score;
int lives;
int dotsLeft;
int gameOverFlag;
int pauseFlag;
int winFlag;
int quitFlag;
int frightTimer;
int tickCount;

// 工具函数

// 隐藏光标
void HideCursor(void) {
    HANDLE handle;
    CONSOLE_CURSOR_INFO cursorInfo;

    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = 0;

    SetConsoleCursorInfo(handle, &cursorInfo);
}

// 移动光标
void gotoxy(int x, int y) {
    HANDLE handle;
    COORD pos;

    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    pos.X = (SHORT)x;
    pos.Y = (SHORT)y;

    SetConsoleCursorPosition(handle, pos);
}

// 设置文字颜色
void setColor(short color) {
    HANDLE handle;
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(handle, color);
}

// 居中显示文字
void printCenter(const char *text, int y) {
    int len = (int)strlen(text);
    int x = (60 - len) / 2;

    if (x < 0) x = 0;

    gotoxy(x, y);
    printf("%s", text);
}

// 判断某格是不是墙
int isWall(int r, int c) {
    if (r < 0 || r >= MAP_H || c < 0 || c >= MAP_W) return 1;
    return maze[r][c] == '#';
}

// 取反方向
int opposite(int d) {
    if (d == DIR_UP)    return DIR_DOWN;
    if (d == DIR_DOWN)  return DIR_UP;
    if (d == DIR_LEFT)  return DIR_RIGHT;
    if (d == DIR_RIGHT) return DIR_LEFT;
    return DIR_NONE;
}

// 开始界面
void startPage(void) {
    int ch;

    system("cls");

    setColor(14);
    printCenter("========== PAC-MAN in C ==========", 2);

    setColor(11);
    printCenter("W A S D / Arrow Keys : Move", 5);
    printCenter("SPACE : Pause / Continue", 6);
    printCenter("ESC   : Quit", 7);

    setColor(10);
    printCenter(".  =  10 points", 9);
    printCenter("o  =  50 points  (ghosts become edible)", 10);
    printCenter("Eat a frightened ghost  =  200 points", 11);

    setColor(12);
    printCenter("Eat all the dots to win!", 13);

    setColor(14);
    printCenter("Press ENTER to start", 16);

    setColor(7);

    while (1) {
        if (kbhit()) {
            ch = getch();
            if (ch == '\r') break;
            if (ch == 27) exit(0);
        }
        Sleep(20);
    }

    system("cls");
}

// 初始化
void initGame(void) {
    int i, j;

    // 复制地图模板
    for (i = 0; i < MAP_H; i++) {
        strcpy(maze[i], mazeTemplate[i]);
    }

    // 清空出生点上的豆子
    maze[PAC_START_R][PAC_START_C] = ' ';
    for (i = 0; i < MAX_GHOSTS; i++) {
        maze[ghostHomeR[i]][ghostHomeC[i]] = ' ';
    }

    // 统计剩余豆子
    dotsLeft = 0;
    for (i = 0; i < MAP_H; i++) {
        for (j = 0; j < MAP_W; j++) {
            if (maze[i][j] == '.' || maze[i][j] == 'o') dotsLeft++;
        }
    }

    // 吃豆人
    pacR = PAC_START_R;
    pacC = PAC_START_C;
    pacDir = DIR_NONE;
    pacWantDir = DIR_NONE;

    // 幽灵
    for (i = 0; i < MAX_GHOSTS; i++) {
        ghostR[i] = ghostHomeR[i];
        ghostC[i] = ghostHomeC[i];
        ghostDir[i] = DIR_DOWN;
    }

    // 状态
    score = 0;
    lives = INIT_LIVES;
    gameOverFlag = 0;
    pauseFlag = 0;
    winFlag = 0;
    frightTimer = 0;
    tickCount = 0;

    system("cls");
}

// 绘制
void drawAll(void) {
    int r, c, i;
    char buf[80];
    int bi;
    int curColor;

    for (r = 0; r < MAP_H; r++) {
        gotoxy(0, r);

        bi = 0;
        curColor = -1;

        for (c = 0; c < MAP_W; c++) {
            char ch;
            int color;
            char mc = maze[r][c];

            // 先按地图内容取默认值
            if (mc == '#') {
                ch = '#';  color = 1;
            } else if (mc == '.') {
                ch = '.';  color = 7;
            } else if (mc == 'o') {
                ch = 'o';  color = 14;
            } else {
                ch = ' ';  color = 0;
            }

            // 幽灵覆盖
            for (i = 0; i < MAX_GHOSTS; i++) {
                if (ghostR[i] == r && ghostC[i] == c) {
                    ch = 'M';
                    color = (frightTimer > 0) ? 9 : ghostColor[i];
                    break;
                }
            }

            // 吃豆人覆盖在最上层
            if (r == pacR && c == pacC) {
                ch = 'O';
                color = 14;
            }

            // 颜色变化时先冲刷缓冲区
            if (color != curColor) {
                if (bi > 0) {
                    buf[bi] = '\0';
                    printf("%s", buf);
                    bi = 0;
                }
                setColor((short)color);
                curColor = color;
            }

            buf[bi++] = ch;
        }

        if (bi > 0) {
            buf[bi] = '\0';
            printf("%s", buf);
        }
    }

    setColor(7);

    // 状态栏
    gotoxy(0, MAP_H);
    printf("Score: %-6d  Lives: %d  Dots: %-4d      ", score, lives, dotsLeft);

    gotoxy(0, MAP_H + 1);
    if (pauseFlag) {
        printf(">> PAUSED - press SPACE to continue <<      ");
    } else {
        printf("WASD/Arrows: Move   SPACE: Pause   ESC: Quit");
    }

    setColor(7);
}

// 吃豆人移动
void movePacman(void) {
    int nr, nc;

    // 尝试转向
    if (pacWantDir != DIR_NONE) {
        nr = pacR + dR[pacWantDir];
        nc = pacC + dC[pacWantDir];

        if (!isWall(nr, nc)) {
            pacDir = pacWantDir;
            pacWantDir = DIR_NONE;
        }
    }

    // 沿当前方向前进
    if (pacDir != DIR_NONE) {
        nr = pacR + dR[pacDir];
        nc = pacC + dC[pacDir];

        if (!isWall(nr, nc)) {
            pacR = nr;
            pacC = nc;
        }
    }

    // 吃东西
    if (maze[pacR][pacC] == '.') {
        maze[pacR][pacC] = ' ';
        score += 10;
        dotsLeft--;
    }
    else if (maze[pacR][pacC] == 'o') {
        int i;

        maze[pacR][pacC] = ' ';
        score += 50;
        dotsLeft--;

        frightTimer = FRIGHT_TICKS;

        // 幽灵立即掉头
        for (i = 0; i < MAX_GHOSTS; i++) {
            int d = opposite(ghostDir[i]);
            if (d != DIR_NONE) ghostDir[i] = d;
        }
    }
}

// 幽灵移动
void moveGhost(int idx) {
    int k, dir, nr, nc;
    int options[4];
    int nopt = 0;
    int bestDir = -1;
    int bestScore = 0x7fffffff;

    // 收集所有可行方向（不包含回头方向）
    for (dir = 0; dir < 4; dir++) {
        if (dir == opposite(ghostDir[idx])) continue;

        nr = ghostR[idx] + dR[dir];
        nc = ghostC[idx] + dC[dir];

        if (isWall(nr, nc)) continue;

        options[nopt++] = dir;
    }

    // 死胡同：掉头
    if (nopt == 0) {
        dir = opposite(ghostDir[idx]);
        if (dir != DIR_NONE) {
            nr = ghostR[idx] + dR[dir];
            nc = ghostC[idx] + dC[dir];

            if (!isWall(nr, nc)) {
                ghostDir[idx] = dir;
                ghostR[idx] = nr;
                ghostC[idx] = nc;
            }
        }
        return;
    }

    if (frightTimer > 0) {
        // 受惊：随机逃窜
        bestDir = options[rand() % nopt];
    }
    else {
        // 追击：选距离吃豆人最近的方向
        for (k = 0; k < nopt; k++) {
            int d = options[k];
            int dist;

            nr = ghostR[idx] + dR[d];
            nc = ghostC[idx] + dC[d];

            dist = abs(nr - pacR) + abs(nc - pacC);
            dist += rand() % 3;

            if (dist < bestScore) {
                bestScore = dist;
                bestDir = d;
            }
        }
    }

    ghostDir[idx] = bestDir;
    ghostR[idx] += dR[bestDir];
    ghostC[idx] += dC[bestDir];
}

// 回合重置
void resetRound(void) {
    int i;

    pacR = PAC_START_R;
    pacC = PAC_START_C;
    pacDir = DIR_NONE;
    pacWantDir = DIR_NONE;

    for (i = 0; i < MAX_GHOSTS; i++) {
        ghostR[i] = ghostHomeR[i];
        ghostC[i] = ghostHomeC[i];
        ghostDir[i] = DIR_DOWN;
    }

    frightTimer = 0;

    drawAll();
    Sleep(500);
}

// 碰撞检测
void checkCollision(void) {
    int i;

    for (i = 0; i < MAX_GHOSTS; i++) {
        if (ghostR[i] == pacR && ghostC[i] == pacC) {

            if (frightTimer > 0) {
                // 吃掉幽灵
                score += 200;
                ghostR[i] = ghostHomeR[i];
                ghostC[i] = ghostHomeC[i];
                ghostDir[i] = DIR_DOWN;
            }
            else {
                // 被幽灵抓住
                lives--;

                if (lives <= 0) {
                    gameOverFlag = 1;
                }
                else {
                    resetRound();
                }
                return;
            }
        }
    }
}

// 逻辑更新
void updateGame(void) {
    int i;

    tickCount++;

    if (frightTimer > 0) frightTimer--;

    movePacman();

    // 幽灵比吃豆人稍慢：每 4 个 tick 少走 1 次
    if (tickCount % 4 != 0) {
        for (i = 0; i < MAX_GHOSTS; i++) {
            moveGhost(i);
        }
    }

    checkCollision();

    if (dotsLeft <= 0) {
        winFlag = 1;
        gameOverFlag = 1;
    }
}

// 键盘处理
void processKey(int ch) {
    switch (ch) {
        case 'w': case 'W': pacWantDir = DIR_UP;    break;
        case 's': case 'S': pacWantDir = DIR_DOWN;  break;
        case 'a': case 'A': pacWantDir = DIR_LEFT;  break;
        case 'd': case 'D': pacWantDir = DIR_RIGHT; break;

        case ' ':
            pauseFlag = !pauseFlag;
            break;

        default:
            break;
    }
}

// 结束界面
void gameOverPage(void) {
    char buf[80];
    int ch;

    system("cls");

    if (winFlag) {
        setColor(10);
        printCenter("========== YOU WIN! ==========", 4);
    }
    else {
        setColor(12);
        printCenter("========== GAME OVER ==========", 4);
    }

    setColor(14);
    sprintf(buf, "Final Score: %d", score);
    printCenter(buf, 6);

    setColor(7);
    printCenter("Press ENTER to play again", 9);
    printCenter("Press ESC to quit", 10);

    while (1) {
        if (kbhit()) {
            ch = getch();
            if (ch == '\r') break;
            if (ch == 27) exit(0);
        }
        Sleep(20);
    }

    system("cls");
}

// 主函数
int main(void) {
    srand((unsigned int)time(NULL));

    HideCursor();
    startPage();

    while (!quitFlag) {
        int acc = 0;

        initGame();
        drawAll();

        while (!gameOverFlag && !quitFlag) {

            // 响应按键
            if (kbhit()) {
                processKey(getch());
            }

            Sleep(10);
            acc += 10;

            // 到达一个逻辑帧
            if (acc >= GAME_TICK) {
                acc = 0;

                if (!pauseFlag) {
                    updateGame();
                }
                drawAll();
            }
        }

        if (quitFlag) break;

        gameOverPage();
    }

    setColor(7);
    system("cls");
    printf("Thanks for playing!\n");

    return 0;
}
