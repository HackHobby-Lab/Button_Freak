
// int count;
int countRight;
int scores;

const int blockSize = 4;

int previousMillis;
int readLeftState;

int readRightState;
int rightButtonState;
int lastRightState = LOW;
int previousFoodMillis;

int currentXCoordinates;
int currentYCoordinates;
int i;

struct Point {
  int x;
  int y;
};

Point snake[500];
int snakeLength = 5;
bool movingLeft;
bool movingRight;
Point food;
int lastXCoordinates;
int lastYCoordinates;
bool collisionFlag = true;
// bool game = true;
int delayFactor = 50;
int xPospre = 100;
int yPospre = 100;

void init() {

  // Initialize the snake's starting position
  for (int i = 0; i < snakeLength; i++) {
    snake[i] = { screenHeight / 2 + i * blockSize, screenWidth / 2 };
  }

  // Draw the initial snake
  drawSnake();
  drawFood();
}

void snakeHunt() {
  
  if (game == false) {
    tft.fillScreen(COLOR_RGB565_BLACK);
    
    snakeLength = 5;
    count = 0;
    Serial.println("You Lost");
    tone(buzzer,  1500, 100);
    tft.setCursor(70,140);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println("Game Over");
    tone(buzzer,  700, 200);
    tft.setCursor(100, 160);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println(scores);
    tone(buzzer,  1000, 100);
    delay(2000);
    noTone(buzzer);
    scores = 0;
    tft.fillScreen(COLOR_RGB565_BLACK);
    currentState = MENU;
    displayMenu();
    return;
    // displayMenu();

    // gameIsOn = false;
    // game = true; // Reset game flag for restart
  } else { 
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println("Score");
    
    tft.setCursor(240-30, 0);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println(scores);

    detectCollision();
    if(checkSelfCollision()){
      Serial.println("It's Self collision");
      game = false;
      
    }

    leftButtonHandler();
    switch (count) {
      case 1:
        moveSnakeRight();
        break;
      case 2:
        moveSnakeTop();
        break;
      case 3:
        moveSnakeLeft();
        break;
      case 4:
        moveSnakeBottom();
        break;
      case 5:
        count = 1;
        break;
    }

    drawSnake();
    delay(delayFactor);
  }
}

void leftButtonHandler() {
  int readLeftState = digitalRead(leftBtn);
  if (readLeftState != lastLeftState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > 40) {
    if (readLeftState != leftButtonState) {
      leftButtonState = readLeftState;
      if (leftButtonState == HIGH) {
        count++;
      }
    }
  }
  lastLeftState = readLeftState;
}

void drawSnake() {
  for (int i = 0; i < snakeLength; i++) {
    tft.fillCircle(snake[i].x, snake[i].y, blockSize, COLOR_RGB565_GREEN);
  }
}

void drawFood() {
  bool validFoodPosition = false;

  while (!validFoodPosition) {
    food.x = random(10, screenWidth - 10);
    food.y = random(30, screenHeight - 10);
    validFoodPosition = true;
    for (int i = 0; i < snakeLength; i++) {
      if (abs(food.x - snake[i].x) < blockSize * 2 && abs(food.y - snake[i].y) < blockSize * 2) {
        validFoodPosition = false;
        break;
      }
    }
  }
  tft.fillCircle(food.x, food.y, 6, COLOR_RGB565_RED);
}

void detectCollision() {
  int tolerance = 8; // Reduced tolerance for better accuracy

  if (abs(snake[0].x - food.x) <= tolerance && abs(snake[0].y - food.y) <= tolerance) {
    Serial.println("Collision detected");
    tft.fillRect(240-35, 0, 35, 25, COLOR_RGB565_BLACK);
    scores++;
    tone(buzzer,  7000, 100);
    tft.fillCircle(food.x, food.y, 10, COLOR_RGB565_BLACK);
    drawFood();
    increaseSnakeLength();
  }
}

bool checkSelfCollision() {
  for (int i = 1; i < snakeLength; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      return true;
    }
  }
  return false;
}

void increaseSnakeLength() {
  snake[snakeLength] = snake[snakeLength - 1];
  snakeLength++;
  delayFactor = max(10, delayFactor - 1); // Ensure delayFactor doesn't go negative
}

void clearTailSegment() {
  tft.fillCircle(snake[snakeLength - 1].x, snake[snakeLength - 1].y, blockSize, COLOR_RGB565_BLACK);
}

void moveSnakeLeft() {
  clearTailSegment();
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  snake[0].y -= blockSize;
  if (snake[0].y < 30) {
    snake[0].y = screenHeight - blockSize;
  }
}

void moveSnakeRight() {
  clearTailSegment();
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  snake[0].y += blockSize;
  if (snake[0].y >= screenHeight) {
    snake[0].y = 30;
  }
}

void moveSnakeTop() {
  clearTailSegment();
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  snake[0].x -= blockSize;
  if (snake[0].x < 0) {
    snake[0].x = screenWidth - blockSize;
  }
}

void moveSnakeBottom() {
  clearTailSegment();
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }
  snake[0].x += blockSize;
  if (snake[0].x >= screenWidth) {
    snake[0].x = 0;
  }
}
