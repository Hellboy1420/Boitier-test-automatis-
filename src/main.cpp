//=====================================================================================================================================================
// Necessary library inclusions
//=====================================================================================================================================================
#include <Adafruit_LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
//=====================================================================================================================================================



//=====================================================================================================================================================
// Pins definition
//=====================================================================================================================================================
const int pin_Emergency = 2;                      // Emergency stop signal input pin
const int pin_Wall_switch = 3;                    // Input pin for checking the 24V DC signal on the power supply
const int pin_Start = 5;                          // Input pin to check if the system is powered by pushing the start button of the machine
const int pin_Shutdown_request = 6;               // Input pin for checking the scanner shutdown request
const int out_pin_Shutdown_command = 8;           // Output pin to send a signal and turn off the scanner
const int button_next_sequence = 7;               // Pin where the push button is connected to move to the next sequence or the next step
const unsigned long SIGNAL_CHECK_INTERVAL = 100;  // Signal check interval every 100 ms
unsigned long lastSignalCheckTime = 0;            // Variable to track the time elapsed since the last check
//=====================================================================================================================================================



//=====================================================================================================================================================
// LCD screen initialization
//=====================================================================================================================================================
LiquidCrystal_I2C lcd(0x27,20,4);    // Adresse LCD : 0x27 ou 0x20
//=====================================================================================================================================================



//=====================================================================================================================================================
// Declaration of sequence functions
//=====================================================================================================================================================
void sequence_EMERGENCY();
void sequence_WALL_SWITCH_FEEDBACK();
void sequence_START_SCANNER();
void sequence_SHUTDOWN_REQUEST();
void sequence_SHUTDOWN_COMMAND();
//=====================================================================================================================================================


//=====================================================================================================================================================
// State variable to track which sequence we are in
//=====================================================================================================================================================
enum SequenceState {
  SEQUENCE_1,
  SEQUENCE_2,
  SEQUENCE_3,
  SEQUENCE_4,
  SEQUENCE_5
};
//=====================================================================================================================================================

SequenceState currentSequence = SEQUENCE_1;  // Initialization to the first sequence

//=====================================================================================================================================================
// Initialization of variables needed for the code to continue
//=====================================================================================================================================================
void setup() {
  // Configure pins as input or output
  pinMode(pin_Emergency, INPUT_PULLUP);
  pinMode(pin_Wall_switch, INPUT_PULLUP);
  pinMode(pin_Start, INPUT_PULLUP);
  pinMode(pin_Shutdown_request, INPUT_PULLUP);
  pinMode(out_pin_Shutdown_command, OUTPUT);
  pinMode(button_next_sequence, INPUT_PULLUP);
  
  //  LCD initialization
  lcd.begin (20,4);          // for a 16x4 LCD module
  lcd.home ();               // set the cursor to 0,0
  lcd.setBacklight(HIGH);    // turn on backlight to the maximum
  lcd.clear();               // clear the LCD display
}
//=====================================================================================================================================================



//=====================================================================================================================================================
// Main function that runs in a loop
//=====================================================================================================================================================
void loop() {
  // Check if the push button is pressed to move to the next sequence or the next step
  if (digitalRead(button_next_sequence) == LOW) {
    delay(1000);// Debounce
    while (digitalRead(button_next_sequence) == LOW) {
    }
    // Move to the next sequence
    currentSequence = static_cast<SequenceState>((currentSequence + 1) % (SEQUENCE_5 + 1));
  }
//=====================================================================================================================================================



//=====================================================================================================================================================
// Execute the current sequence
//=====================================================================================================================================================
  switch (currentSequence) {
    case SEQUENCE_1:
      sequence_EMERGENCY();
      break;
    case SEQUENCE_2:
      sequence_WALL_SWITCH_FEEDBACK();
      break;
    case SEQUENCE_3:
      sequence_START_SCANNER();
      break;
    case SEQUENCE_4:
      sequence_SHUTDOWN_REQUEST();
      break;
    case SEQUENCE_5:
      sequence_SHUTDOWN_COMMAND();
      break;
  }
}
//=====================================================================================================================================================



//=====================================================================================================================================================
// Sequence 1: Checks the signal received from the EMERGENCY STOP
//=====================================================================================================================================================
void sequence_EMERGENCY() {
  static bool countdownDone = false; // Variable to track if the countdown is done

  // Check if the time elapsed since the last check exceeds the defined interval
  if (millis() - lastSignalCheckTime >= SIGNAL_CHECK_INTERVAL) {
    lastSignalCheckTime = millis(); // Update the time of the last check

    lcd.clear();
    // If the countdown has not been done yet
    if (!countdownDone) {
      // Display the initial message on the LCD screen
      lcd.setCursor(3, 0);
      lcd.print(" First test : ");
      lcd.setCursor(3, 1);
      lcd.print("EMERGENCY STOP");

      // 10-second countdown
      for (int i = 10; i > 0; i--) {
        lcd.setCursor(4, 2);
        lcd.print("Waiting :" + String(i) + "s ");
        delay(1000);
      }

      // Mark that the countdown is done
      countdownDone = true;
    }
    // Check the input signal state after the countdown
    if (digitalRead(pin_Emergency) == LOW) {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("EMERGENCY STOP OK");
      lcd.setCursor(1, 2);
      lcd.print("Push next button");
      lcd.setCursor(1, 3);
      lcd.print("if the test is OK");
    } else {
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("EMERGENCY STOP NOK");
      lcd.setCursor(1, 2);
      lcd.print("Push next button");
      lcd.setCursor(1, 3);
      lcd.print("if the test is OK");
      
    }
  }
}
//=====================================================================================================================================================



//=====================================================================================================================================================
// Séquence 2 : Checks the signal received from the 24V DC power supply
//=====================================================================================================================================================
void sequence_WALL_SWITCH_FEEDBACK() {
  static bool countdownDone = false; // Variable to track if the countdown is done

  // Check if the time elapsed since the last check exceeds the defined interval
  if (millis() - lastSignalCheckTime >= SIGNAL_CHECK_INTERVAL) {
    lastSignalCheckTime = millis(); // Update the time of the last check

    lcd.clear();

    // If the countdown has not been done yet
    if (!countdownDone) {

    // Display the initial message on the LCD screen
    lcd.setCursor(3, 0);
    lcd.print("Second test :");
    lcd.setCursor(5, 1);
    lcd.print("Check the");
    lcd.setCursor(2, 2);
    lcd.print("POWER SUPPLY 24V");

    // 10-second countdown
      for (int i = 10; i > 0; i--) {
        lcd.setCursor(4, 3);
        lcd.print("Waiting :" + String(i) + "s ");
        delay(1000); // Wait for one second
      }

    // Mark that the countdown is done
      countdownDone = true;
    }

    // Check the input signal state after the 10-second countdown
    if (digitalRead(pin_Wall_switch) == LOW) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WALL SWITCH PRESENT");
      lcd.setCursor(1, 1);
      lcd.print("Push next button");
      lcd.setCursor(3, 2);
      lcd.print("if the test is");
      lcd.setCursor(0, 3);
      lcd.print("WALL SWITCH PRESENT");
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("WALL SWITCH MISSING");
      lcd.setCursor(1, 1);
      lcd.print("Push next button");
      lcd.setCursor(3, 2);
      lcd.print("if the test is");
      lcd.setCursor(0, 3);
      lcd.print("WALL SWITCH PRESENT");
      
    }
  }
}
//=====================================================================================================================================================




//=====================================================================================================================================================
// Sequence 3: Checks the signal received when the machine starts by pressing the START button
//=====================================================================================================================================================
void sequence_START_SCANNER() {
  static bool countdownDone = false; // Variable to track if the countdown is done
  bool signalReceived = false;
  bool buttonPressed = false;

  // Check if the countdown is not already done
  if (!countdownDone) {
    // Display the initial message on the LCD screen
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Third test: check");
    lcd.setCursor(2, 1);
    lcd.print(" if the SCANNER");
    lcd.setCursor(5, 2);
    lcd.print("STARTS UP");

    // 10-second countdown
    for (int i = 10; i > 0; i--) {
      lcd.setCursor(4, 3);
      lcd.print("Waiting :" + String(i) + "s ");
      delay(1000); // Wait for one second
    }

    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("IF FORCE:");
    lcd.setCursor(1, 1);
    lcd.print("Push next button");
    lcd.setCursor(0, 2);
    lcd.print("If X.CITE|CEED|GO :");
    lcd.setCursor(0, 3);
    lcd.print("Push little button");

    // Marking that the countdown is complete
    countdownDone = true;
  }

  // Loop to remain in this sequence until a condition is met
  while (!signalReceived && !buttonPressed) {
    // Check if a LOW signal is detected
    if (digitalRead(pin_Start) == LOW) {
      // Clear the LCD screen
      lcd.clear();
      // Display a message indicating that the signal has been received
      lcd.setCursor(2, 0);
      lcd.print("GANTRY SHOULD BE");
      lcd.setCursor(6, 1);
      lcd.print("ENERGIZED");
      lcd.setCursor(0, 2);
      lcd.print("Push next button if");
      lcd.setCursor(0, 3);
      lcd.print("Gantry is ENERGIZED");

      // Wait for the button press to move to the next sequence or the next step
      while (digitalRead(button_next_sequence) == HIGH) {
        // Wait
      }
        // Wait for a short moment to avoid bouncing
      delay(1000);
        // Wait for the button to be released
      while (digitalRead(button_next_sequence) == LOW) {
        // Wait
      }

      signalReceived = true; // Mark that the signal has been received
    }

    // Check if the next button is pressed
    if (digitalRead(button_next_sequence) == LOW) {
      // Clear the LCD screen
      lcd.clear();
      // Display a message indicating that the button has been pressed
      lcd.setCursor(3, 0);
      lcd.print("Test force:");
      lcd.setCursor(2, 1);
      lcd.print("Push the GREEN");
      lcd.setCursor(5, 2);
      lcd.print("button on");
      lcd.setCursor(0, 3);
      lcd.print("electrical cabinet");

      delay(10000);

      // Display another message
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Did the Force");
      lcd.setCursor(2, 1);
      lcd.print("system start up");
      lcd.setCursor(0, 2);
      lcd.print("without any issues?");
      lcd.setCursor(1, 3);
      lcd.print("If YES: push next");

      // Wait for the button press to move to the next sequence or the next step
      while (digitalRead(button_next_sequence) == HIGH) {
        // Wait
      }
        // Wait for a short moment to avoid bouncing
      delay(1000);
        // Wait for the button to be released
      while (digitalRead(button_next_sequence) == LOW) {
        // Wait
      }
      buttonPressed = true; // Mark that the button has been pressed
    }
  }

  // Once one of the conditions is met, proceed to the next sequence
  currentSequence = SEQUENCE_4;
}
//=====================================================================================================================================================



//=====================================================================================================================================================
// Sequence 4: Checks the signal received when the machine sends a scanner shutdown request after pressing the stop button
//=====================================================================================================================================================
void sequence_SHUTDOWN_REQUEST() {
  static bool countdownDone = false; // Variable to track if the countdown is done
  static bool signalReceived = false; // Variable to track if a signal is received

  // Check if the time elapsed since the last check exceeds the defined interval
  if (millis() - lastSignalCheckTime >= SIGNAL_CHECK_INTERVAL) {
    // Update the time of the last check
    lastSignalCheckTime = millis();
    lcd.clear();

    // If the countdown is not yet done
    if (!countdownDone) {
      // Display initial message on the LCD screen
      lcd.setCursor(4, 0);
      lcd.print("Fourth test :");
      lcd.setCursor(6, 1);
      lcd.print("Check the");
      lcd.setCursor(2, 2);
      lcd.print("SHUTDOWN REQUEST");

      // 10-second countdown
      for (int i = 10; i > 0; i--) {
        lcd.setCursor(4, 3);
        lcd.print("Waiting :" + String(i) + "s ");
        delay(1000); // Wait for one second
      }

      // Mark that the countdown is complete
      countdownDone = true;
    }
    // Display message before checking button state
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Push the RED BUTTON");
      lcd.setCursor(3, 1);
      lcd.print("on electrical");
      lcd.setCursor(6, 2);
      lcd.print("cabinet");

    // Check if the next button is pressed
    if (digitalRead(button_next_sequence) == LOW) {
      // Wait for the button to be released
      while (digitalRead(button_next_sequence) == LOW) {
        // Wait
      }
      // Proceed to the next sequence
      currentSequence = SEQUENCE_5;
    }

    // If a signal is received
    if (digitalRead(pin_Shutdown_request) == HIGH) {
      // Display message when the received signal is LOW
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SHUTDOWN REQUEST OK");
      lcd.setCursor(2, 1);
      lcd.print("If the test is :");
      lcd.setCursor(0, 2);
      lcd.print("Shutdown request OK");
      lcd.setCursor(0, 3);
      lcd.print("=> Push next button");
      // Wait for the button press to move to the next sequence or the next step
      while (digitalRead(button_next_sequence) == HIGH) {
        // Wait
      }
        // Wait for a short moment to avoid bouncing
      delay(1000);
        // Wait for the button to be released
      while (digitalRead(button_next_sequence) == LOW) {
        // Wait
      }
      currentSequence = SEQUENCE_5;
    }
  }
}
//=====================================================================================================================================================



//=====================================================================================================================================================
// Sequence 5: Controls the output to the relay
//=====================================================================================================================================================
void sequence_SHUTDOWN_COMMAND() {
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("The system will");
  lcd.setCursor(3, 1);
  lcd.print("shut down in :");

  // Countdown of 20 seconds
  for (int i = 20; i > 0; i--) {
    lcd.setCursor(9, 2);
    lcd.print(String(i) + "s ");
    delay(1000); // Wait for one second
  }

  bool signalReceived = false;

  // Wait for the HIGH signal reception on pin_Start
  while (!signalReceived) {
    digitalWrite(out_pin_Shutdown_command, HIGH); //  Set the output pin to HIGH state
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Did the system ");
    lcd.setCursor(1, 1);
    lcd.print("shut down without");
    lcd.setCursor(4, 2);
    lcd.print("any issues ?");
    lcd.setCursor(0, 3);
    lcd.print("If YES : push next!");

    // Wait for the button press to move to the next sequence or the next step
    while (digitalRead(button_next_sequence) == HIGH) {
        // Wait
    }
    // Wait for a short moment to avoid bouncing
    delay(1000);
    // Wait for the button to be released
    while (digitalRead(button_next_sequence) == LOW) {
        // Wait
    }
    // Signal received and button pressed, move to the next sequence or the next step
    signalReceived = true;
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("SHUTDOWN COMMAND OK");
    lcd.setCursor(1, 1);
    lcd.print("If the test is :");
    lcd.setCursor(0, 2);
    lcd.print("Shutdown command OK");
    lcd.setCursor(0, 3);
    lcd.print("=> Push next button");

    
    // Send a LOW signal on the same pin
    digitalWrite(out_pin_Shutdown_command, LOW);

    // Wait for the button press to move to the next sequence or the next step
    while (digitalRead(button_next_sequence) == HIGH) {
        // Wait
    }
    // Wait for a short moment to avoid bouncing
    delay(1000);
    // Wait for the button to be released
    while (digitalRead(button_next_sequence) == LOW) {
        // Wait
    }

    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("FINAL TEST:");
    lcd.setCursor(0, 1);
    lcd.print("Check the indicator");
    lcd.setCursor(0, 2);
    lcd.print("lights for X_RAY ON");
    lcd.setCursor(2, 3);
    lcd.print("and X_RAY READY");

    delay(5000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("If the X-RAY tests");
    lcd.setCursor(2, 1);
    lcd.print("are completed :");
    lcd.setCursor(0, 2);
    lcd.print("Press next button &");
    lcd.setCursor(0, 3);
    lcd.print("TURN OFF the casing");
  
    // Wait for the button press to move to the next sequence or the next step
    while (digitalRead(button_next_sequence) == HIGH) {
        // Wait
    }
    // Wait for a short moment to avoid bouncing
    delay(1000);
    // Wait for the button to be released
    while (digitalRead(button_next_sequence) == LOW) {
        // Wait
    }
    // Signal received and button pressed, move to the next sequence or the next step
    currentSequence = SEQUENCE_1;
  }
}
//=====================================================================================================================================================
