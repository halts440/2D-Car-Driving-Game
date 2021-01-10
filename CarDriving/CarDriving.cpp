#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <string>
#include <sstream>
using namespace std;

// RGB Colors Array
float colors[][3] = {
    0.87f, 0.86f, 0.84f,    // gray
    1.0f, 1.0f, 1.0f,       // white
    0.9f, 0.0f, 0.0f,       // red
    0.63f, 0.63f, 0.26f,    // yellowish
    0.2f, 0.6f, 1.0f,       // blue
    0.03f, 0.80f, 0.79f,    // bluish green
    1.0f, 0.49f, 0.0f,      // orange
    0.6f, 0.31f, 0.49f,     // 
    0.0f, 0.0f, 0.0f,       // black
    0.0f, 0.5f, 0.4f,       // dark green
};

// variables for board, shape and score
int board[24][25] = { {0} };
int car[2] = { -1 };
int carColor = 1;
int score = 0;
int oldScore = 0;
string scoreStr = "Score: 0";
int addNew = 0;
int crash = 0;

// All helper functions
void drawSquare(float x, float y, float size, float rgb[3]);
void drawBoxes();
void makeRoadside();
void moveRoadSides();
void setCar(int i, int j, int c);
void addCar();
void moveRoadAndCars();
int canMove(int x, int y);
void moveCarLeft();
void moveCarRight();
void removeCar();
void finishCurrGame();
void displayText(int x, int y, const string& str);
void displayScore();
void displayGameOver();


// main display function
void display() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set background color to black
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer

    if (addNew == 0) {
        addNew = 10;
        addCar();
        score++;
    }
    addNew--;
    moveRoadSides();
    removeCar();
    moveRoadAndCars();
    setCar(car[0], car[1], carColor);
    drawBoxes();
    displayScore();

    glFlush();  // Render now
}

// function to handle arrow keys
void NonPrintableKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)
        moveCarLeft();
    else if (key == GLUT_KEY_RIGHT)
        moveCarRight();
    glutPostRedisplay();
}

// function to execute after every specified time
void Timer(int m) {
    glutPostRedisplay();
    // once again we tell the library to call our Timer function after next 800
    glutTimerFunc(200, Timer, 0);
}

// main function
int main(int argc, char** argv) {
    srand(time(0));
    int width = 480, height = 480;
    makeRoadside();
    carColor = rand() % 6 + 2;
    setCar(6, 1, carColor);
    car[0] = 6;
    car[1] = 1;

    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(width, height);   // Set the window's initial width & height
    glutInitWindowPosition(400, 180); // Position the window's initial top-left corner
    glutCreateWindow("Car Driving"); // Create a window with the given title

    // set viewpoint
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutDisplayFunc(display); // Register display callback handler for window re-paint
    glutSpecialFunc(NonPrintableKeys); // telling library which function to call for non-printable ASCII characters
    glutTimerFunc(5 / 5, Timer, 0);

    glutMainLoop();           // Enter the infinitely event-processing loop
    return 0;
}

// Draw Square: makes a sqaure at specified position
void drawSquare(float x, float y, float size, float rgb[3]) {
    glBegin(GL_QUADS);              // 4 vertices will form a quad
    glColor3f(rgb[0], rgb[1], rgb[2]);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glVertex2f(x, y);
    glVertex2f(x + size, y);
    glEnd();
}

// Draw Boxes: draws boxes according to values in board
void drawBoxes() {
    for (int a = 0; a < 24; a++) {
        for (int b = 0; b < 20; b++) {
            if( board[a][b] > 0 )
                drawSquare(a * 24, b * 24, 23, colors[board[a][b]]);
            else 
                drawSquare(a * 24, b * 24, 24, colors[board[a][b]]);
        }
    }
}

// Make Road Side: set values for roadside
void makeRoadside() {
    int path = 3;
    int space = 1;
    for (int i = 0; i < 25; i++) {
        if (path > 0) {
            path--;
            board[4][i] = board[5][i] = board[14][i] = board[15][i] = 9;
        }
        else {
            space--;
            if (space == 0) {
                path = 3;
                space = 1;
            }
        }
    }
}

// Move Road Sides: move roadside one step down
void moveRoadSides() {
    int temp[4] = { board[4][0], board[5][0], board[14][0], board[15][0] };
    for (int a = 0; a < 24; a++) {
        board[4][a] = board[4][a + 1];
        board[5][a] = board[5][a + 1];
        board[14][a] = board[14][a + 1];
        board[15][a] = board[15][a + 1];
    }
    // copy the values of roadside in 0th row to last row
    board[4][24] = temp[0];
    board[5][24] = temp[1];
    board[14][24] = temp[2];
    board[15][24] = temp[3];
}

// Set Car: put values for car in board starting at position i,j
void setCar(int i, int j, int c) {
    board[i][j] = 8;    board[i + 1][j] = board[i + 2][j] = c;    board[i + 3][j] = 8;
    board[i + 1][j + 1] = board[i + 2][j + 1] = c;
    board[i + 1][j + 2] = board[i + 2][j + 2] = c;
    board[i][j + 3] = 8;  board[i + 1][j + 3] = board[i + 2][j + 3] = c;    board[i + 3][j + 3] = 8;
}

// Add Car: place a new car on road
void addCar() {
    int x = rand() % 2;
    x == 0 ? x = 6 : x = 10;
    setCar(x, 20, rand() % 6 + 2);
}

// Move Road And Cars: move all other cars except our own car
void moveRoadAndCars() {
    // check if we hit another car
    for (int a = 0; a < 4; a++) {
        if (board[car[0] + a][car[1] + 4] > 1)
            crash = 1;
    }

    // if we did not hit any other car then move cars
    if (crash == 0) {
        for (int a = 0; a < 24; a++) {
            for (int b = 6; b < 14; b++)
            {
                board[b][a] = board[b][a + 1];
            }
        }
    }

    // if we hit another car then finish game
    else {
        finishCurrGame();
    }
}

// Can Move: check if car can be moved at position starting at x,y or not
int canMove(int x, int y) {
    int canMove_ = 1;
    for (int a = 0; a < 4; a++) {
        if (board[x + a][y] > 0)
            canMove_ = 0;
    }
    for (int a = 1; a < 3; a++) {
        if (board[x + a][y + 1] > 0)
            canMove_ = 0;
    }
    for (int a = 1; a < 3; a++) {
        if (board[x + a][y + 2] > 0)
            canMove_ = 0;
    }
    for (int a = 0; a < 4; a++) {
        if (board[x + a][y + 3] > 0)
            canMove_ = 0;
    }
    return canMove_;
}

// Move Shape Left: move car to left if space is available
void moveCarLeft() {
    // check if car can be moved to left or not
    int x = car[0] - 4;
    int y = car[1];
    if ( x > 3 ) {
        // if it can be moved, move it
        if ( canMove(x, y) ) {
            removeCar();
            car[0] = x;
            car[1] = y;
            setCar(x, y, 2);
        }
        // if it hit another car, finish the game
        else {
            finishCurrGame();
        }
    }
}

// Move Car Right: move car to the right
void moveCarRight() {
    // check if car can be moved to right or not
    int x = car[0] + 4;
    int y = car[1];
    if (x < 16 ) {
        // if car can be moved, then move it
        if (canMove(x, y)) {
            removeCar();
            car[0] = x;
            car[1] = y;
            setCar(x, y, 2);
        }
        // if car hit another car then finish the game
        else {
            finishCurrGame();
        }
    }
}

// Remove Car: remove our car from board
void removeCar() {
    int x = car[0];
    int y = car[1];
    for (int a = 0; a < 4; a++)
        board[x + a][y] = 0;
    for (int a = 1; a < 3; a++)
        board[x + a][y + 1] = 0;
    for (int a = 1; a < 3; a++)
        board[x + a][y + 2] = 0;
    for (int a = 0; a < 4; a++)
        board[x + a][y + 3] = 0;
}

// Finish Current Game: finish the current game and start the new one
void finishCurrGame() {
    // clear all cars from road
    for (int a = 0; a < 25; a++) {
        for (int b = 6; b < 14; b++)
            board[b][a] = 0;
    }
    carColor = rand() % 6 + 2;
    setCar(6, 1, carColor);
    car[0] = 6;
    car[1] = 1;
    addNew = 0;
    crash = 0;
    displayGameOver();
    displayScore();
    glFlush();
    score = 0;
    Sleep(2000);
}

// Display Text: display the text on specified position
void displayText(int x, int y, const string& str)
{
    GLvoid* font_style = GLUT_BITMAP_TIMES_ROMAN_24;
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos3f(x, y, 1);
    //  Draw the characters one by one
    for (int a = 0; a < str.size(); a++)
        glutBitmapCharacter(font_style, str[a]);
}

// Display Score: displays current score on screen
void displayScore() {
    // if score is updated then update the score string
    if (score != oldScore) {
        oldScore = score;
        stringstream s;
        s << score;
        string tmpStr = "";
        s >> tmpStr;
        scoreStr = "Score: " + tmpStr;
    }
    displayText(10, 450, scoreStr);
}

// Display Game Over: show game over text on screen
void displayGameOver() {
    displayText(200, 250, "Game Over");
}