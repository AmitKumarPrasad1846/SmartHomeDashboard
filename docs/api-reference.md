# API Reference

## Overview
The ESP8266 Smart Home Dashboard exposes a simple REST API for controlling devices and reading sensor data.

## Base URL
`http://[ESP_IP_ADDRESS]`

## Endpoints

### GET /
Returns the main dashboard HTML page.

**Response:** HTML content

### GET /data
Returns JSON data with all current sensor readings and device states.

**Response:**
```json
{
  "t": 25.5,
  "h": 60.2,
  "dark": true,
  "led": true,
  "r1": false,
  "r2": true
}