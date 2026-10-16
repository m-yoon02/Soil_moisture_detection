// Soil Moisture Detector with Beeper — Arduino Uno
// Beeps every 30 seconds while the soil is dry.

const int SENSOR_PIN   = A0;
const int SENSOR_POWER = 7;   // sensor only gets power while reading
const int BUZZER = 8;
const int RED = 4, GREEN = 5; // optional LEDs

// ---- Calibrate these for your sensor ----
const int DRY_RAW = 0;   // reading in dry air
const int WET_RAW = 876;   // reading in a glass of water

const int DRY_PERCENT = 30;  // below this -> needs water
const int OK_PERCENT  = 40;  // must rise above this to stop beeping

const unsigned long READ_INTERVAL = 5000;   // check soil every 5 s
const unsigned long BEEP_INTERVAL = 30000;  // remind every 30 s while dry

bool isDry = false;
unsigned long lastRead = 0;
unsigned long lastBeep = 0;

int readRaw() {
  digitalWrite(SENSOR_POWER, HIGH);
  delay(100);                       // let the sensor settle
  long sum = 0;
  for (int i = 0; i < 10; i++) {    // average 10 readings
    sum += analogRead(SENSOR_PIN);
    delay(5);
  }
  digitalWrite(SENSOR_POWER, LOW);
  return sum / 10;
}

int toPercent(int raw) {
  return constrain(map(raw, DRY_RAW, WET_RAW, 0, 100), 0, 100);
}

void beepAlert() {
  tone(BUZZER, 1000, 150);
  delay(250);
  tone(BUZZER, 1000, 150);
}

void checkSoil() {
  int raw = readRaw();
  int pct = toPercent(raw);

  Serial.print("raw=");
  Serial.print(raw);
  Serial.print("  moisture=");
  Serial.print(pct);
  Serial.println("%");

  if (!isDry && pct < DRY_PERCENT) {
    isDry = true;
    Serial.println("Soil is dry, time to water!");
    beepAlert();
    lastBeep = millis();
  } else if (isDry && pct > OK_PERCENT) {
    isDry = false;
    Serial.println("Soil is moist again.");
  }

  digitalWrite(RED, isDry);
  digitalWrite(GREEN, !isDry);
}

void setup() {
  Serial.begin(9600);
  pinMode(SENSOR_POWER, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(SENSOR_POWER, LOW);

  Serial.println("Plant monitor ready");
  checkSoil();
}

void loop() {
  if (millis() - lastRead >= READ_INTERVAL) {
    lastRead = millis();
    checkSoil();
  }

  // Keep reminding while the soil stays dry
  if (isDry && millis() - lastBeep >= BEEP_INTERVAL) {
    lastBeep = millis();
    beepAlert();
  }
}
