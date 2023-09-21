#include <GL/glut.h>
#include <string.h>
#include <cstring>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define gameName "Fighting Game Mini Project"

#define font_bitmap_small GLUT_BITMAP_8_BY_13
#define font_bitmap_large GLUT_BITMAP_9_BY_15

#define font_small GLUT_BITMAP_HELVETICA_12
#define font_large GLUT_BITMAP_HELVETICA_18

#define font_huge GLUT_BITMAP_TIMES_ROMAN_24
    
int menuWindowID;
int gameWindowID;

// Window dimensions
int windowWidth = 1280;
int windowHeight = 720;

int squareX = 0;
int squareY = 0;
int squareSize = 100;

bool isInGame = false;
bool mainMenuActive = false; // Flag to control the display of "Hello World"

// Player square dimensions
float playerWidth = 0.15f;
float playerHeight = 0.25f;

// Enemy boss dimensions
float enemyWidth = playerWidth * 3.0f;
float enemyHeight = playerHeight;

// Small square dimensions
float smallSquareWidth = 0.03f;
float smallSquareHeight = 0.05f;

// Game over state
bool gameOver = false;

// Large square position
float largeSquareX = 0.0f;

// Player and enemy HP
int playerHP = 100;
int enemyHP = 1000;

// Line dimensions
float lineWidth = windowWidth / 2.0f;
float lineHeight = 0.0f; // Starting position at the top
bool linefall = true;

// Small square structure
struct SmallSquare {
    float x;
    float y;
};

// Collection of active small squares
std::vector<SmallSquare> smallSquares;

void mouse(int button, int state, int x, int y);

void displayText(const std::string& text, float x, float y)
{
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void update(int value) {
    // Update the position of the falling line
    if (linefall == true) {
        lineHeight -= 0.005f; // Adjust the falling speed as needed
    }

    // Check if the line has reached the bottom
    if (lineHeight < -1.0f) {
        // Reset the line position to the top of the window
        lineHeight = 1.0f;
    }

    // Check line box collision
    if ( (largeSquareX <= 0.5 ) && ( largeSquareX >= -0.5 ) && (lineHeight >= -0.9f) && (lineHeight <= -0.9f + playerHeight) ) {
        playerHP -= 1;
    }

    if (!gameOver) {
        // Move all the active small squares upwards
        for (auto& smallSquare : smallSquares) {
            smallSquare.y += 0.01f;

            // Check for collision with the enemy rectangle
            if (smallSquare.y + smallSquareHeight >= 0.9f &&
                smallSquare.x - (smallSquareWidth / 2) <= enemyWidth / 2 &&
                smallSquare.x + (smallSquareWidth / 2) >= -enemyWidth / 2) {
                // Collision detected
                enemyHP -= 50;

                // Remove the small square from the active squares
                smallSquare.y = 2.0f;  // Move it out of the screen to "delete" it
            }
        }
    }

    // Remove any small squares that are out of bounds
    smallSquares.erase(std::remove_if(smallSquares.begin(), smallSquares.end(), [](const SmallSquare& smallSquare) { return smallSquare.y > 1.0f; }), smallSquares.end());

    // Check game over conditions
    if (enemyHP <= 0 || playerHP <= 0) {
        gameOver = true;
        linefall = false;
    }

    glutPostRedisplay();
    // Call the update function again after 10 milliseconds
    glutTimerFunc(16, update, 0);
}

// Function to display the game
void displayGame() {
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Calculate the scaling factors for width and height
    float aspectRatio = (float)windowWidth / windowHeight;
    float scaleX = aspectRatio / (16.0f / 9.0f);
    float scaleY = 1.0f;

    // Draw the falling line
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(-0.5, lineHeight);  // Starting position of the line
    glVertex2f(0.5, lineHeight);   // Ending position of the line
    glEnd();

    // Draw the large rectangle for the enemy boss
    glColor3f(0.4f, 0.1f, 0.8f);
    glRectf(-enemyWidth / 2, 0.9f, enemyWidth / 2, 1.0f);

    // Draw the player square
    glColor3f(1.0f, 0.4f, 0.0f);
    glRectf(largeSquareX - (playerWidth / 2), -0.9f, largeSquareX + (playerWidth / 2), -0.9f + playerHeight);

    // Draw all the active small squares
    glColor3f(0.0f, 0.6f, 1.0f);
    for (const auto& smallSquare : smallSquares) {
        glRectf(smallSquare.x - (smallSquareWidth / 2), smallSquare.y, smallSquare.x + (smallSquareWidth / 2), smallSquare.y + smallSquareHeight);
    }

    // Display Player HP
    glColor3f(1.0f, 1.0f, 1.0f);
    std::string playerHPText = "Player HP: " + std::to_string(playerHP);
    displayText(playerHPText, -0.9f, 0.9f);

    // Display Enemy HP
    std::string enemyHPText = "Enemy HP: " + std::to_string(enemyHP);
    displayText(enemyHPText, 0.6f, 0.9f);

    // Display victory or defeat message if game over
    if (gameOver) {
        glColor3f(1.0f, 1.0f, 1.0f);
        std::string message;
        if (enemyHP <= 0) {
            message = "You are Victorious!";
        }
        else if (playerHP <= 0) {
            message = "You Died!";
        }
        displayText(message, -0.12f, 0.0f);
    }

    glutPostRedisplay();
    // Swap buffers to display the rendered image
    glutSwapBuffers();
}

// Function to display instructions page
void instructions() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color to black
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white

    // Draw the first set of big texts
    const char* line1 = "SPACEBAR - To make the player JUMP";
    int line1Length = strlen(line1);

    const char* line2 = "LEFT ARROW KEY - To make the player go left";
    int line2Length = strlen(line2);

    const char* line3 = "RIGHT ARROW KEY - To make the player go right";
    int line3Length = strlen(line3);

    // Calculate horizontal positions for the first set of texts
    int line1X = (windowWidth - line1Length * 8) / 2;
    int line2X = (windowWidth - line2Length * 8) / 2;
    int line3X = (windowWidth - line3Length * 8) / 2;

    // Calculate vertical positions for the first set of texts
    int line1Y = windowHeight / 2 + 120;
    int line2Y = windowHeight / 2 + 70;
    int line3Y = windowHeight / 2 + 20;

    // Draw the first set of texts
    glRasterPos2f(line1X, line1Y);
    for (int i = 0; i < line1Length; i++) {
        glutBitmapCharacter(font_bitmap_large, line1[i]);
    }

    glRasterPos2f(line2X, line2Y);
    for (int i = 0; i < line2Length; i++) {
        glutBitmapCharacter(font_bitmap_large, line2[i]);
    }

    glRasterPos2f(line3X, line3Y);
    for (int i = 0; i < line3Length; i++) {
        glutBitmapCharacter(font_bitmap_large, line3[i]);
    }

    // Draw the second set of smaller texts
    const char* line4 = "Player has a total of 100 health points (100HP)";
    int line4Length = strlen(line4);

    const char* line5 = "If you get hit by the enemy, you lose 10HP";
    int line5Length = strlen(line5);

    const char* line6 = "If you hit the enemy, it loses 50HP";
    int line6Length = strlen(line6);

    const char* line7 = "The enemy has 1000HP";
    int line7Length = strlen(line7);

    const char* line8 = "Press ENTER key to go back to the main menu";
    int line8Length = strlen(line8);

    // Calculate horizontal positions for the second set of texts
    int line4X = (windowWidth - line4Length * 6) / 2;
    int line5X = (windowWidth - line5Length * 6) / 2;
    int line6X = (windowWidth - line6Length * 6) / 2;
    int line7X = (windowWidth - line7Length * 6) / 2;
    int line8X = (windowWidth - line8Length * 6) / 2;

    // Calculate vertical positions for the second set of texts
    int line4Y = windowHeight / 2 - 20;
    int line5Y = windowHeight / 2 - 70;
    int line6Y = windowHeight / 2 - 120;
    int line7Y = windowHeight / 2 - 170;
    int line8Y = windowHeight / 2 - 250;

    // Draw the second set of texts
    glRasterPos2f(line4X, line4Y);
    for (int i = 0; i < line4Length; i++) {
        glutBitmapCharacter(font_bitmap_small, line4[i]);
    }

    glRasterPos2f(line5X, line5Y);
    for (int i = 0; i < line5Length; i++) {
        glutBitmapCharacter(font_bitmap_small, line5[i]);
    }

    glRasterPos2f(line6X, line6Y);
    for (int i = 0; i < line6Length; i++) {
        glutBitmapCharacter(font_bitmap_small, line6[i]);
    }

    glRasterPos2f(line7X, line7Y);
    for (int i = 0; i < line7Length; i++) {
        glutBitmapCharacter(font_bitmap_small, line7[i]);
    }

    glRasterPos2f(line8X, line8Y);
    for (int i = 0; i < line8Length; i++) {
        glutBitmapCharacter(font_bitmap_small, line8[i]);
    }

    glFlush(); // Ensure all drawing commands are executed
}

// Function to display main menu
void mainMenu() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color to black
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white

    const char* line1 = "START GAME";
    int line1Length = strlen(line1);
    int line1Width = glutBitmapLength(font_huge, (const unsigned char*)line1);

    const char* line2 = "INSTRUCTIONS";
    int line2Length = strlen(line2);
    int line2Width = glutBitmapLength(font_huge, (const unsigned char*)line2);

    const char* line3 = "CREDITS";
    int line3Length = strlen(line3);
    int line3Width = glutBitmapLength(font_huge, (const unsigned char*)line3);

    const char* line4 = "EXIT";
    int line4Length = strlen(line4);
    int line4Width = glutBitmapLength(font_huge, (const unsigned char*)line4);

    // Calculate vertical positions
    int line1Y = windowHeight / 2 + 100;
    int line2Y = windowHeight / 2 + 50;
    int line3Y = windowHeight / 2;
    int line4Y = windowHeight / 2 - 50;

    // Calculate horizontal positions
    int line1X = (windowWidth - line1Width) / 2;
    int line2X = (windowWidth - line2Width) / 2;
    int line3X = (windowWidth - line3Width) / 2;
    int line4X = (windowWidth - line4Width) / 2;

    // Draw the text lines
    glRasterPos2f(line1X, line1Y);
    for (int i = 0; i < line1Length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, line1[i]);
    }

    glRasterPos2f(line2X, line2Y);
    for (int i = 0; i < line2Length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, line2[i]);
    }

    glRasterPos2f(line3X, line3Y);
    for (int i = 0; i < line3Length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, line3[i]);
    }

    glRasterPos2f(line4X, line4Y);
    for (int i = 0; i < line4Length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, line4[i]);
    }

    glFlush(); // Ensure all drawing commands are executed
}

// Function to display project info
void mainInfo() {
    glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

    glColor3f(1.0f, 1.0f, 1.0f); // Set color to white

    const char* line1 = "VIVEKANANDA INSTITUTE OF TECHNOLOGY";
    int line1Length = strlen(line1);
    int line1Width = glutBitmapLength(font_large, (const unsigned char*)line1);

    const char* line2 = "DEPARTMENT OF COMPUTER SCIENCE";
    int line2Length = strlen(line2);
    int line2Width = glutBitmapLength(font_small, (const unsigned char*)line2);

    const char* line3 = "A MINI PROJECT ON";
    int line3Length = strlen(line3);
    int line3Width = glutBitmapLength(font_small, (const unsigned char*)line3);

    const char* line4 = "FIGHTING GAME";
    int line4Length = strlen(line4);
    int line4Width = glutBitmapLength(font_large, (const unsigned char*)line4);

    const char* line5 = "Created by";
    int line5Length = strlen(line5);
    int line5Width = glutBitmapLength(font_small, (const unsigned char*)line5);

    const char* line6 = "STUDENT NAMES";
    int line6Length = strlen(line6);
    int line6Width = glutBitmapLength(font_small, (const unsigned char*)line6);

    const char* line7 = "ADIB UL HUQ KHAN (1VK20CS005)";
    int line7Length = strlen(line7);
    int line7Width = glutBitmapLength(font_small, (const unsigned char*)line7);

    const char* line8 = "RISHABH RANJAN (1VK20CS067)";
    int line8Length = strlen(line8);
    int line8Width = glutBitmapLength(font_small, (const unsigned char*)line8);

    const char* line9 = "Under the Guidance of";
    int line9Length = strlen(line9);
    int line9Width = glutBitmapLength(font_small, (const unsigned char*)line9);

    const char* line10 = "Mrs. Deepthi T.K";
    int line10Length = strlen(line10);
    int line10Width = glutBitmapLength(font_small, (const unsigned char*)line10);

    const char* line11 = "ACADEMIC YEAR 2022-23";
    int line11Length = strlen(line11);
    int line11Width = glutBitmapLength(font_small, (const unsigned char*)line11);

    const char* line12 = "Press ENTER to go to the Main Menu";
    int line12Length = strlen(line12);
    int line12Width = glutBitmapLength(font_small, (const unsigned char*)line12);

    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);

    // Calculate vertical and horizontal offsets for centering
    int line1X = (windowWidth - line1Width) / 2;
    int line1Y = windowHeight - 100;

    int line2X = (windowWidth - line2Width) / 2;
    int line2Y = line1Y - 50;

    int line3X = (windowWidth - line3Width) / 2;
    int line3Y = line2Y - 50;

    int line4X = (windowWidth - line4Width) / 2;
    int line4Y = line3Y - 50;

    int line5X = (windowWidth - line5Width) / 2;
    int line5Y = line4Y - 50;

    int line6X = 100;
    int line6Y = line5Y - 100;

    int line7X = line6X;
    int line7Y = line6Y - 50;

    int line8X = line6X;
    int line8Y = line7Y - 50;

    int line9X = windowWidth - line9Width - 100;
    int line9Y = line5Y - 100;

    int line10X = line9X;
    int line10Y = line9Y - 50;

    int line11X = (windowWidth - line9Width) / 2;
    int line11Y = line10Y - 50;

    int line12X = ((windowWidth - line10Width) / 2 ) - 50;
    int line12Y = line11Y - 50;

    glRasterPos2f(line1X, line1Y); // Set the position for the first line
    for (int i = 0; i < line1Length; i++) {
        glutBitmapCharacter(font_large, line1[i]);
    }

    glRasterPos2f(line2X, line2Y); // Set the position for the second line
    for (int i = 0; i < line2Length; i++) {
        glutBitmapCharacter(font_small, line2[i]);
    }

    glRasterPos2f(line3X, line3Y); // Set the position for the third line
    for (int i = 0; i < line3Length; i++) {
        glutBitmapCharacter(font_small, line3[i]);
    }

    glRasterPos2f(line4X, line4Y); // Set the position for the fourth line
    for (int i = 0; i < line4Length; i++) {
        glutBitmapCharacter(font_large, line4[i]);
    }

    glRasterPos2f(line5X, line5Y); // Set the position for the fifth line
    for (int i = 0; i < line5Length; i++) {
        glutBitmapCharacter(font_small, line5[i]);
    }

    glRasterPos2f(line6X, line6Y); // Set the position for the sixth line
    for (int i = 0; i < line6Length; i++) {
        glutBitmapCharacter(font_small, line6[i]);
    }

    glRasterPos2f(line7X, line7Y); // Set the position for the seventh line
    for (int i = 0; i < line7Length; i++) {
        glutBitmapCharacter(font_small, line7[i]);
    }

    glRasterPos2f(line8X, line8Y); // Set the position for the eighth line
    for (int i = 0; i < line8Length; i++) {
        glutBitmapCharacter(font_small, line8[i]);
    }

    glRasterPos2f(line9X, line9Y); // Set the position for the ninth line
    for (int i = 0; i < line9Length; i++) {
        glutBitmapCharacter(font_small, line9[i]);
    }

    glRasterPos2f(line10X, line10Y); // Set the position for the tenth line
    for (int i = 0; i < line10Length; i++) {
        glutBitmapCharacter(font_small, line10[i]);
    }

    glRasterPos2f(line11X, line11Y); // Set the position for the ninth line
    for (int i = 0; i < line11Length; i++) {
        glutBitmapCharacter(font_small, line11[i]);
    }

    glRasterPos2f(line12X, line12Y); // Set the position for the tenth line
    for (int i = 0; i < line12Length; i++) {
        glutBitmapCharacter(font_small, line12[i]);
    }

    if (mainMenuActive) {
        mainMenu();
    }

    glFlush(); // Ensure all drawing commands are executed
}

// Function to handle the display callback
void display() {
    glClear(GL_COLOR_BUFFER_BIT);  // Clear the color buffer

    // Draw your OpenGL scene here
    mainInfo();

    glFlush();  // Ensure all drawing commands are executed
}

// Function to handle the window reshape callback
void reshape(int width, int height) {
    glViewport(0, 0, width, height);  // Set the viewport

    windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    windowWidth = glutGet(GLUT_WINDOW_WIDTH);

    // Set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
}

// Function to handle keyboard callback
void keyboard(unsigned char key, int x, int y) {
    if (key == 13) { // Check for Enter key press
        printf("ENTER clicked\n");
        mainMenuActive = true; // Set the flag to display "Hello World"
        glutPostRedisplay(); // Trigger a redisplay to show the updated screen
    }
}


void keyboardGame(int key, int x, int y) {
    // printf("Entered keyboardGame function");
    switch (key) {
    case GLUT_KEY_LEFT:
        if (largeSquareX > -1.0f)
            largeSquareX -= 0.1f;
        break;
    case GLUT_KEY_RIGHT:
        if (largeSquareX < 1.0f)
            largeSquareX += 0.1f;
        break;
    case GLUT_KEY_UP:
        // Fire a new small square
        SmallSquare smallSquare;
        smallSquare.x = largeSquareX;
        smallSquare.y = -0.6f;
        smallSquares.push_back(smallSquare);
        break;
    case GLUT_KEY_F10: // F10 key
        exit(0);
        break;
    }
}


// Function to handle mouse callback
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        // Convert mouse coordinates to OpenGL coordinates
        int oglX = x;
        int oglY = windowHeight - y;

        // Check if the mouse click is within the bounds of a button
        if (oglX >= (windowWidth - 200) / 2 && oglX <= (windowWidth + 200) / 2) {
            if (oglY >= windowHeight / 2 + 100 && oglY <= windowHeight / 2 + 120) {
                // START GAME button clicked
                // Perform action here
                printf("START GAME clicked\n");
                if (!isInGame) {
                    isInGame = true;
                    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
                    gameWindowID = glutCreateWindow(gameName);
                    glutDisplayFunc(displayGame);
                    glutSpecialFunc(keyboardGame); // Register keyboard callback
                    printf("keyboard registered");
                    glutDestroyWindow(menuWindowID);
                    glutPostRedisplay();
                    glutTimerFunc(10, update, 0);
                }
            }
            else if (oglY >= windowHeight / 2 + 50 && oglY <= windowHeight / 2 + 70) {
                // INSTRUCTIONS button clicked
                // Perform action here
                printf("INSTRUCTIONS clicked\n");
                instructions();
            }
            else if (oglY >= windowHeight / 2 && oglY <= windowHeight / 2 + 20) {
                // CREDITS button clicked
                // Perform action here
                printf("CREDITS clicked\n");
                mainMenuActive = false;
                mainInfo();
            }
            else if (oglY >= windowHeight / 2 - 50 && oglY <= windowHeight / 2 - 30) {
                // EXIT button clicked
                // Perform action here
                printf("EXIT clicked\n");
                exit(0); // Close the program
            }
        }
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);  // Initialize GLUT
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  // Set display mode

    glutInitWindowSize(1280, 720);  // Set window size
    glutInitWindowPosition(120, 70);
    menuWindowID = glutCreateWindow(gameName);  // Create a window

    windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    windowWidth = glutGet(GLUT_WINDOW_WIDTH);

    glutDisplayFunc(display);  // Register display callback
    glutReshapeFunc(reshape);  // Register reshape callback
    glutKeyboardFunc(keyboard); // Register keyboard callback
    glutMouseFunc(mouse); // Register mouse callback

    glutMainLoop();  // Enter the main loop

    return 0;
}