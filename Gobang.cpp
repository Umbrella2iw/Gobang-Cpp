#include "Gobang.h"
#include "Utility.h"
#include "KeyCode.h"
#include <cstdio>
#include <iostream>
#include "conio.h"

Gobang::Gobang() {
    init();
    displayMainMenu();
}

bool Gobang::init() {
    if (isInit) {
        return false;
    }
    board = nullptr;
    isInit = true;
    SetConsoleTitle(TITLE);
    char cmd[50];
    sprintf(cmd, "mode con cols=%d lines=%d", WIDTH, HEIGHT + 1);
    system(cmd);
    handle = GetStdHandle(STD_OUTPUT_HANDLE);
    Utility::SetHandle(handle);
    return true;
}

void Gobang::displayMainMenu() {
    clearConsole();
    drawBorder();
    drawText({25, 5}, "�� ����������Ϸ ��");
    drawText({28, 10}, "1) ��ʼ��Ϸ");
    drawText({28, 15}, "2) �˳���Ϸ");
    drawText({25, 20}, "�������ּ�����ѡ��");
    int clicked = 0;
    while (!clicked) {
        if (kbhit()) {
            switch ((KeyCode)getch()) {
                case KEY_1:
                    clicked = 1;
                    break;
                case KEY_2:
                    clicked = 2;
                    break;
            }
        }
    }
    if (clicked == 1) {
        selectBoardSize();
    }
}

void Gobang::selectBoardSize() {
    clearConsole();
    drawBorder();
    drawText({25, 5}, "�� ����������Ϸ ��");
    drawText({28, 7}, "ѡ�����̴�С");
    drawText({30, 10}, "1) 3 X 3");
    drawText({30, 14}, "2) 4 X 4");
    drawText({15, 20}, "ESC����س����ص����˵��������ּ�����ѡ��");
    int clicked = 0;
    while (!clicked) {
        if (kbhit()) {
            switch ((KeyCode)getch()) {
                case KEY_1:
                    clicked = 1;
                    break;
                case KEY_2:
                    clicked = 2;
                    break;
                case KEY_ESCAPE:
                case KEY_BACKSPACE:
                    clicked = 3;
                    break;
                default:
                    clicked = 4;
            }
        }
    }
    if (clicked == 1 || clicked == 2) {
        boardSize = clicked == 1 ? 3 : 4;
        startGame();
    } else if (clicked == 3) {
        displayMainMenu();
    } else {
        selectBoardSize();
    }
}

void Gobang::startGame() {
    if (board != nullptr) {
        for (int i = 0; i < prevBoardSize; i++) {
            delete[] board[i];
            delete[] vis[i];
        }
        delete[] board;
        delete[] vis;
    }
    board = new int*[boardSize];
    vis = new int*[boardSize];
    for (int i = 0; i < boardSize; i++) {
        board[i] = new int[boardSize];
        vis[i] = new int[boardSize];
    }
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            board[i][j] = 0;
            vis[i][j] = 0;
        }
    }
    while (1) {
        showBoard();
        bool ended = putChess();
        if (ended) {
            showResult();
            break;
        }
    }
}

void Gobang::showBoard() {
    clearConsole();
    drawBorder();
    short xOffset = 0, yOffset = 0;
    if (boardSize == 4) {
        xOffset = -3;
    }
    for (short i = 0; i < boardSize; i++) {
        for (short j = 0; j < boardSize; j++) {
            drawWindow({BOARD_START_X + BLOCK_WIDTH * i + xOffset, BOARD_START_Y + BLOCK_HEIGHT * j}, {7, 4});
            if (board[i][j] > 0) {
                Utility::SetCursorPos(BOARD_START_X + BLOCK_WIDTH / 2 + BLOCK_WIDTH * i + xOffset, BOARD_START_Y + BLOCK_HEIGHT / 2 + BLOCK_HEIGHT * j);
                switch (board[i][j]) {
                    case 1:
                        Utility::SetTextColor(ColorCode::TintBlue);
                        std::cout << 'O';
                        break;
                    case 2:
                        Utility::SetTextColor(ColorCode::TintGreen);
                        std::cout << 'X';
                        break;
                }
                Utility::SetTextColor(ColorCode::White);
            }
        }
    }
    Utility::SetCursorPos(WIDTH / 2 - 20, HEIGHT - 4);
    std::cout << "���������" << 1 + round << "�Ļغϣ���������λ�ã��� �У���";
}

bool Gobang::putChess() {
    int putX, putY;
    std::cin >> putX >> putY;
    if (!checkValid({putX, putY}) || board[putX][putY] > 0) {
        for (int i = 1; i < WIDTH - 1; i++) {
            Utility::SetCursorPos(i, HEIGHT - 4);
            std::cout << ' ';
        }
        Utility::SetCursorPos(WIDTH / 2 - 15, HEIGHT - 4);
        std::cout << "���벻�Ϸ������������룺";
        return putChess();
    }
    board[putX][putY] = 1 + round;
    vis[putX][putY] = 1;
    if (checkWin(round, {putX, putY}, 1)) {
        return true;
    }
    vis[putX][putY] = 0;
    round = !round;
    return false;
}

void Gobang::showResult() {
    prevBoardSize = boardSize;
    clearConsole();
    drawBorder();
    drawText({27, 5}, "�� ���ֽ����� ��");
    Utility::SetCursorPos(28, 8);
    std::cout << "��� " << 1 + round << " ��ʤ��";
    drawText({28, 11}, "1) ����һ��");
    drawText({28, 14}, "2) �������˵�");
    drawText({27, 18}, "�������ּ�����ѡ��");
    int clicked = 0;
    while (!clicked) {
        if (kbhit()) {
            switch ((KeyCode)getch()) {
                case KEY_1:
                    clicked = 1;
                    break;
                case KEY_2:
                    clicked = 2;
                    break;
            }
        } 
    }
    boardSize = 0;
    if (clicked == 1) {
        boardSize = prevBoardSize;
        round = 0;
        startGame();
    }
    if (clicked == 2) {
        displayMainMenu();
    }
}

bool Gobang::checkWin(bool isPlayer1, COORD pos, int total = 1, COORD fromDir) {
    COORD dir[] = {{-1, -1}, {1, 1}, {-1, 1}, {1, -1}, {0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    int nowTotal = total;
    COORD cPos;
    if (fromDir.X != 0 || fromDir.Y != 0) {
        cPos = {pos.X + fromDir.X, pos.Y + fromDir.Y};
        if (canCalc(cPos, isPlayer1)) {
            if (nowTotal + 1 == WIN_NUM) {
                return true;
            }
            vis[pos.X][pos.Y] = 1;
            bool flag = checkWin(isPlayer1, cPos, nowTotal + 1, fromDir);
            vis[pos.X][pos.Y] = 0;
            if (flag) {
                return true;
            }
        }
    } else {
        for (int i = 1; i <= 8; i++) {
            cPos = {pos.X + dir[i - 1].X, pos.Y + dir[i - 1].Y};
            if (canCalc(cPos, isPlayer1)) {
                nowTotal++;
            }
            if ((i & 1) == 0) {
                if (total == WIN_NUM) {
                    return true;
                }
                for (int j = -1; j < 1; j++) {
                    cPos = {pos.X + dir[(i - 1) + j].X, pos.Y + dir[(i - 1) + j].Y};
                    if (canCalc(cPos, isPlayer1)) {
                        vis[pos.X][pos.Y] = 1;
                        bool flag = checkWin(isPlayer1, cPos, nowTotal, {dir[i - 1 + j].X, dir[i - 1 + j].Y});
                        vis[pos.X][pos.Y] = 0;
                        if (flag) {
                            return true;
                        }
                    }
                }
                nowTotal = total;
            }
        }
    }
    return false;
}

bool Gobang::checkValid(COORD pos) {
    return pos.X >= 0 && pos.X < boardSize && pos.Y >= 0 && pos.Y < boardSize;
}

bool Gobang::canCalc(COORD pos, bool round) {
    return checkValid(pos) && board[pos.X][pos.Y] == (1 + round) && !vis[pos.X][pos.Y];
}

void Gobang::clearConsole() {
    system("cls");
}

void Gobang::drawBorder() {
    drawWindow({0, 0}, {WIDTH, HEIGHT - 1});
}

void Gobang::drawWindow(COORD pos, COORD size) {
    for (int i = 1; i < size.Y; i++) {
        Utility::SetCursorPos(pos.X, pos.Y + i);
        std::cout << '*';
        Utility::SetCursorPos(pos.X + size.X - 1, pos.Y + i);
        std::cout << '*';
    }
    for (int i = 0; i < size.X; i++) {
        Utility::SetCursorPos(pos.X + i, pos.Y);
        std::cout << '*';
        Utility::SetCursorPos(pos.X + i, pos.Y + size.Y);
        std::cout << '*';
    }
}

void Gobang::drawText(COORD pos, const char* text, ColorCode color) {
    Utility::SetCursorPos(pos.X, pos.Y);
    Utility::SetTextColor(color);
    std::cout << text;
}
