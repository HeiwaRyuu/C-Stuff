
#include <windows.h>
#include <stdio.h> // For sprintf

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Game state variables
int paddleWidth = 20, paddleHeight = 100;
int leftPaddleY, rightPaddleY;
int ballX, ballY, ballVelocityX = 3, ballVelocityY = 3, ballSize = 20;
int paddleSpeed = 20;
int leftScore = 0, rightScore = 0;
BOOL gameOver = FALSE; // To track if the game is over

// Initialize game state
void InitializeGame() {
    leftPaddleY = rightPaddleY = (WINDOW_HEIGHT - paddleHeight) / 2;
    ballX = WINDOW_WIDTH / 2 - ballSize / 2;
    ballY = WINDOW_HEIGHT / 2 - ballSize / 2;
}

// Handle paddle movement based on key presses
void MovePaddles(WPARAM wParam) {
    switch (wParam) {
        case VK_UP:
            if (rightPaddleY > 0) rightPaddleY -= paddleSpeed;
            break;
        case VK_DOWN:
            if (rightPaddleY < WINDOW_HEIGHT - paddleHeight) rightPaddleY += paddleSpeed;
            break;
        case 0x57: // W KEY
            if (leftPaddleY > 0) leftPaddleY -= paddleSpeed;
            break;
        case 0x53: // S KEY
            if (leftPaddleY < WINDOW_HEIGHT - paddleHeight) leftPaddleY += paddleSpeed;
            break;
        default:
            break;
        // To control the left paddle, you can add additional case statements here
    }
}

// Update ball position and handle collisions
void UpdateBallAndScore() {
    ballX += ballVelocityX;
    ballY += ballVelocityY;

    // Collision with top or bottom
    if (ballY <= 0 || ballY + ballSize*2 >= WINDOW_HEIGHT) ballVelocityY = -ballVelocityY;

    // Collision with paddles
    if (((ballX <= paddleWidth && ballX >= paddleWidth/2) && ballY + ballSize >= leftPaddleY && ballY <= leftPaddleY + paddleHeight) ||
        ((ballX + ballSize >= WINDOW_WIDTH - paddleWidth && ballX + ballSize <= WINDOW_WIDTH - paddleWidth/2) && ballY + ballSize >= rightPaddleY && ballY <= rightPaddleY + paddleHeight)) {
        ballVelocityX = -ballVelocityX;
        if(ballVelocityX > 0){
            ballX = paddleWidth;
        }
        else{
            ballX = WINDOW_WIDTH - paddleWidth - ballSize;
        }
    }
    else if (ballX < 0) { // Ball passed left paddle
        rightScore++;
        InitializeGame(); // Reset game state
    } else if (ballX > WINDOW_WIDTH - ballSize) { // Ball passed right paddle
        leftScore++;
        InitializeGame(); // Reset game state
    }

    // Check for game over condition
    if (leftScore >= 5 || rightScore >= 5) {
        gameOver = TRUE;
    }
}

// Drawing function including score display and game over message
void Draw(HWND hwnd, HDC hdc) {
    char scoreText[50];
    SetBkMode(hdc, TRANSPARENT); // To ensure text background is transparent
    SetTextColor(hdc, RGB(255, 255, 255)); // White text

    // Display scores
    sprintf(scoreText, "Left: %d  Right: %d", leftScore, rightScore);
    TextOut(hdc, WINDOW_WIDTH / 2 - 50, 10, scoreText, strlen(scoreText));

    // Define the font characteristics
    int fontSize = 48;
    char *fontFace = "Arial";
    // Create a larger font
    HFONT hFont = CreateFont(
        fontSize,                   // nHeight
        0,                          // nWidth
        0,                          // nEscapement
        0,                          // nOrientation
        FW_BOLD,                    // fnWeight
        FALSE,                      // fdwItalic
        FALSE,                      // fdwUnderline
        FALSE,                      // fdwStrikeOut
        DEFAULT_CHARSET,            // fdwCharSet
        OUT_DEFAULT_PRECIS,         // fdwOutputPrecision
        CLIP_DEFAULT_PRECIS,        // fdwClipPrecision
        DEFAULT_QUALITY,            // fdwQuality
        DEFAULT_PITCH | FF_SWISS,   // fdwPitchAndFamily
        fontFace);                   // lpszFace

    // Select the large font into the DC
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    SIZE textSizeGameOver;
    SIZE textSize;

    if (gameOver) {
        // Display game over message
        char* gameOverMsg = "Game Over!";
        char* winnerTxt = NULL;
        if(leftScore>=5){
            winnerTxt = "Left Wins!";
        }
        else{
            winnerTxt = "Right Wins!";
        }
        int x = 0;
        int y = 0;
        GetTextExtentPoint32(hdc, gameOverMsg, strlen(gameOverMsg), &textSizeGameOver);
        x = (WINDOW_WIDTH - textSizeGameOver.cx) / 2; // Center Horizontally
        y = (WINDOW_HEIGHT - textSizeGameOver.cy) / 2 - textSizeGameOver.cy; // Center Vertically
        TextOut(hdc, x, y, gameOverMsg, strlen(gameOverMsg));

        GetTextExtentPoint32(hdc, winnerTxt, strlen(winnerTxt), &textSize);
        x = (WINDOW_WIDTH - textSize.cx) / 2; // Center Horizontally
        y = (WINDOW_HEIGHT - textSize.cy) / 2 + textSize.cy/2; // Center Vertically
        TextOut(hdc, x, y, winnerTxt, strlen(winnerTxt));
    }

    // Restore the original font
    SelectObject(hdc, hOldFont);

    // Clean up
    DeleteObject(hFont);
}



LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HBRUSH brush = CreateSolidBrush(RGB(255, 255, 255)); // White color

            // Draw paddles and ball
            if(!gameOver){
                FillRect(hdc, &(RECT){0, leftPaddleY, paddleWidth, leftPaddleY + paddleHeight}, brush);
                FillRect(hdc, &(RECT){WINDOW_WIDTH - paddleWidth, rightPaddleY, WINDOW_WIDTH, rightPaddleY + paddleHeight}, brush);
                FillRect(hdc, &(RECT){ballX, ballY, ballX + ballSize, ballY + ballSize}, brush);
            }

            Draw(hwnd, hdc); // Call the function to draw scores and game over message

            DeleteObject(brush);
            EndPaint(hwnd, &ps);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KEYDOWN:
            if (gameOver) {
                if (wParam == VK_RETURN) { // Restart the game if Enter key is pressed
                    leftScore = 0;
                    rightScore = 0;
                    InitializeGame(); // Reset game state
                    gameOver = FALSE; // Reset game over flag
                    InvalidateRect(hwnd, NULL, TRUE); // Redraw window
                }
            } else {
                MovePaddles(wParam);
                InvalidateRect(hwnd, NULL, TRUE); // Redraw window for paddle movement
            }
            break;
        case WM_TIMER:
            if (!gameOver) {
                UpdateBallAndScore();
                InvalidateRect(hwnd, NULL, TRUE); // Redraw window for game updates
            }
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl = {0};

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = "PongGame";
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)(COLOR_BACKGROUND+1);

    if (!RegisterClassEx(&wincl)) return 0;

    hwnd = CreateWindowEx(
        0, "PongGame", "Pong Game",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT,
        HWND_DESKTOP, NULL, hThisInstance, NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    InitializeGame();
    SetTimer(hwnd, 1, 10, NULL); // Set a timer to update game state

    while (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}
