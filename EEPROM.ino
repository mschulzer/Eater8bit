// What pins are we using on the Arduino Nano
#define SHIFT_DAT 2
#define SHIFT_CLK 3
#define SHIFT_LAT 4
#define EEPROM_D0 5
#define EEPROM_D7 12
#define WRITE_EN 13

// HELPER:- setting the initial address
void setAddress (int address, bool outputEnable) {
  // Process: put a bit on SHIFT_DATA -- pulse SHIFT_CLK; doing it twice for 16 bits
  shiftOut(SHIFT_DAT, SHIFT_CLK, MSBFIRST, (address >> 8) | (outputEnable ? 0x00 : 0x80));
  shiftOut(SHIFT_DAT, SHIFT_CLK, MSBFIRST, address);

  // Toggle storage register (storage clock pulse); just pulses it high
  digitalWrite(SHIFT_LAT, LOW);
  digitalWrite(SHIFT_LAT, HIGH);
  digitalWrite(SHIFT_LAT, LOW);
}


// HELPER:- Reading the EEPROM
byte readEEPROM (int address) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, INPUT); 
  }

  // Set address and data
  setAddress(address, true);
  byte data = 0;
  
  // Read each pin and shift into byte of data
  for (int pin = EEPROM_D7; pin >= EEPROM_D0; pin -= 1) {
    data = (data << 1) + digitalRead(pin);
  }
  return data;
}

// HELPER:- Writing to the EEPROM
void writeEEPROM (int address, byte data) {
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    pinMode(pin, OUTPUT); 
  }
  
  // Set address and data
  setAddress (address, false);
  
  for (int pin = EEPROM_D0; pin <= EEPROM_D7; pin += 1) {
    digitalWrite(pin, data & 1);
    data = data >> 1;
  }

  // Pulse write-enable
  digitalWrite(WRITE_EN, LOW);
  delayMicroseconds(1);
  digitalWrite(WRITE_EN, HIGH);
  delay(10);
}

// HELPER:- Dump the contents of the EEPROM
void dump(const uint16_t address) {
  char text[80];
  for (int i = 0; i < 256; i += 16) {
    byte data[16];
    for (int offset = 0; offset < 16; offset++) {
      data[offset] = readEEPROM(address + i + offset);
    }
    sprintf(text, "%04x   %02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x", address + i, 
                  data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], 
                  data[8], data[9], data[10], data[11], data[12], data[13], data[14], data[15]);
    Serial.println(text);
  }
}

void setup () {
  
  // Configure relevant pins for output
  pinMode(SHIFT_DAT, OUTPUT);
  pinMode(SHIFT_CLK, OUTPUT);
  pinMode(SHIFT_LAT, OUTPUT);

  //digitalWrite(WRITE_EN, HIGH);   // Just sets pullup-resistor
  //pinMode(WRITE_EN, OUTPUT);      // .. so it will already be high
  //writeEEPROM(1, 0x55);           // hex: 37 ==> bin: 00110111 ==> dec: 55

  Serial.begin(57600);
  dump(0);
}

void loop () {}
