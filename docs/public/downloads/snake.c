/*
贪吃蛇小游戏

操作说明：
    W       上移
    S       下移
    A       左移
    D       右移
    空格        暂停 / 继续

游戏规则：
    每吃一个食物 +10 分
    随着分数提高：
        1. 蛇移动速度加快
        2. 每得到50分场地中增加障碍物
    撞墙、撞自己、撞障碍物时，游戏结束
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include <windows.h>



// 游戏参数

// 屏幕尺寸
#define WIDTH           60
#define HEIGHT          20

// 蛇的最大长度
#define MAX_SNAKE       100

// 最大的障碍物数量
#define MAX_OBSTACLE    30

// 初始蛇速度
#define INITIAL_SPEED   120

// 最快速度
#define MIN_SPEED       15

// 每多少分增加一次难度
#define LEVEL_SCORE     50

// 每次增加多少障碍物
#define OBSTACLE_ADD    3


// 方向
int nextDir[4][2] = {
    {-1, 0},
    { 1, 0},
    { 0,-1},
    { 0, 1}
};


// 蛇
int snakeX[MAX_SNAKE];
int snakeY[MAX_SNAKE];

int snakeLen;
int direction;


// 食物
int foodX;
int foodY;


// 障碍物
int obstacleX[MAX_OBSTACLE];
int obstacleY[MAX_OBSTACLE];
int obstacleCount;


// 游戏状态
int score;
int gameOverFlag;
int pauseFlag;


// 当前蛇速度
int speed;


// 当前难度等级
int difficultyLevel;


// 工具函数声明
void HideCursor(void);
void gotoxy(int x, int y);
void setColor(short color);
void printCenter(const char *text, int y);
void startPage(void);
void drawMap(void);
void initGame(void);
void generateFood(void);
void generateObstacles(int number);
int isObstacle(int x, int y);
int isSnake(int x, int y);
void printSnake(void);
void printFood(void);
void printObstacles(void);
void manChange(void);
void updateDifficulty(void);
void autoChange(void);
void gameOver(void);


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
    int len;
    int x;

    len = (int)strlen(text);

    x = (WIDTH - len) / 2;

    if (x < 0) x = 0;

    gotoxy(x, y);

    printf("%s", text);
}


// 绘制开始界面
void startPage(void) {
    char ch;

    system("cls");

    // 标题
    setColor(14);
    printCenter("===== SNAKE in C =====", HEIGHT / 2 - 6);


    // 信息
    setColor(11);
    printCenter("Programming Course", HEIGHT / 2 - 4);
    printCenter("Guess who is the Developer?", HEIGHT / 2 - 3);


    // 操作说明
    setColor(7);
    printCenter("W A S D : Move", HEIGHT / 2 - 1);
    printCenter("SPACE : Pause / Continue", HEIGHT / 2);

    setColor(12);
    printCenter("Press ENTER to start", HEIGHT / 2 + 5);

    setColor(7);
    // 等待 Enter
    while (1) {
        if (kbhit()) {
            ch = getch();
            if (ch == '\r') break;
        }
    }

    system("cls");
}


// 绘制地图
void drawMap(void) {
    int i;

    setColor(7);

    // 上边界
    for (i = 0; i < WIDTH; i++) {
        gotoxy(i, 0);
        printf("#");
    }

    // 下边界
    for (i = 0; i < WIDTH; i++) {
        gotoxy(i, HEIGHT - 1);
        printf("#");
    }

    // 左边界
    for (i = 1; i < HEIGHT - 1; i++) {
        gotoxy(0, i);
        printf("#");
    }

    // 右边界
    for (i = 1; i < HEIGHT - 1; i++) {
        gotoxy(WIDTH - 1, i);
        printf("#");
    }
}


// 判断是不是蛇
int isSnake(int x, int y) {
    int i;
    for (i = 0; i < snakeLen; i++) {
        if (snakeX[i] == x && snakeY[i] == y) {
            return 1;
        }
    }
    return 0;
}


// 判断是不是障碍物
int isObstacle(int x, int y) {
    int i;

    for (i = 0; i < obstacleCount; i++) {
        if (obstacleX[i] == x && obstacleY[i] == y) {
            return 1;
        }
    }
    return 0;
}


// 生成食物
void generateFood(void) {
    int repeat;
    int i;

    do
    {
        repeat = 0;


        // 生成位置
        foodX = rand() % (HEIGHT - 2) + 1;
        foodY = rand() % (WIDTH - 2) + 1;

        // 检查蛇
        for (i = 0; i < snakeLen; i++) {
            if (snakeX[i] == foodX && snakeY[i] == foodY) {
                repeat = 1;
                break;
            }
        }

        // 检查障碍物
        if (isObstacle(foodX, foodY)) {
            repeat = 1;
        }
    } while (repeat);
}


// 生成障碍物
void generateObstacles(int number) {
    int x, y;

    int repeat;
    int i;

    for (i = 0; i < number; i++) {
        if (obstacleCount >= MAX_OBSTACLE) return;

        do
        {
            repeat = 0;
            x = rand() % (HEIGHT - 2) + 1;
            y = rand() % (WIDTH - 2) + 1;

            if (isSnake(x, y)) repeat = 1;
            if (x == foodX && y == foodY) repeat = 1;
            if (isObstacle(x, y)) repeat = 1;
            if (abs(x - snakeX[0]) <= 2 && abs(y - snakeY[0]) <= 2) repeat = 1;
        } while (repeat);

        obstacleX[obstacleCount] = x;
        obstacleY[obstacleCount] = y;
        obstacleCount++;
    }
}


// 初始化游戏
void initGame(void) {
    int i;

    snakeLen = 3; // 初始长度
    direction = 3; // 初始方向：右
    score = 0;
    gameOverFlag = 0;
    pauseFlag = 0;
    difficultyLevel = 1;
    speed = INITIAL_SPEED;
    obstacleCount = 0;

    // 初始化蛇
    for (i = 0; i < snakeLen; i++) {
        snakeX[i] = HEIGHT / 2;
        snakeY[i] = WIDTH / 2 - i;
    }

    system("cls");
    drawMap();
    gotoxy(0, HEIGHT);
    printf("Score: 0");
    gotoxy(20, HEIGHT);
    printf("Level: 1");
    generateFood();
}


// 绘制蛇
void printSnake(void) {
    int i;
    setColor(10);

    // 蛇头
    gotoxy(snakeY[0], snakeX[0]);
    printf("O");

    // 蛇身
    for (i = 1; i < snakeLen; i++) {
        gotoxy(snakeY[i], snakeX[i]);
        printf("*");
    }
    setColor(7);
}


// 绘制食物
void printFood(void) {
    setColor(12);
    gotoxy(foodY, foodX);
    printf("$");
    setColor(7);
}


// 绘制障碍物
void printObstacles(void) {
    int i;

    setColor(8);

    for (i = 0; i < obstacleCount; i++) {
        gotoxy(obstacleY[i], obstacleX[i]);
        printf("@");
    }

    setColor(7);
}


// 键盘响应
void manChange(void) {
    char key;
    if (!kbhit()) return;

    key = getch();
    switch (key) {
        case 'w':
        case 'W':
            if (direction != 1) direction = 0;
            break;

        case 's':
        case 'S':
            if (direction != 0) direction = 1;
            break;

        case 'a':
        case 'A':
            if (direction != 3) direction = 2;
            break;

        case 'd':
        case 'D':
            if (direction != 2) direction = 3;
            break;

        case ' ':
            pauseFlag = !pauseFlag;
            if (pauseFlag) {
                setColor(14);
                printCenter("=== PAUSED ===", HEIGHT + 1);
                setColor(7);
            }
            else {
                printCenter("              ", HEIGHT + 1);
            }
            break;

        default:
            break;
    }
}


// 更新难度
void updateDifficulty(void) {
    int newLevel;
    int oldSpeed;

    newLevel = score / LEVEL_SCORE + 1;

    if (newLevel > difficultyLevel) {
        difficultyLevel = newLevel;
        generateObstacles(OBSTACLE_ADD);
        printObstacles();
    }

    oldSpeed = INITIAL_SPEED - (score / 10) * 5;
    if (oldSpeed < MIN_SPEED) oldSpeed = MIN_SPEED;
    speed = oldSpeed;

    gotoxy(0, HEIGHT);
    printf("Score: %-5d", score);

    gotoxy(20, HEIGHT);
    printf("Level: %-3d", difficultyLevel);

    gotoxy(35, HEIGHT);
    printf("Speed: %-3dms", speed);
}


// 蛇移动
void autoChange(void) {
    int newHeadX, newHeadY;
    int oldTailX, oldTailY;
    int eatFood;
    int i;

    if (gameOverFlag) return;
    if (pauseFlag) return;


    // 计算新的蛇头
    newHeadX = snakeX[0] + nextDir[direction][0];
    newHeadY = snakeY[0] + nextDir[direction][1];


    // 撞墙检测
    if (newHeadX <= 0 || newHeadX >= HEIGHT - 1 || newHeadY <= 0 || newHeadY >= WIDTH - 1) {
        gameOverFlag = 1;
        gameOver();
        return;
    }

    // 撞障碍物检测
    if (isObstacle(newHeadX, newHeadY)) {
        gameOverFlag = 1;
        gameOver();
        return;
    }


    // 判断是否吃到食物
    eatFood = 0;
    if (newHeadX == foodX && newHeadY == foodY) eatFood = 1;

    // 撞自己？
    if (eatFood) {
        for (i = 1; i < snakeLen; i++) {
            if (newHeadX == snakeX[i] && newHeadY == snakeY[i]) {
                gameOverFlag = 1;
                gameOver();
                return;
            }
        }
    }
    else {
        for (i = 1; i < snakeLen - 1; i++) {
            if (newHeadX == snakeX[i] && newHeadY == snakeY[i]) {
                gameOverFlag = 1;
                gameOver();
                return;
            }
        }
    }

    // 保存旧蛇尾
    oldTailX = snakeX[snakeLen - 1];
    oldTailY = snakeY[snakeLen - 1];

    // 蛇身体向后移动
    for (i = snakeLen - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // 更新蛇头
    snakeX[0] = newHeadX;
    snakeY[0] = newHeadY;

    // 吃食物
    if (eatFood) {
        if (snakeLen < MAX_SNAKE) {
            snakeX[snakeLen] = oldTailX;
            snakeY[snakeLen] = oldTailY;
            snakeLen++;
        }

        score += 10;
        updateDifficulty();
        generateFood();
        printFood();
    }
    else {
        gotoxy(oldTailY, oldTailX);
        printf(" ");
    }

    // 绘制蛇
    printSnake();
}


// 结束界面
void gameOver(void) {
    char key;
    char scoreText[50];
    char levelText[50];

    system("cls");
    setColor(12);
    printCenter("===== GAME OVER =====", HEIGHT / 2 - 4);

    setColor(14);
    sprintf(scoreText, "Final Score: %d", score);
    printCenter(scoreText, HEIGHT / 2 - 2);

    sprintf(levelText, "Final Level: %d", difficultyLevel);
    printCenter(levelText, HEIGHT / 2);

    setColor(7);
    printCenter("Press ENTER to restart", HEIGHT / 2 + 3);

    while (1) {
        key = getch();
        if (key == '\r') break;
    }

    system("cls");
}


// 主函数
int main(void) {
    srand((unsigned int)time(NULL));
    HideCursor();
    startPage();

    while (1) {
        initGame();
        printSnake();
        printFood();

        while (!gameOverFlag) {
            manChange();
            autoChange();
            Sleep(speed);
        }
    }

    return 0;
}
