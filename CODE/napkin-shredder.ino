
/*
 * NAPKIN SHREDDER 
 * uploaded code 12.02.2026
 */

#include <OneWire.h>
#include <DallasTemperature.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define R1 25
#define R2 2
#define R3 4
#define R4 16
#define R5 17
#define R6 26

#define R7 18    // Heater
#define R8 19   // Mixer

#define L1 13
#define L2 12
#define L3 27
#define L4 14
#define L5 5
#define L6 15

#define buzz 23
#define ENC_CLK 32  //y
#define ENC_DT  33 //b
#define ENC_SW  34 //o
#define ONE_WIRE_BUS 35

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float currentTemp = -127.0;
unsigned long tempTimer = 0;


bool LSW1=false;bool LSW2=false;bool LSW3=false;
bool LSW4=false;bool LSW5=false;bool LSW6=false;
bool top=false;
bool mid=false;
bool bot=false;

unsigned long HEATTime = 60000; 
unsigned long MIXTime  = 60000;

bool systemRunning = false;

unsigned long stepTimer = 0;
int processStep = 0;
int heatMinutes = 1;
int mixMinutes  = 1;

bool selectHeat = true;   

int lastCLK;
bool btnLast = HIGH;

unsigned long btnPressTime = 0;
bool longPressDone = false;

unsigned long lcdTimer = 0;
unsigned long l2LowTimer = 0;
bool l2ConfirmedLow = false;

struct LimitConfirm {
  unsigned long timer = 0;
  bool armed = false;
};
LimitConfirm lcL1, lcL2, lcL3, lcL4, lcL5, lcL6;
enum HomeState {
  HOME_IDLE,
  HOME_TOP,
  HOME_MID,
  HOME_BOTTOM,
  HOME_DONE
};

HomeState homeState = HOME_IDLE;
bool homingActive = false;

bool confirmLimitLOW(uint8_t pin, LimitConfirm &lc, unsigned long confirmMs = 100)
{
  if (digitalRead(pin) == LOW)
  {
    if (!lc.armed)
    {
      lc.timer = millis();
      lc.armed = true;
    }
    if (millis() - lc.timer >= confirmMs)
    {
      return true;
    }
  }
  else
  {
    lc.armed = false;   
  }
  return false;
}

void HomeTask()
{
  if (!homingActive) return;

  switch (homeState)
  {
   case HOME_TOP:
  if (confirmLimitLOW(L1, lcL1))
  {
    StopALL();
    homeState = HOME_MID;
    Serial.println("Top Home OK");
  }
  else
  {
    digitalWrite(R1, HIGH);
    digitalWrite(R2, LOW);
  }
  break;

  case HOME_MID:
  if (confirmLimitLOW(L3, lcL3))
  {
    StopALL();
    homeState = HOME_BOTTOM;
    Serial.println("mid Home OK");
  }
  else
  {
    digitalWrite(R3, HIGH);
    digitalWrite(R4, LOW);
  }
  break;

  case HOME_BOTTOM:
  if (confirmLimitLOW(L6, lcL6))
  {
    StopALL();
    homeState = HOME_DONE;
    Serial.println("bootom Home OK");
  }
  else
  {
    digitalWrite(R5, HIGH);
    digitalWrite(R6, LOW);
  }
  break;

    case HOME_DONE:
      StopALL();
      homingActive = false;
      homeState = HOME_IDLE;

      digitalWrite(buzz, HIGH);
      delay(4000);
      digitalWrite(buzz, LOW);

      Serial.println("HOME POSITION REACHED");
      break;

    default:
      break;
  }
}


void StopALL()
{
  digitalWrite(R1, LOW);digitalWrite(R2, LOW);digitalWrite(R3, LOW);digitalWrite(R4, LOW);
  digitalWrite(R5, LOW);digitalWrite(R6, LOW);digitalWrite(R7, LOW);digitalWrite(R8, LOW);
}
void controlRelay(int relay, bool state)
{
  digitalWrite(relay, state ? HIGH : LOW);
}
/*void processSerial()
{
  if (Serial.available())
  {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    cmd.toUpperCase();

    if (cmd == "ALL OFF")
    {
      StopALL();
      Serial.println("ALL RELAYS OFF");
      return;
    }
    if (cmd == "START")
    {
      if (!systemRunning)
      {
        systemRunning = true;
        processStep = 0;
        Serial.println("START COMMAND ACCEPTED");
      }
      return;
    }

    bool state = false;

    if (cmd.endsWith("ON"))  state = true;
    if (cmd.endsWith("OFF")) state = false;

    if (cmd.startsWith("R1")) controlRelay(R1, state);
    else if (cmd.startsWith("R2")) controlRelay(R2, state);
    else if (cmd.startsWith("R3")) controlRelay(R3, state);
    else if (cmd.startsWith("R4")) controlRelay(R4, state);
    else if (cmd.startsWith("R5")) controlRelay(R5, state);
    else if (cmd.startsWith("R6")) controlRelay(R6, state);
    else if (cmd.startsWith("R7")) controlRelay(R7, state); // Heater
    else if (cmd.startsWith("R8")) controlRelay(R8, state); // Mixer
    else
    {
      Serial.println("INVALID COMMAND");
      return;
    }

    Serial.println(cmd + " OK");
  }
}*/


void StartProcessNonBlocking()
{
  if (!systemRunning) return;

  switch (processStep)
  {

    case 0:
    top = mid = bot = false;
    StopALL(); 
    digitalWrite(R7, HIGH);
    stepTimer = millis();
    lcdRunningScreen("HEAT", HEATTime);
    processStep = 1;
    break;


    case 1:
    if (millis() - lcdTimer >= 1000)
    {
      lcdTimer = millis();
      lcdRunningScreen("HEATER", HEATTime);
    }
  
    if (millis() - stepTimer >= HEATTime)
    {
      digitalWrite(R7, LOW);
      processStep = 2;
    }
    break;

     case 2:
    if (confirmLimitLOW(L2, lcL2))
    {
      digitalWrite(R2, LOW);
      Serial.println("Already at L2 ");
      processStep = 3;
      break;
    }
  
    digitalWrite(R2, HIGH);
    break;

    case 3:
      StopALL();              
      digitalWrite(R8, HIGH);
      stepTimer = millis();
      lcdRunningScreen("SHREDDER", MIXTime);
      processStep = 4;
      break;
 
    case 4:
      if (millis() - lcdTimer >= 1000)
      {
        lcdTimer = millis();
        lcdRunningScreen("SHREDDER", MIXTime);
      }
    
      if (millis() - stepTimer >= MIXTime)
      {
        digitalWrite(R8, LOW);
        processStep = 5;
      }
      break;

    case 5:
  if (confirmLimitLOW(L5, lcL5))
  {
    digitalWrite(R6, LOW);
    Serial.println("Already at L5");
    processStep = 6;
    break;
  }

  digitalWrite(R6, HIGH);
  break;

  case 6:
  if (confirmLimitLOW(L4, lcL4))
  {
    digitalWrite(R4, LOW);
    Serial.println("Already at L4");
    stepTimer = millis();
    processStep = 7;
    break;
  }

  digitalWrite(R4, HIGH);
  break;

    case 7:
      if (millis() - stepTimer >= 5000)
      {
        processStep = 8;
      }
      break;

   
      case 8:
      lcd.clear();
      lcd.print("PROCESS DONE");
      homingActive = true;
      homeState = HOME_TOP;

      systemRunning = false;
      processStep = 0;
      lcdIdleScreen();
      break;

  }
}
void lcdIdleScreen()
{
  lcd.setCursor(0, 0);
  lcd.print("HEATER  :");

  if (heatMinutes < 10) lcd.print("0");
  lcd.print(heatMinutes);
  lcd.print("  M");

  lcd.setCursor(15, 0);
  lcd.print(selectHeat ? 'H' : 'S');

  lcd.setCursor(0, 1);
  lcd.print("SHREDDER:");

  if (mixMinutes < 10) lcd.print("0");
  lcd.print(mixMinutes);
  lcd.print("  M");
}

void lcdRunningScreen(const char* mode, unsigned long totalTime)
{
  unsigned long elapsed = millis() - stepTimer;
  unsigned long remain  = (totalTime > elapsed) ? totalTime - elapsed : 0;

  lcd.setCursor(0, 0);
  lcd.print(mode);
  lcd.print(" ON         ");

  lcd.setCursor(0, 1);
  lcd.print("REMAIN:");

  if (remain >= 60000)   
  {
    int remainMin = (remain + 59999) / 60000;  
    lcd.print(remainMin);
    lcd.print(" M   ");
  }
  else                 
  {
    int remainSec = (remain + 999) / 1000;     
    lcd.print(remainSec);
    lcd.print(" S   ");
  }
}

void handleEncoder()
{
  int clkState = digitalRead(ENC_CLK);

  if (clkState != lastCLK)
  {
    if (digitalRead(ENC_DT) != clkState)
    {
      if (selectHeat) heatMinutes++;
      else mixMinutes++;
    }
    else
    {
      if (selectHeat && heatMinutes > 1) heatMinutes--;
      if (!selectHeat && mixMinutes > 1) mixMinutes--;
    }

    HEATTime = heatMinutes * 60000UL;
    MIXTime  = mixMinutes  * 60000UL;

    lcdIdleScreen();
  }
  lastCLK = clkState;

  bool btnState = digitalRead(ENC_SW);

  // button pressed
  if (btnState == LOW && btnLast == HIGH)
  {
    btnPressTime = millis();
    longPressDone = false;
  }

  // long press → START
  if (btnState == LOW && !longPressDone)
  {
    if (millis() - btnPressTime >= 2000)
    {
      if (!systemRunning)
      {
        systemRunning = true;
        processStep = 0;
        lcd.clear();
      }
      longPressDone = true;
    }
  }

  // short press → toggle selection
  if (btnState == HIGH && btnLast == LOW)
  {
    if (!longPressDone)
    {
      selectHeat = !selectHeat;
      lcdIdleScreen();
    }
  }

  btnLast = btnState;
}
void startHomingSafely()
{
  StopALL();   
  delay(200);

  bool atTop    = (digitalRead(L1) == LOW);
  bool atMid    = (digitalRead(L3) == LOW);
  bool atBottom = (digitalRead(L6) == LOW);

  if (atTop && atMid && atBottom)
  {
    Serial.println("Already at HOME – no motion");
    homingActive = false;
    homeState = HOME_IDLE;
    digitalWrite(buzz, HIGH);
    delay(4000);
    digitalWrite(buzz, LOW);
    return;
  }

  homingActive = true;

  if (!atTop)
    homeState = HOME_TOP;
  else if (!atMid)
    homeState = HOME_MID;
  else if (!atBottom)
    homeState = HOME_BOTTOM;
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin(115200);
  pinMode(R1, OUTPUT);pinMode(R2, OUTPUT);pinMode(R3, OUTPUT);pinMode(R4, OUTPUT);
  pinMode(R5, OUTPUT);pinMode(R6, OUTPUT);pinMode(R7, OUTPUT);pinMode(R8, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);

  pinMode(L1, INPUT_PULLUP);pinMode(L2, INPUT_PULLUP);pinMode(L3, INPUT_PULLUP);
  pinMode(L4, INPUT_PULLUP);pinMode(L5, INPUT_PULLUP);pinMode(L6, INPUT_PULLUP);

  StopALL();
  sensors.begin();

  Wire.begin(21, 22);

  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     NAPKIN     ");
  lcd.setCursor(0, 1);
  lcd.print("    SHREDDER    ");
  delay(2000);
  lcd.clear();
  
  lastCLK = digitalRead(ENC_CLK);
  lcdIdleScreen();
  startHomingSafely();


}
void TemperatureTask()
{
  if (millis() - tempTimer < 1000) return;  // 1 sec update
  tempTimer = millis();

  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);

  if (t > -55 && t < 125)   // valid range
  {
    currentTemp = t;
  }

  // ---- LCD overlay (safe zone) ----
  lcd.setCursor(10, 0);    // DOES NOT clash with your UI
  lcd.print("    ");       // clear old
  lcd.setCursor(10, 0);
  lcd.print((int)currentTemp);
  lcd.print("C");
}


void loop() {
  // put your main code here, to run repeatedly:
  //LSW1=digitalRead(L1);LSW2=digitalRead(L2);LSW3=digitalRead(L3);
  //LSW4=digitalRead(L4);LSW5=digitalRead(L5);LSW6=digitalRead(L6);
   
  /*Serial.print("LSW1:");Serial.print(LSW1);
  Serial.print("| LSW2:");Serial.print(LSW2);
  Serial.print("| LSW3:");Serial.print(LSW3);
  Serial.print("| LSW4:");Serial.print(LSW4);
  Serial.print("| LSW5:");Serial.print(LSW5);
  Serial.print("| LSW6:");Serial.println(LSW6);*/

  //processSerial();
  handleEncoder();
  StartProcessNonBlocking();
  HomeTask(); 
  TemperatureTask();

  
 
}
