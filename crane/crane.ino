#include <Stepper.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

#define STEPS 32
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

String movestring = "";
long move = 0;
Stepper stepper1(STEPS, 8, 10, 9, 11); 
Stepper baseStepper(STEPS, 2, 4, 3, 5);

Servo scoopServo;

// 0x27 is the most common I2C address for a 16x2 backpack;
// change to 0x3F if your module does not respond.
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

int val = 0;

// Print a two-line message to the LCD, overwriting both lines in place
// (no lcd.clear()) to avoid flicker on transitions.
void showOnLCD(String line1, String line2) {
    lcd.setCursor(0, 0);
    lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print(line1.substring(0, LCD_COLS));
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(0, 1);
    lcd.print(line2.substring(0, LCD_COLS));
}

// Spinner frames for the "moving" animation. The HD44780 character ROM maps
// 0x5C to the yen symbol (¥) instead of a backslash, so we define our own
// backslash as a custom character (slot 0) and use it as the 4th frame.
byte backslashGlyph[8] = {
    0b10000,
    0b01000,
    0b00100,
    0b00010,
    0b00001,
    0b00000,
    0b00000,
    0b00000
};
const char SPINNER[3] = {'|', '/', '-'};
uint8_t spinnerPos = 0;

// Draw the current spinner frame at (col,row); frame 3 is the custom backslash.
void drawSpinner(uint8_t col, uint8_t row) {
    lcd.setCursor(col, row);
    if (spinnerPos == 3) {
        lcd.write((uint8_t)0);
    } else {
        lcd.print(SPINNER[spinnerPos]);
    }
    spinnerPos = (spinnerPos + 1) % 4;
}

// Step a stepper in chunks, updating the LCD with remaining steps and a spinner
// so progress is visible while the motor is actually moving. Uses long so moves
// up to 50000+ steps don't overflow the 16-bit int. Clears only once at the
// start to avoid flicker from repeated lcd.clear() calls.
void moveStepper(Stepper &stepper, long steps) {
    long total = (steps < 0) ? -steps : steps;   // avoid abs() overflow on long
    int dir = (steps < 0) ? -1 : 1;
    long remaining = total;
    long lastShown = -1;                  // forces the initial display
    unsigned long lastSpinner = 0;
    const unsigned long SPINNER_MS = 420; // spinner frame interval (0.4 rot/s = 1.6 fps)
    const int STEP_CHUNK = 1;             // 1 step/chunk => step() blocks only ~9 ms,
                                          // so the spinner timer fires reliably
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Moving");
    while (remaining > 0) {
        int thisChunk = (remaining > STEP_CHUNK) ? STEP_CHUNK : (int)remaining;
        stepper.step(dir * thisChunk);
        remaining -= thisChunk;
        // Spinner advances on a fixed time interval, so its speed is independent
        // of the total move size (a huge move no longer makes it crawl).
        if (millis() - lastSpinner >= SPINNER_MS) {
            drawSpinner(7, 0);
            lastSpinner = millis();
        }
        // Refresh the remaining-count line only every 100 steps (and at the end)
        // so the number doesn't churn on every single update.
        if (lastShown == -1 || lastShown - remaining >= 100 || remaining == 0) {
            lcd.setCursor(0, 1);
            lcd.print("                ");
            lcd.setCursor(0, 1);
            lcd.print(String(remaining) + " steps left");
            lastShown = remaining;
        }
    }
}

// Show a spinner animation for a fixed duration (used for non-stepper moves).
void spinFor(unsigned long ms, String line1) {
    unsigned long start = millis();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1.substring(0, LCD_COLS));
    lcd.setCursor(0, 1);
    lcd.print("moving");
    while (millis() - start < ms) {
        drawSpinner(8, 1);
        delay(100);
    }
}

void setup() {
    Serial.begin(9600);
    stepper1.setSpeed(200);
    baseStepper.setSpeed(400);
    scoopServo.attach(6);

    lcd.init();
    lcd.backlight();
    lcd.createChar(0, backslashGlyph);
    Wire.setClock(400000);   // faster I2C bus = snappier, less flickery updates
    showOnLCD("Crane ready", "");
}
void loop() {

    if (Serial.available()) {
        movestring = Serial.readString();
        movestring.trim();
        
        if (movestring.length() > 1) {
            char motor = movestring.charAt(0);
            String stepsStr = movestring.substring(1);
            move = stepsStr.toInt();
            
            if (motor == 'a') {
                moveStepper(stepper1, move);
                Serial.println("Stepper 1 moved " + String(move) + " steps.");
                showOnLCD("Stepper 1", "moved " + String(move) + " steps");
                delay(1500);
                showOnLCD("Crane ready", "");
            } else if (motor == 'b') {
                moveStepper(baseStepper, move);
                Serial.println("Base stepper moved " + String(move) + " steps.");
                showOnLCD("Base stepper", "moved " + String(move) + " steps");
                delay(1500);
                showOnLCD("Crane ready", "");
            } else if (motor == 's') {
                scoopServo.write(move);
                Serial.println("Scoop servo moved to " + String(move));
                //180 = fully open, 0 = fully closed
                spinFor(600, "Scoop servo");
                showOnLCD("Scoop servo", "moved to " + String(move));
                delay(1500);
                showOnLCD("Crane ready", "");
            }
            else {
                Serial.println("Invalid motor. Use 'a', 'b', or 's' followed by steps.");
                showOnLCD("Invalid motor", "use a/b/s + n");
                delay(1500);
                showOnLCD("Crane ready", "");
            }
        }
    }
    movestring = "";
}
