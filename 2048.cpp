#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <curses.h>
#include <iostream>
using namespace std;

//编译命令 Linux/Mac: g++ 2048.cpp -l ncurses -o 2048
//格子数
#define N 4
//格子的字符长度
#define WIDTH 5

#define TARGET 2048

//游戏状态
#define S_FAIL 0
#define S_WIN 1
#define S_NORMAL 2
#define S_QUIT 3

class Game2048 {
public:
    Game2048() : status(S_NORMAL) 
    {
        randNew();
        randNew();
    }

    int getStatus() {return status;}

    void processInput()
    {
        char ch = getch();
        //转换大写
        ch = toupper(ch);

        if (status == S_NORMAL)
        {
            bool updated = false;
            if (ch == 'A') 
            {
                updated = moveLeft();
            }
            else if (ch == 'S')
            {
                rotate();
                draw();
                updated = moveLeft();
                rotate();
                rotate();
                rotate();
            }
            else if (ch == 'D')
            {
                rotate();
                rotate();
                updated = moveLeft();
                rotate();
                rotate();
            }
            else if (ch == 'W')
            {
                rotate();
                rotate();
                rotate();
                updated = moveLeft();
                rotate();
            }

            if (updated) 
            {
                randNew();
                if (isOver()) status = S_FAIL;
            }
        }

        if (ch == 'Q')
        {
            status = S_QUIT;
        }
        else if (ch == 'R')
        {
            restart();
        }
    }

    void draw()
    {
        clear();
        const int offset = 12;
        for (int i = 0; i <= N; ++i)
        {
            for (int j = 0; j <= N; ++j)
            {
                //相交点
                drawItem(i * 2, 1 + j * WIDTH + offset, '+');

                //竖线
                if (i != N)
                {
                    drawItem(i * 2 + 1, 1 + j * WIDTH + offset, '|');
                }

                //横线
                for (int k = 1; j != N && k <WIDTH; ++k)
                {
                    drawItem(i * 2, 1 + j * WIDTH + k + offset, '-');
                }

                //数字
                if (i != N && j != N)
                {
                    drawNum(i * 2 + 1, (j + 1) * WIDTH + offset, data[i][j]);
                }
            }
        }

        mvprintw(2 * N + 2, (5 * (N - 4) - 1) / 2, "W(UP),S(DOWN),A(LEFT),D(RIGHT),R(RESTART),Q(QUIT)");

        if (status == S_WIN) {
            mvprintw( N, 5 * N / 2 - 1, " YOU WIN,PRESS R TO CONTINUE ");
        } else if (status == S_FAIL) {
            mvprintw( N, 5 * N / 2 - 1, " YOU LOSE,PRESS R TO CONTINUE ");
        }

    }

    // void setTestData()
    // {
    //     for (int i = 0; i < N; ++i) {
    //         for (int j = 0; j < N; ++j) {
    //             data[i][j] = 16 << (i + j);
    //             //data[i][j] = 2 << (1 + rand() % 7);
    //         }
    //     }
    // }
    
private:
    bool isOver()
    {
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if ((j + 1 < N) && (data[i][j] * data[i][j+1] == 0 || data[i][j] == data[i][j+1])) return false;
                if ((i + 1 < N) && (data[i][j] * data[i+1][j] == 0 || data[i][j] == data[i+1][j])) return false;
            }
        }
        return true;
    }
 
    void restart()
    {
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                data[i][j] = 0;
            }
        }

        randNew();
        randNew();
        status = S_NORMAL;
        //setTestData();
    }

    bool moveLeft()
    {
        int temp[N][N];
        for (int i = 0; i < N; ++i)
        {
            int preVal = 0;
            int curPos = 0;
            int index = 1;
            for (int j = 0; j < N; ++j)
            {
                temp[i][j] = data[i][j];
                if (data[i][j] == 0)
                {
                    continue;
                }

                if (preVal == 0)
                {
                    preVal = data[i][j];
                }

                else if (preVal == data[i][j])
                {
                    data[i][curPos] = preVal * 2;
                    preVal = 0;
                    if (data[i][curPos] == TARGET) status = S_WIN;
                    ++curPos;
                }

                else if (preVal != data[i][j])
                {
                    data[i][curPos] = preVal;
                    preVal = data[i][j];
                    ++curPos;
                }
                data[i][j] = 0;
            }
            if (preVal) data[i][curPos] = preVal;
        }

        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                if (data[i][j] != temp[i][j]) return true;
            }
        }
        return false;
    }

    //顺时针旋转90度
    void rotate()
    {
        int temp[N][N] = {0};
        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                temp[i][j] = data[N - 1 - j][i];
            }
        }

        for (int i = 0; i < N; ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                data[i][j] = temp[i][j];
            }
        }
        
    }

    //随机产生俩数字
    bool randNew()
    {
        vector<int> emptyPos;
        // 把空位置先存起来
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (data[i][j] == 0) {
                    emptyPos.push_back(i * N + j);
                }
            }
        }
        if (emptyPos.size() == 0) {
            return false;
        }
        // 随机找个空位置
        int value = emptyPos[rand() % emptyPos.size()];
        // 10%的概率产生4
        data[value / N][value % N] = rand() % 10 == 1 ? 4 : 2;
        return true;
    }
    

    void drawItem(int row, int col, char c)
    {
        move(row, col);
        addch(c);
    }

    //数字右对齐
    void drawNum(int row, int col, int num)
    {
        while (num > 0)
        {
            drawItem(row, col, num % 10 + '0');
            num /= 10;
            --col;
        }
    }

private:
    int data[N][N] = {0};
    int status;


};

void initialize()
{
    //初始化
    initscr();
    //不需要输入回车直接交互
    cbreak();
    //按键不显示
    noecho();
    //隐藏光标
    curs_set(0);
    //随机数
    srand(time(NULL));
}

void shutdown()
{ 
    //ncurses清理
    endwin();
}

int main(){

    initialize();

    Game2048 game;
    do {
        game.draw();
        game.processInput();
    } while (S_QUIT != game.getStatus());

    shutdown();


    return 0;
}