
## **5. schematics/README.md Content**
```markdown
# Circuit Schematics

This directory contains the circuit diagrams for the Smart Home Dashboard project.

## Files

- `wiring-diagram.fzz` - Fritzing file for editing
- `schematic.png` - Schematic diagram
- `breadboard-view.png` - Breadboard layout
- `pcb-layout.png` - PCB layout (optional)

## Circuit Description

### Power Supply
- ESP8266 powered via USB or external 5V
- Relays require separate 5V power supply
- DHT11 and LDR powered from ESP8266 3.3V

### Inputs
1. **DHT11** (Digital Temperature/Humidity)
   - VCC → 3.3V
   - GND → GND
   - DATA → D1 (GPIO5) with 10K pull-up resistor

2. **LDR** (Light Dependent Resistor)
   - One end → 3.3V
   - Other end → D2 (GPIO4) with 10K pull-down to GND

### Outputs
1. **Relay Module 1**
   - VCC → 5V
   - GND → GND
   - IN → D5 (GPIO14)

2. **Relay Module 2**
   - VCC → 5V
   - GND → GND
   - IN → D6 (GPIO12)

3. **LED Indicator**
   - Anode (+) → D3 (GPIO0) through 330Ω resistor
   - Cathode (-) → GND

## Safety Notes
- Use appropriate relays for your load (5A for lights, 10A for fans)
- Keep high voltage away from low voltage circuits
- Use proper insulation
- Don't exceed ESP8266 current limits (12mA per pin, 200mA total)