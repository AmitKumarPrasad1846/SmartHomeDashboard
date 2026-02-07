
## **4. docs/troubleshooting.md Content**
```markdown
# Troubleshooting Guide

## Common Issues and Solutions

### 1. WiFi Connection Issues
**Symptoms:**
- "Connecting to WiFi..." message repeats
- ESP8266 doesn't get IP address
- Serial monitor shows "WiFi Failed!"

**Solutions:**
1. Verify SSID and password in the code
2. Check router settings (WPA2 recommended)
3. Ensure ESP8266 is within range
4. Try restarting both ESP8266 and router
5. Check if router has MAC filtering enabled

### 2. Web Page Not Loading
**Symptoms:**
- Can't connect to ESP8266 IP
- Browser shows connection error
- Page loads but doesn't update

**Solutions:**
1. Verify IP address in serial monitor
2. Check if on same network as ESP8266
3. Disable firewall or antivirus temporarily
4. Try different browser (Chrome/Firefox recommended)
5. Clear browser cache

### 3. Sensor Readings Incorrect
**Symptoms:**
- Temperature/Humidity shows -- or NaN
- Wrong temperature values
- No light detection changes

**Solutions:**
1. Check DHT11 wiring (VCC, GND, DATA)
2. Ensure pull-up resistor for DHT11 (10K between VCC and DATA)
3. Verify LDR connection with 10K pull-down resistor
4. Check if DHT11 library is installed
5. Try different DHT11 sensor (they can fail)

### 4. Relays Not Working
**Symptoms:**
- Relays don't click when toggled
- No LED indicator on relay module
- Devices connected to relays don't turn on

**Solutions:**
1. Check power supply to relay module (5V required)
2. Verify relay logic (LOW/HIGH based on module)
3. Check wiring from ESP8266 to relay IN pins
4. Test with simple LED to verify relay output
5. Ensure relay module is not defective

### 5. Serial Monitor Issues
**Symptoms:**
- Gibberish characters
- No output
- Messages repeating too frequently

**Solutions:**
1. Set baud rate to 115200
2. Check USB cable (some only charge, don't transfer data)
3. Select correct COM port in Arduino IDE
4. Add `Serial.println()` at beginning of setup to test
5. Try different USB port

### 6. LED Control Issues
**Symptoms:**
- LED doesn't respond to light changes
- Manual override doesn't work
- LED stuck on or off

**Solutions:**
1. Check LED polarity (long leg is anode/+)
2. Verify resistor value (220-330 ohms recommended)
3. Test LDR readings in serial monitor
4. Check `manualOverride` variable logic
5. Verify pinMode is set correctly

## Debugging Steps

### 1. Basic ESP8266 Test
```cpp
void setup() {
  Serial.begin(115200);
  Serial.println("ESP8266 Test");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  delay(1000);
  Serial.println("Blink");
}