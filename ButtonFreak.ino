#include <Arduino.h>
#include <Wire.h>
#include <DFRobot_GDL.h>

// Pin Definitions
#define leftBtn 5
#define buzzer 4

#define TFT_DC 19
#define TFT_CS 6
#define TFT_RST 20
#define TFT_BL -1

const int screenWidth = 240;
const int screenHeight = 320;


// Global Variables
DFRobot_ST7789_240x320_HW_SPI tft(/*dc=*/TFT_DC, /*cs=*/TFT_CS, /*rst=*/TFT_RST);

enum GameState { MENU, GAME1, GAME2, GAME_OVER };
GameState currentState = MENU;
int selectedGame = 0;
int count;
int buttonState;
int leftButtonState;
int lastLeftState = LOW;
int lastButtonState = LOW;
unsigned long lastButtonPressTime = 0;
const int debounceDelay = 30;
int lastDebounceTime;
const unsigned long selectionTimeout = 5000; // 5 seconds timeout
bool gameIsOn = false;
bool game = true;
bool gameOver = false;

void setup() {
    Serial.begin(115200);
    pinMode(leftBtn, INPUT);
    pinMode(buzzer, OUTPUT);
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(COLOR_RGB565_BLACK);
    displayMenu();
    tft.fillRect(0, 0, 240, 60, COLOR_RGB565_BLACK); // Clear only the title area
    tft.setTextSize(3);
    tft.setCursor(240 / 2 - 100, 0);
    
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println("ButtonFreak");
}

void loop() {
  // if (game == false) {
  //   gameIsOn = false;
  //   noTone(buzzer);
  //       tft.fillScreen(COLOR_RGB565_BLACK);

  //   displayMenu();
    
  // }
    checkButton();
    switch (currentState) {
        case MENU:
            handleMenuInput();
            break;
        case GAME1:
            game1Loop();
            break;
        case GAME2:
            game2Loop();
            break;
        case GAME_OVER:
            Serial.println("Meant for GameOver but not yet implemented");
            break;
    }

    if (currentState == MENU && (millis() - lastButtonPressTime > selectionTimeout)) {
        // Automatically select the game if timeout has passed
        if (selectedGame == 1) {
            currentState = GAME1;
            startGame1();
        } else if (selectedGame == 2) {
            currentState = GAME2;
            startGame2();
        }
    }
}

void displayMenu() {
  gameIsOn = false;
    //tft.fillScreen(COLOR_RGB565_BLACK);
    tft.fillRect(0, 0, 240, 60, COLOR_RGB565_BLACK); // Clear only the title area
    tft.setTextSize(3);
    tft.setCursor(240 / 2 - 100, 0);
    
    tft.setTextColor(COLOR_RGB565_YELLOW);
    tft.println("ButtonFreak");
  // game = false;
  // gameOver = false;
    // game = true;
    tft.setTextSize(2);
    tft.setTextColor(COLOR_RGB565_WHITE);

    // Clear and redraw the game options area
    tft.fillRect(0, 100, 240, 100, COLOR_RGB565_BLACK);
    
    tft.setCursor(240 / 2 - 70, 100);
    tft.setTextColor(selectedGame == 1 ? COLOR_RGB565_GREEN : COLOR_RGB565_WHITE);
    tft.print("Snake Hunt ");
    if (selectedGame == 1) tft.print("<");

    tft.setCursor(240 / 2 - 70, 150);
    tft.setTextColor(selectedGame == 2 ? COLOR_RGB565_GREEN : COLOR_RGB565_WHITE);
    tft.print("Bounce Ball ");
    if (selectedGame == 2) tft.print("<");
}

void handleMenuInput() {
    if (buttonState == HIGH && lastButtonState == LOW) {
      int currentTime = millis();
        if (selectedGame == 1 && (currentTime - lastDebounceTime) > 1000) {
            currentState = GAME1;
            startGame1();
        } 
        else if (selectedGame == 2 && (currentTime - lastDebounceTime) > 1000) {
            currentState = GAME2;
            startGame2();
        }
        lastDebounceTime = currentTime;
    }
}

void startGame1() {
    // Initialize Game 1
    tft.fillScreen(COLOR_RGB565_BLACK);
    init();

}

void startGame2() {
    // Initialize Game 2
    bounceInit();
}

void game1Loop() {
    gameIsOn = true;
        snakeHunt();

}

void game2Loop() {
       gameIsOn = true;
       bounceBall();

}


void checkButton() {
  if(gameIsOn != true){
    int reading = digitalRead(leftBtn);
    if (reading != lastButtonState) {
        lastButtonPressTime = millis();
        lastDebounceTime = millis();
    }
    
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == HIGH) {
                selectedGame++;
                if (selectedGame > 2) selectedGame = 0;
                displayMenu();
                tone(buzzer,  1000, 150);
            }
        }
    
    lastButtonState = reading;
}
}
