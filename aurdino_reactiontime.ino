#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define IR_SENSOR_PIN 7  
#define LED_PIN 6
#define BUZZER_PIN 5
#define BUTTON_START 2  
#define BUTTON_STATS 3  
#define BUTTON_RESET 4  
#define BUTTON_CONTINUE 1  

LiquidCrystal_I2C lcd(0x3F, 16, 2);
long startTime;
bool gameStarted = false;
bool waitingForContinue = false; 

// ระดับความยาก
enum Difficulty { EASY, MEDIUM, HARD };
Difficulty currentDifficulty = EASY;

const int TIME_LIMIT = 1000;  
const int TOO_FAST_LIMIT = 50;  
unsigned long previousMillis = 0;

void setup() {
    pinMode(IR_SENSOR_PIN, INPUT);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(BUTTON_START, INPUT_PULLUP);
    pinMode(BUTTON_STATS, INPUT_PULLUP);
    pinMode(BUTTON_RESET, INPUT_PULLUP);
    pinMode(BUTTON_CONTINUE, INPUT_PULLUP);

    lcd.init();
    lcd.backlight();

    resetScreen();
}

void loop() {
    unsigned long currentMillis = millis(); 

    if (waitingForContinue && digitalRead(BUTTON_CONTINUE) == LOW) {
        if (currentMillis - previousMillis >= 200) { 
            previousMillis = currentMillis;
            changeDifficulty(); // เปลี่ยนระดับความยาก
        }
    }

    if (digitalRead(BUTTON_START) == LOW) {  
        if (currentMillis - previousMillis >= 200) { 
            previousMillis = currentMillis;
            startGame();
        }
    }

    if (digitalRead(BUTTON_STATS) == LOW) {  
        if (currentMillis - previousMillis >= 200) {
            previousMillis = currentMillis;
            showBestTimes();
        }
    }

    if (digitalRead(BUTTON_RESET) == LOW) {  
        if (currentMillis - previousMillis >= 200) {
            previousMillis = currentMillis;
            resetBestTimes();
        }
    }
}

// 📌 ฟังก์ชันเปลี่ยนระดับความยาก
void changeDifficulty() {
    currentDifficulty = (Difficulty)((currentDifficulty + 1) % 3);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Difficulty: ");
    
    switch (currentDifficulty) {
        case EASY:
            lcd.print("Easy");
            break;
        case MEDIUM:
            lcd.print("Medium");
            break;
        case HARD:
            lcd.print("Hard");
            break;
    }

    lcd.setCursor(0, 1);
    lcd.print("Press Start");
}

// 📌 ฟังก์ชันเริ่มเกม
void startGame() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wait for LED...");
    
    int waitTime;
    switch (currentDifficulty) {
        case EASY:
            waitTime = random(2000, 5000);
            break;
        case MEDIUM:
            waitTime = random(3000, 6000);
            tone(BUZZER_PIN, 500, 300);
            break;
        case HARD:
            waitTime = random(4000, 7000);
            tone(BUZZER_PIN, 1000, 500);
            delay(random(500, 1500));
            noTone(BUZZER_PIN);
            break;
    }

    delay(waitTime);  

    digitalWrite(LED_PIN, HIGH);  
    tone(BUZZER_PIN, 1000, 200);  

    startTime = millis();
    gameStarted = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("GO! Move Hand!");

    while (gameStarted) {
        if (digitalRead(IR_SENSOR_PIN) == HIGH) {  
            long reactionTime = millis() - startTime;

            lcd.clear();
            lcd.setCursor(0, 0);

            if (reactionTime < TOO_FAST_LIMIT) {
                lcd.print("Too fast!");
            } else if (reactionTime > TIME_LIMIT) {
                lcd.print("Too slow!");
            } else {
                lcd.print("Time: ");
                lcd.print(reactionTime);
                lcd.print(" ms");
                updateBestTimes(reactionTime);
            }

            delay(2000);
            digitalWrite(LED_PIN, LOW);
            noTone(BUZZER_PIN);
            gameStarted = false;

            waitingForContinue = true;
            lcd.setCursor(0, 1);
            lcd.print("Press Continue");
            return;
        }
    }
}

// 📌 ฟังก์ชันโชว์สถิติ
void showBestTimes() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Best Times:");

    long bestTimes[3] = {0, 0, 0};
    long bestAllTime = 1000;

    for (int i = 0; i < 3; i++) {
        EEPROM.get(i * sizeof(long), bestTimes[i]);
    }
    EEPROM.get(3 * sizeof(long), bestAllTime);

    for (int i = 0; i < 3; i++) {
        lcd.setCursor(0, i + 1);
        lcd.print(i + 1);
        lcd.print(": ");
        if (bestTimes[i] == 0) {
            lcd.print("---");
        } else {
            lcd.print(bestTimes[i]);
            lcd.print("ms");
        }
    }

    lcd.setCursor(0, 1);
    lcd.print("Best Ever: ");
    lcd.print(bestAllTime);
    lcd.print("ms");

    waitingForContinue = true;
}

// 📌 ฟังก์ชันลบสถิติ
void resetBestTimes() {
    for (int i = 0; i < 4; i++) {
        EEPROM.put(i * sizeof(long), (long)1000);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Stats Reset!");

    waitingForContinue = true;
}

// 📌 ฟังก์ชันบันทึกค่า 3 อันดับดีที่สุด และ Best All-Time
void updateBestTimes(long newTime) {
    long bestTimes[3];
    long bestAllTime;

    for (int i = 0; i < 3; i++) {
        EEPROM.get(i * sizeof(long), bestTimes[i]);
    }
    EEPROM.get(3 * sizeof(long), bestAllTime);

    if (newTime >= TOO_FAST_LIMIT && newTime <= TIME_LIMIT) {
        for (int i = 0; i < 3; i++) {
            if (bestTimes[i] == 0 || newTime < bestTimes[i]) {
                for (int j = 2; j > i; j--) {
                    bestTimes[j] = bestTimes[j - 1]; 
                }
                bestTimes[i] = newTime;
                break;
            }
        }

        if (newTime < bestAllTime) {
            bestAllTime = newTime;
        }

        for (int i = 0; i < 3; i++) {
            EEPROM.put(i * sizeof(long), bestTimes[i]);
        }
        EEPROM.put(3 * sizeof(long), bestAllTime);
    }
}

// 📌 ฟังก์ชันรีเซ็ตหน้าจอ
void resetScreen() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready to play!");
    lcd.setCursor(0, 1);
    lcd.print("Press Start");
}

