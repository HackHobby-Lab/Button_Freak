
int speed = 4;
int playerSpeed = 5;

int countX;
int heart = 5;
int points = 0;

int playerX;
// Ball properties
struct Ball {
  int x, y;
  int radius;
  int xVelocity, yVelocity;
  uint16_t color;
};

bool gameWin = false;

const int numRows = 7;          // Number of rows of tiles
const int numCols = 6;          // Number of columns of tiles
const int tileWidth = 25;       // Width of each tile
const int tileHeight = 8;       // Height of each tile
const int tileSpacingRo = 6;    // Spacing between tiles
const int tileSpacingCol = 18;  // Spacing between tiles

struct Tile {
  int x, y;
  uint16_t color;
  bool present;
};

Tile tiles[numRows][numCols];

Ball ball = { 100, 30, 8, 1, speed, COLOR_RGB565_GREEN };


void bounceInit() {

  tft.fillScreen(COLOR_RGB565_BLACK);

  // Initialize tiles
  for (int row = 2; row < numRows; row++) {
    for (int col = 0; col < numCols; col++) {
      tiles[row][col].x = col * (tileWidth + tileSpacingCol);
      tiles[row][col].y = row * (tileHeight + tileSpacingRo);
      tiles[row][col].color = random(0xFFFF);  // Random color for each tile
      tiles[row][col].present = true;
    }
  }

  ball.y = screenHeight / 2 - 60;
  //playerX = screenWidth / 2 - 10; // Initialize playerX at the center
  drawTiles();
  drawPlayer();
  drawBall();
}

void bounceBall() {

  if (gameOver) {
    noTone(buzzer);
    tft.fillScreen(COLOR_RGB565_BLACK);
    Serial.println("You Lost");
    tone(buzzer, 700, 100);
    tft.setCursor(70, 140);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println("Game Over");
    tone(buzzer, 700, 200);
    tft.setCursor(100, 160);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println(points);
    tone(buzzer, 1000, 100);
    delay(20);
    tft.fillScreen(COLOR_RGB565_BLACK);
    currentState = MENU;
    displayMenu();
    return;
    gameOver = false;
  } 
  else if (gameWin){
    noTone(buzzer);
    tft.fillScreen(COLOR_RGB565_BLACK);
    Serial.println("You Won");
    tone(buzzer, 7000, 100);
    tft.setCursor(70, 140);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println("You Won!");
    tone(buzzer, 700, 200);
    tft.setCursor(100, 160);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println(points);
    tone(buzzer, 10000, 100);
    delay(20);
    tft.fillScreen(COLOR_RGB565_BLACK);
    currentState = MENU;
    displayMenu();
    return;
  } 
  else {
    tft.setCursor(0, 0);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_LGRAY);
    tft.println("Hearts");
    tft.setCursor(95, 0);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println(heart);

    tft.setCursor(120, 0);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_LGRAY);
    tft.println("Points ");
    tft.setCursor(210, 0);
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println(points);

    leftBounceButtonHandler();
    drawBall();
    if (leftButtonState == HIGH) {
      switch (count) {
        case 1:
          movePlayerLeft();
          drawPlayer();
          break;
        case 2:
          movePlayerRight();
          drawPlayer();
          break;
        case 3:
          count = 1;
          break;
      }
    }

    // Check for collision
    if (checkCollision()) {
      // Handle collision
      Serial.println("Collision detected!");
      noTone(buzzer);
      tone(buzzer, 1000, 200);  // Play a sound for 200 milliseconds

      // Reverse the y-velocity of the ball to make it bounce back
      ball.yVelocity = -ball.yVelocity;
    }
    checkTileCollisions();
  }
}

void drawPlayer() {
  tft.fillRect(playerX, 300, 30, 8, COLOR_RGB565_YELLOW);
}

void drawTiles() {
  for (int row = 0; row < numRows; row++) {
    for (int col = 0; col < numCols; col++) {
      if (tiles[row][col].present) {
        tft.fillRect(tiles[row][col].x, tiles[row][col].y, tileWidth, tileHeight, tiles[row][col].color);
      }
    }
  }
}

void drawBall() {
  // Save the current position to clear later
  int prevX = ball.x;
  int prevY = ball.y;

  // Update ball position
  ball.x += ball.xVelocity;
  ball.y += ball.yVelocity;

  // Check for collisions with the walls
  if (ball.x - ball.radius < 0 || ball.x + ball.radius > tft.width()) {
    ball.xVelocity = -ball.xVelocity;  // Reverse horizontal direction
  }
  if (ball.y - ball.radius < 0) {
    ball.yVelocity = -ball.yVelocity;  // Reverse vertical direction
  }
  if (ball.y + ball.radius > 310) {
    ball.y = screenHeight / 2 - 60;
    ball.x = screenWidth / 2;
    ball.yVelocity = speed;
    tft.fillRect(90, 0, 25, 25, COLOR_RGB565_BLACK);
    heart -= 1;
    if (heart == 0) {
      gameOver = true;
    }
  }

  // Clear the previous position of the ball
  tft.fillCircle(prevX, prevY, ball.radius, COLOR_RGB565_BLACK);

  // Draw the ball at the new position
  tft.fillCircle(ball.x, ball.y, ball.radius, ball.color);
}

void movePlayerLeft() {
  clearSegments();
  countX = countX + playerSpeed;
  playerX = countX;
  Serial.println(countX);
  if (playerX - playerSpeed >= 210) {
    playerX = 210;
    countX = 210;
  }
}

void movePlayerRight() {
  clearSegmentsOther();
  countX = countX - playerSpeed;
  playerX = countX;
  Serial.println(countX);
  if (playerX + playerSpeed <= 0) {
    playerX = 0;
    countX = 0;
  }
}

void clearSegments() {
  tft.fillRect(playerX - 2, 300, 30, 8, COLOR_RGB565_BLACK);
}

void clearSegmentsOther() {
  tft.fillRect(playerX + 2, 300, 30, 8, COLOR_RGB565_BLACK);
}

void leftBounceButtonHandler() {
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

bool checkCollision() {
  // Check if the ball is within the horizontal bounds of the player
  bool withinX = ball.x + ball.radius >= playerX && ball.x - ball.radius <= playerX + 30;

  // Check if the ball is within the vertical bounds of the player
  bool withinY = ball.y + ball.radius >= 300 && ball.y - ball.radius <= 300 + 20;

  return withinX && withinY;
}

void checkTileCollisions() {
  for (int row = 0; row < numRows; row++) {
    for (int col = 0; col < numCols; col++) {
      if (tiles[row][col].present) {
        bool withinX = ball.x + ball.radius >= tiles[row][col].x && ball.x - ball.radius <= tiles[row][col].x + tileWidth;
        bool withinY = ball.y + ball.radius >= tiles[row][col].y && ball.y - ball.radius <= tiles[row][col].y + tileHeight;

        if (withinX && withinY) {
          // Collision detected, remove tile and reverse ball direction
          ball.yVelocity = -ball.yVelocity;
          tiles[row][col].present = false;
          tft.fillRect(tiles[row][col].x, tiles[row][col].y, tileWidth, tileHeight, COLOR_RGB565_BLACK);  // Clear the tile
          tft.fillRect(240 - 40, 0, 35, 25, COLOR_RGB565_BLACK);
          points += 1;
          Serial.println("Collision detected with tile!");
          noTone(buzzer);
          tone(buzzer, 1000, 200);  // Play a sound for 200 milliseconds
          if (points >= 30) {
            gameWin = true;
          }
          if (points >= 15) {
            playerSpeed = 10;
          }
        }
      }
    }
  }
}
