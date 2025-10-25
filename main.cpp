#ifdef __APPLE__
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

// ==================== CONSTANTS ====================
#define M_PI 3.14159265358979323846
#define BOARD_WIDTH 28
#define BOARD_HEIGHT 31
#define INITIAL_LIVES 3
#define TOTAL_PEBBLES 244
#define NORMAL_PEBBLE_POINTS 10
#define SUPER_PEBBLE_POINTS 50
#define GHOST_EAT_POINTS 200
#define NUM_GHOSTS 4

// ==================== GAME STATES ====================
enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_HELP,
    STATE_GAME_OVER,
    STATE_WIN
};

// ==================== GLOBAL VARIABLES ====================
GameState currentState = STATE_MENU;
int menuSelection = 0;
const int MENU_OPTIONS = 4; // Start, Help, High Score, Exit

// Game Statistics
int lives = INITIAL_LIVES;
int score = 0;
int highScore = 0;
int pebbles_left = TOTAL_PEBBLES;
time_t gameStartTime;
time_t pausedTime = 0;
time_t totalPausedDuration = 0;
int gameTimeSeconds = 0;

// Player Variables
double pacmanX = 13.5;
double pacmanY = 23.0;
double pacmanSpeed = 0.05;
double pacmanAngle = 90.0;
bool isAnimating = false;
unsigned char currentKey = 0;

// Ghost Variables
int startCountdown = 3;
bool gameOver = false;
int level = 1;

// ==================== BOARD ARRAYS ====================
int board_array[BOARD_HEIGHT][BOARD_WIDTH] = {
    {8,5,5,5,5,5,5,5,5,5,5,5,5,1,1,5,5,5,5,5,5,5,5,5,5,5,5,7},
    {6,0,0,0,0,0,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,0,0,0,0,0,0,6},
    {6,0,8,1,1,7,0,8,1,1,1,7,0,2,4,0,8,1,1,1,7,0,8,1,1,7,0,6},
    {6,0,2,11,11,4,0,2,11,11,11,4,0,2,4,0,2,11,11,11,4,0,2,11,11,4,0,6},
    {6,0,9,3,3,10,0,9,3,3,3,10,0,9,10,0,9,3,3,3,10,0,9,3,3,10,0,6},
    {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
    {6,0,8,1,1,7,0,8,7,0,8,1,1,1,1,1,1,7,0,8,7,0,8,1,1,7,0,6},
    {6,0,9,3,3,10,0,2,4,0,9,3,3,11,11,3,3,10,0,2,4,0,9,3,3,10,0,6},
    {6,0,0,0,0,0,0,2,4,0,0,0,0,2,4,0,0,0,0,2,4,0,0,0,0,0,0,6},
    {9,5,5,5,5,7,0,2,11,1,1,7,0,2,4,0,8,1,1,11,4,0,8,5,5,5,5,10},
    {0,0,0,0,0,6,0,2,11,3,3,10,0,9,10,0,9,3,3,11,4,0,6,0,0,0,0,0},
    {0,0,0,0,0,6,0,2,4,0,0,0,0,0,0,0,0,0,0,2,4,0,6,0,0,0,0,0},
    {0,0,0,0,0,6,0,2,4,0,8,5,5,1,1,5,5,7,0,2,4,0,6,0,0,0,0,0},
    {5,5,5,5,5,10,0,9,10,0,6,0,0,0,0,0,0,6,0,9,10,0,9,5,5,5,5,5},
    {0,0,0,0,0,0,0,0,0,0,6,0,0,0,0,0,0,6,0,0,0,0,0,0,0,0,0,0},
    {5,5,5,5,5,7,0,8,7,0,6,0,0,0,0,0,0,6,0,8,7,0,8,5,5,5,5,5},
    {0,0,0,0,0,6,0,2,4,0,9,5,5,5,5,5,5,10,0,2,4,0,6,0,0,0,0,0},
    {0,0,0,0,0,6,0,2,4,0,0,0,0,0,0,0,0,0,0,2,4,0,6,0,0,0,0,0},
    {0,0,0,0,0,6,0,2,4,0,8,1,1,1,1,1,1,7,0,2,4,0,6,0,0,0,0,0},
    {8,5,5,5,5,10,0,9,10,0,9,3,3,11,11,3,3,10,0,9,10,0,9,5,5,5,5,7},
    {6,0,0,0,0,0,0,0,0,0,0,0,0,2,4,0,0,0,0,0,0,0,0,0,0,0,0,6},
    {6,0,8,1,1,7,0,8,1,1,1,7,0,2,4,0,8,1,1,1,7,0,8,1,1,7,0,6},
    {6,0,9,3,11,4,0,9,3,3,3,10,0,9,10,0,9,3,3,3,10,0,2,11,3,10,0,6},
    {6,0,0,0,2,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,4,0,0,0,6},
    {2,1,7,0,2,4,0,8,7,0,8,1,1,1,1,1,1,7,0,8,7,0,2,4,0,8,1,4},
    {2,3,10,0,9,10,0,2,4,0,9,3,3,11,11,3,3,10,0,2,4,0,9,10,0,9,3,4},
    {6,0,0,0,0,0,0,2,4,0,0,0,0,2,4,0,0,0,0,2,4,0,0,0,0,0,0,6},
    {6,0,8,1,1,1,1,11,11,1,1,7,0,2,4,0,8,1,1,11,11,1,1,1,1,7,0,6},
    {6,0,9,3,3,3,3,3,3,3,3,10,0,9,10,0,9,3,3,3,3,3,3,3,3,10,0,6},
    {6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6},
    {9,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,10}
};

int pebble_array[BOARD_HEIGHT][BOARD_WIDTH] = {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,3,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,3,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,1,0,0,0,0,1,0},
    {0,3,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,1,3,0},
    {0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0},
    {0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0},
    {0,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};

int temp_pebble_array[BOARD_HEIGHT][BOARD_WIDTH];
GLuint displayLists[5];

// ==================== FORWARD DECLARATIONS ====================
bool isCellOpen(int x, int y);
void resetPebbles();
void resetPacman();
void resetGhosts();
void updateGameTime();
void drawText(float x, float y, void* font, const char* text, float r, float g, float b);

// ==================== GHOST CLASS ====================
class Ghost {
private:
    static const int EDIBLE_DURATION = 300;
    static const int INITIAL_JAIL_TIME = 30;

public:
    double x, y;
    double angle;
    double speed;
    double maxSpeed;
    float color[3];
    bool edible;
    int edibleTimer;
    bool eaten;
    bool inJail;
    int jailTimer;
    bool transporting;

    Ghost(double startX, double startY) {
        x = startX;
        y = startY;
        angle = 90.0;
        maxSpeed = 0.03;
        speed = maxSpeed;
        edible = false;
        eaten = false;
        inJail = true;
        jailTimer = INITIAL_JAIL_TIME;
        transporting = false;
        color[0] = 1.0f;
        color[1] = 0.0f;
        color[2] = 0.0f;
    }

    void move() {
        x += speed * cos(M_PI / 180.0 * angle);
        y += speed * sin(M_PI / 180.0 * angle);
    }

    void update() {
        // Tunnel transport logic
        if ((int)x == 0 && (int)y == 14 && !transporting) {
            angle = 180.0;
        }
        if (x < 0.1 && (int)y == 14) {
            x = 26.9;
            transporting = true;
        }
        if ((int)x == 27 && (int)y == 14 && !transporting) {
            angle = 0.0;
        }
        if (x > 26.9 && (int)y == 14) {
            x = 0.1;
            transporting = true;
        }
        if ((int)x == 2 || (int)x == 25) {
            transporting = false;
        }

        // Edibility management
        if (edible && edibleTimer > 0) {
            edibleTimer--;
            if (edibleTimer == 0) {
                edible = false;
                speed = maxSpeed;
            }
        }

        // Jail management
        if (inJail && (int)(y + 0.9) == 11) {
            inJail = false;
            angle = 180.0;
        }

        if (inJail && ((int)x == 13 || (int)x == 14)) {
            angle = 270.0;
        }

        if (jailTimer == 0 && inJail) {
            if (x < 13) angle = 0.0;
            if (x > 14) angle = 180.0;
        }

        if (jailTimer > 0) {
            jailTimer--;
        }

        // Return eaten ghost to jail
        if (eaten && ((int)x == 13 || (int)(x + 0.9) == 14) &&
            ((int)y > 10 && (int)y < 15)) {
            inJail = true;
            angle = 90.0;
            if ((int)y == 14) {
                eaten = false;
                speed = maxSpeed;
                jailTimer = 66;
                x = 11;
            }
        }
    }

    bool catchPacman(double px, double py) {
        return (px - x < 0.2 && px - x > -0.2 &&
                py - y < 0.2 && py - y > -0.2);
    }

    void makeVulnerable() {
        if (!edible && !eaten) {
            angle = fmod((angle + 180.0), 360.0);
            speed = maxSpeed * 0.5;
        }
        edible = true;
        edibleTimer = EDIBLE_DURATION;
    }

    void chase(double targetX, double targetY, bool* openDirections) {
        int multiplier = edible ? -1 : 1;

        if ((int)angle == 0 || (int)angle == 180) {
            if ((int)(multiplier * targetY) > (int)(multiplier * y) && openDirections[1])
                angle = 90.0;
            else if ((int)(multiplier * targetY) < (int)(multiplier * y) && openDirections[3])
                angle = 270.0;
        } else if ((int)angle == 90 || (int)angle == 270) {
            if ((int)(multiplier * targetX) > (int)(multiplier * x) && openDirections[0])
                angle = 0.0;
            else if ((int)(multiplier * targetX) < (int)(multiplier * x) && openDirections[2])
                angle = 180.0;
        }

        // Ensure valid movement
        if ((int)angle == 0 && !openDirections[0]) angle = 90.0;
        if ((int)angle == 90 && !openDirections[1]) angle = 180.0;
        if ((int)angle == 180 && !openDirections[2]) angle = 270.0;
        if ((int)angle == 270 && !openDirections[3]) angle = 0.0;
        if ((int)angle == 0 && !openDirections[0]) angle = 90.0;
    }

    void draw() {
        // Set color based on state
        if (!edible) {
            glColor3f(color[0], color[1], color[2]);
        } else {
            if (edibleTimer < 150) {
                int flash = (edibleTimer / 10) % 2;
                glColor3f(flash, flash, 1.0f);
            } else {
                glColor3f(0.0f, 0.0f, 1.0f);
            }
        }

        if (eaten) {
            glColor3f(1.0f, 1.0f, 0.0f);
        }

        glPushMatrix();
        glTranslatef(x, -y, 0.0);
        glTranslatef(0.5, 0.6, 0.0);
        glTranslatef((float)BOARD_HEIGHT / -2.0f, (float)BOARD_WIDTH / 2.0f, 0.5);
        glutSolidSphere(0.5, 15, 10);
        glPopMatrix();
    }

    void reset(double startX, double startY, int jailDelay) {
        x = startX;
        y = startY;
        angle = 90.0;
        speed = maxSpeed;
        edible = false;
        eaten = false;
        inJail = true;
        jailTimer = jailDelay;
        transporting = false;
    }
};

Ghost* ghosts[NUM_GHOSTS];

// ==================== UTILITY FUNCTIONS ====================
bool isCellOpen(int x, int y) {
    if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
        return false;
    }
    return (board_array[y][x] == 0);
}

void drawText(float x, float y, void* font, const char* text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void resetPebbles() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            temp_pebble_array[i][j] = pebble_array[i][j];
        }
    }
    pebbles_left = TOTAL_PEBBLES;
}

void resetPacman() {
    pacmanX = 13.5;
    pacmanY = 23.0;
    pacmanAngle = 90.0;
    isAnimating = false;
}

void resetGhosts() {
    startCountdown = 3;
    int startPositions[NUM_GHOSTS] = {11, 12, 15, 16};
    float ghostColors[NUM_GHOSTS][3] = {
        {1.0f, 0.0f, 0.0f},    // Red (Blinky)
        {1.0f, 0.75f, 0.8f},   // Pink (Pinky)
        {0.0f, 1.0f, 1.0f},    // Cyan (Inky)
        {1.0f, 0.6f, 0.0f}     // Orange (Clyde)
    };

    for (int i = 0; i < NUM_GHOSTS; i++) {
        ghosts[i]->reset(startPositions[i], 14, (i + 1) * 33 + 33);
        ghosts[i]->maxSpeed = 0.03 - 0.003 * i;
        ghosts[i]->speed = ghosts[i]->maxSpeed;
        for (int j = 0; j < 3; j++) {
            ghosts[i]->color[j] = ghostColors[i][j];
        }
    }
}

void updateGameTime() {
    if (currentState == STATE_PLAYING && !gameOver) {
        time_t currentTime = time(NULL);
        gameTimeSeconds = (int)difftime(currentTime, gameStartTime) - totalPausedDuration;
    }
}

// ==================== PACMAN FUNCTIONS ====================
void movePacman() {
    pacmanX += pacmanSpeed * cos(M_PI / 180.0 * pacmanAngle);
    pacmanY += pacmanSpeed * sin(M_PI / 180.0 * pacmanAngle);

    // Check for direction changes
    if (isAnimating && currentKey == GLUT_KEY_UP &&
        (int)pacmanX - pacmanX > -0.1 && pacmanAngle != 270.0) {
        if (isCellOpen(pacmanX, pacmanY - 1)) {
            isAnimating = true;
            pacmanAngle = 270.0;
        }
    } else if (isAnimating && currentKey == GLUT_KEY_DOWN &&
               (int)pacmanX - pacmanX > -0.1 && pacmanAngle != 90.0) {
        if (isCellOpen(pacmanX, pacmanY + 1)) {
            isAnimating = true;
            pacmanAngle = 90.0;
        }
    } else if (isAnimating && currentKey == GLUT_KEY_LEFT &&
               (int)pacmanY - pacmanY > -0.1 && pacmanAngle != 180.0) {
        if (isCellOpen(pacmanX - 1, pacmanY)) {
            isAnimating = true;
            pacmanAngle = 180.0;
        }
    } else if (isAnimating && currentKey == GLUT_KEY_RIGHT &&
               (int)pacmanY - pacmanY > -0.1 && pacmanAngle != 0.0) {
        if (isCellOpen(pacmanX + 1, pacmanY)) {
            isAnimating = true;
            pacmanAngle = 0.0;
        }
    }
}

void drawPacman() {
    glColor3f(1.0f, 1.0f, 0.0f);  // Yellow color
    glPushMatrix();
    glTranslatef(pacmanX, -pacmanY, 0.0);
    glTranslatef(0.5, 0.6, 0.0);
    glTranslatef((float)BOARD_HEIGHT / -2.0f, (float)BOARD_WIDTH / 2.0f, 0.5);
    glutSolidSphere(0.5, 15, 10);
    glPopMatrix();
}

// ==================== RENDERING FUNCTIONS ====================
void createDisplayLists() {
    // Wall segments
    displayLists[1] = glGenLists(1);
    glNewList(displayLists[1], GL_COMPILE);
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 1.0, 1.0);
    glEnd();
    glEndList();

    displayLists[2] = glGenLists(1);
    glNewList(displayLists[2], GL_COMPILE);
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 1.0, 1.0);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glVertex3f(1.0, 0.0, 0.0);
    glVertex3f(1.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glEnd();
    glEndList();

    displayLists[3] = glGenLists(1);
    glNewList(displayLists[3], GL_COMPILE);
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 1.0f);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 1.0, 1.0);
    glNormal3f(1.0f, 0.0f, 0.0f);
    glVertex3f(1.0, 1.0, 0.0);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(1.0, 0.0, 1.0);
    glVertex3f(1.0, 0.0, 0.0);
    glEnd();
    glEndList();

    displayLists[4] = glGenLists(1);
    glNewList(displayLists[4], GL_COMPILE);
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.2f, 0.5f);
    glNormal3f(1.0f, 0.0f, 1.0f);
    glVertex3f(1.0, 1.0, 1.0);
    glVertex3f(0.0, 1.0, 1.0);
    glVertex3f(0.0, 0.0, 1.0);
    glVertex3f(1.0, 0.0, 1.0);
    glEnd();
    glEndList();
}

void drawMaze() {
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            int listIndex = 0;
            glPushMatrix();
            glTranslatef(-(float)BOARD_HEIGHT / 2.0f, -(float)BOARD_WIDTH / 2.0f, 0.0);
            glTranslatef(j, BOARD_WIDTH - i, 0.0);
            glPushMatrix();
            glTranslatef(0.5, 0.5, 0.0);

            switch (board_array[i][j]) {
                case 4:
                    glRotatef(90.0, 0.0, 0.0, 1.0);
                case 3:
                    glRotatef(90.0, 0.0, 0.0, 1.0);
                case 2:
                    glRotatef(90.0, 0.0, 0.0, 1.0);
                case 1:
                    listIndex = 1;
                    break;
                case 6:
                    glRotatef(90.0, 0.0, 0.0, 1.0);
                case 5:
                    listIndex = 2;
                    break;
                case 10:
                    glRotatef(90.0, 0.0, 0.0, 1.0);
                case 9:
                    glRotatef(90.0, 0.0, 0.0, 1.0);
                case 8:
                    glRotatef(90.0, 0.0, 0.0, 1.0);
                case 7:
                    listIndex = 3;
                    break;
            }

            glScalef(1.0, 1.0, 0.5);
            glTranslatef(-0.5, -0.5, 0.0);
            glCallList(displayLists[listIndex]);
            glPopMatrix();

            if (listIndex != 0 || board_array[i][j] == 11) {
                glTranslatef(0.0, 0.0, -0.5);
                glCallList(displayLists[4]);
            }
            glPopMatrix();

            // Draw pebbles
            if (temp_pebble_array[i][j] > 0) {
                float pebbleColor = 1.0f / (float)temp_pebble_array[i][j];
                glColor3f(1.0f, pebbleColor, 0.0f);
                glPushMatrix();
                glTranslatef(-(float)BOARD_HEIGHT / 2.0f, -(float)BOARD_WIDTH / 2.0f, 0.0);
                glTranslatef(j, BOARD_WIDTH - i, 0.0);
                glTranslatef(0.5, 0.5, 0.5);
                float pebbleSize = 0.1f * (float)temp_pebble_array[i][j];
                glutSolidSphere(pebbleSize, 6, 6);
                glPopMatrix();
            }
        }
    }
    drawPacman();
