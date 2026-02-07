// =============================================
// SMART HOME DASHBOARD
// =============================================

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// ============ CONFIGURATION ============
// WiFi Configuration
#define WIFI_SSID "Airtel_sanj_4842"  // CHANGE THIS
#define WIFI_PASSWORD "Air@15439"     // CHANGE THIS

// Pin Configuration
#define PIN_LDR D2     // GPIO4
#define PIN_LED D3     // GPIO0
#define PIN_RELAY1 D5  // GPIO14
#define PIN_RELAY2 D6  // GPIO12
#define PIN_DHT D1     // GPIO5

// DHT Configuration
#define DHT_TYPE DHT11

// Relay Logic (Change if needed)
#define RELAY_ON LOW  // LOW = Relay ON for most modules
#define RELAY_OFF HIGH

// ============ GLOBAL VARIABLES ============
ESP8266WebServer server(80);
DHT dht(PIN_DHT, DHT_TYPE);

float temperature = 0;
float humidity = 0;
bool isDark = false;
bool ledState = false;
bool relay1State = false;
bool relay2State = false;
bool manualOverride = false;  // Track if LED is manually controlled

// Tracking variables for state changes
bool lastDarkState = false;
bool lastLedState = false;
bool lastRelay1State = false;
bool lastRelay2State = false;

// ============ SUNSET GLASS THEME HTML PAGE ============
const char HTML_PAGE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Home Control Panel</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Segoe UI', system-ui, -apple-system, sans-serif;
        }

        :root {
            --primary: #f97316;
            --primary-light: #fb923c;
            --secondary: #ea580c;
            --success: #f59e0b;
            --danger: #dc2626;
            --warning: #f59e0b;
            --dark: #1c1917;
            --light: #fef3c7;
            --card-bg: rgba(254, 243, 199, 0.85);
            --glass-bg: rgba(255, 255, 255, 0.15);
            --shadow: 0 8px 32px rgba(0, 0, 0, 0.2);
            --transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
        }

        body {
            background: linear-gradient(135deg, #fbbf24 0%, #f59e0b 100%);
            min-height: 100vh;
            padding: 20px;
            color: var(--dark);
        }

        .container {
            max-width: 1400px;
            margin: 0 auto;
        }

        /* Header */
        .header {
            text-align: center;
            padding: 40px 20px;
            margin-bottom: 40px;
            background: var(--glass-bg);
            backdrop-filter: blur(20px);
            border-radius: 24px;
            border: 1px solid rgba(255, 255, 255, 0.3);
            box-shadow: var(--shadow);
        }

        .header h1 {
            font-size: 3.5rem;
            color: white;
            margin-bottom: 10px;
            text-shadow: 0 2px 10px rgba(0, 0, 0, 0.2);
        }

        .header p {
            color: rgba(255, 255, 255, 0.9);
            font-size: 1.2rem;
        }

        .connection-status {
            display: inline-flex;
            align-items: center;
            gap: 10px;
            margin-top: 20px;
            padding: 12px 24px;
            background: rgba(255, 255, 255, 0.2);
            border-radius: 50px;
            color: white;
            font-weight: 500;
        }

        .status-dot {
            width: 12px;
            height: 12px;
            border-radius: 50%;
            background: #4ade80;
            box-shadow: 0 0 0 0 rgba(74, 222, 128, 0.7);
            animation: pulse-green 2s infinite;
        }

        @keyframes pulse-green {
            0% { box-shadow: 0 0 0 0 rgba(74, 222, 128, 0.7); }
            70% { box-shadow: 0 0 0 10px rgba(74, 222, 128, 0); }
            100% { box-shadow: 0 0 0 0 rgba(74, 222, 128, 0); }
        }

        /* Dashboard Grid */
        .dashboard-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(350px, 1fr));
            gap: 30px;
            margin-bottom: 50px;
        }

        /* Cards */
        .card {
            background: var(--card-bg);
            border-radius: 20px;
            padding: 30px;
            box-shadow: var(--shadow);
            transition: var(--transition);
            border: 1px solid rgba(255, 255, 255, 0.4);
            backdrop-filter: blur(10px);
            position: relative;
            overflow: hidden;
        }

        .card::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            height: 4px;
            background: linear-gradient(90deg, #f97316, #f59e0b);
        }

        .card:hover {
            transform: translateY(-10px);
            box-shadow: 0 20px 40px rgba(0, 0, 0, 0.25);
        }

        .card-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 25px;
        }

        .card-title {
            font-size: 1.5rem;
            font-weight: 600;
            color: var(--dark);
        }

        .card-icon {
            width: 60px;
            height: 60px;
            border-radius: 15px;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 28px;
            color: white;
            background: linear-gradient(135deg, #f97316, #fb923c);
        }

        /* Sensor Values */
        .sensor-display {
            margin: 25px 0;
            text-align: center;
        }

        .value {
            font-size: 4.5rem;
            font-weight: 800;
            line-height: 1;
            margin: 15px 0;
            background: linear-gradient(135deg, #dc2626, #f97316);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
        }

        .temp-value {
            background: linear-gradient(135deg, #dc2626, #f97316);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        .hum-value {
            background: linear-gradient(135deg, #0ea5e9, #0369a1);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        .value-label {
            font-size: 1rem;
            color: #7c2d12;
            text-transform: uppercase;
            letter-spacing: 1.5px;
            margin-bottom: 10px;
            font-weight: 600;
        }

        .status-indicator {
            display: inline-flex;
            align-items: center;
            gap: 10px;
            padding: 10px 20px;
            border-radius: 50px;
            font-weight: 600;
            font-size: 0.9rem;
            margin-top: 15px;
        }

        .status-on {
            background: rgba(245, 158, 11, 0.2);
            color: #d97706;
        }

        .status-off {
            background: rgba(220, 38, 38, 0.2);
            color: #b91c1c;
        }

        .status-dark {
            background: rgba(124, 45, 18, 0.2);
            color: #7c2d12;
        }

        .status-light {
            background: rgba(251, 191, 36, 0.3);
            color: #d97706;
        }

        /* Control Panel */
        .control-panel {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 25px;
            margin-top: 30px;
        }

        .device-card {
            background: var(--card-bg);
            border-radius: 20px;
            padding: 25px;
            text-align: center;
            box-shadow: var(--shadow);
            transition: var(--transition);
            border: 1px solid rgba(255, 255, 255, 0.4);
            backdrop-filter: blur(10px);
        }

        .device-icon {
            width: 70px;
            height: 70px;
            border-radius: 20px;
            display: flex;
            align-items: center;
            justify-content: center;
            font-size: 32px;
            margin: 0 auto 20px;
            background: linear-gradient(135deg, #f97316, #f59e0b);
            color: white;
        }

        .device-name {
            font-size: 1.3rem;
            font-weight: 600;
            margin-bottom: 15px;
            color: var(--dark);
        }

        .device-status {
            display: inline-block;
            padding: 8px 20px;
            border-radius: 50px;
            font-weight: 600;
            margin-bottom: 20px;
            background: rgba(255, 255, 255, 0.3);
        }

        .toggle-switch {
            position: relative;
            display: inline-block;
            width: 80px;
            height: 40px;
        }

        .toggle-switch input {
            opacity: 0;
            width: 0;
            height: 0;
        }

        .toggle-slider {
            position: absolute;
            cursor: pointer;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(90deg, #dc2626, #f97316);
            transition: var(--transition);
            border-radius: 34px;
        }

        .toggle-slider:before {
            position: absolute;
            content: "";
            height: 32px;
            width: 32px;
            left: 4px;
            bottom: 4px;
            background-color: white;
            transition: var(--transition);
            border-radius: 50%;
        }

        input:checked + .toggle-slider {
            background: linear-gradient(90deg, #0ea5e9, #0369a1);
        }

        input:checked + .toggle-slider:before {
            transform: translateX(40px);
        }

        .toggle-label {
            display: flex;
            align-items: center;
            justify-content: space-between;
            gap: 15px;
            margin-top: 15px;
        }

        .toggle-label span {
            color: #7c2d12;
            font-weight: 600;
        }

        /* Manual Override Indicator */
        .override-indicator {
            display: inline-flex;
            align-items: center;
            gap: 8px;
            padding: 6px 12px;
            border-radius: 15px;
            font-size: 0.8rem;
            background: rgba(124, 45, 18, 0.2);
            color: #7c2d12;
            margin-top: 10px;
        }

        /* Footer */
        .footer {
            text-align: center;
            padding: 30px;
            margin-top: 50px;
            background: var(--glass-bg);
            backdrop-filter: blur(20px);
            border-radius: 24px;
            border: 1px solid rgba(255, 255, 255, 0.3);
            color: white;
        }

        .update-info {
            font-size: 1.1rem;
            margin-bottom: 15px;
        }

        .update-time {
            font-family: 'Courier New', monospace;
            font-size: 1.4rem;
            font-weight: 600;
            background: rgba(255, 255, 255, 0.2);
            padding: 15px 30px;
            border-radius: 15px;
            display: inline-block;
            margin: 15px 0;
            color: white;
        }

        .ip-display {
            font-size: 0.9rem;
            opacity: 0.8;
            margin-top: 20px;
        }

        /* Responsive Design */
        @media (max-width: 1200px) {
            .control-panel {
                grid-template-columns: repeat(2, 1fr);
            }
        }

        @media (max-width: 768px) {
            .dashboard-grid {
                grid-template-columns: 1fr;
            }
            
            .control-panel {
                grid-template-columns: 1fr;
            }
            
            .header h1 {
                font-size: 2.5rem;
            }
            
            .value {
                font-size: 3.5rem;
            }
            
            .card, .device-card {
                padding: 20px;
            }
        }

        /* Animations */
        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(20px); }
            to { opacity: 1; transform: translateY(0); }
        }

        .card {
            animation: fadeIn 0.6s ease-out;
        }

        /* Loading Animation */
        .loading {
            display: inline-block;
            width: 20px;
            height: 20px;
            border: 3px solid rgba(255,255,255,.3);
            border-radius: 50%;
            border-top-color: var(--primary);
            animation: spin 1s ease-in-out infinite;
        }

        @keyframes spin {
            to { transform: rotate(360deg); }
        }

        /* Info Text */
        .info-text {
            margin-top: 15px;
            color: #7c2d12;
            font-size: 0.9rem;
            font-style: italic;
        }
    </style>
</head>
<body>
    <div class="container">
        <!-- Header -->
        <div class="header">
            <h1>🌅 Smart Home Dashboard</h1>
            <p>Warm Automation for Your Comfort</p>
            <div class="connection-status">
                <div class="status-dot"></div>
                <span>System Online • Connected</span>
            </div>
        </div>

        <!-- Dashboard Grid -->
        <div class="dashboard-grid">
            <!-- Temperature Card -->
            <div class="card">
                <div class="card-header">
                    <div class="card-title">Temperature</div>
                    <div class="card-icon">🌡️</div>
                </div>
                <div class="sensor-display">
                    <div class="value-label">Current Temperature</div>
                    <div class="value temp-value" id="temp">--°C</div>
                    <div class="status-indicator" id="tempStatus">
                        <span>Optimal</span>
                    </div>
                </div>
                <p class="info-text">Comfort range: 20°C - 25°C</p>
            </div>

            <!-- Humidity Card -->
            <div class="card">
                <div class="card-header">
                    <div class="card-title">Humidity</div>
                    <div class="card-icon">💧</div>
                </div>
                <div class="sensor-display">
                    <div class="value-label">Current Humidity</div>
                    <div class="value hum-value" id="hum">--%</div>
                    <div class="status-indicator" id="humStatus">
                        <span>Optimal</span>
                    </div>
                </div>
                <p class="info-text">Comfort range: 40% - 60%</p>
            </div>

            <!-- Light Sensor Card -->
            <div class="card">
                <div class="card-header">
                    <div class="card-title">Ambient Light</div>
                    <div class="card-icon">🔆</div>
                </div>
                <div class="sensor-display">
                    <div class="value-label">Light Level</div>
                    <div style="font-size: 5rem; margin: 20px 0;" id="lightIcon">🌙</div>
                    <div class="status-indicator" id="lightStatus">
                        <span>Checking...</span>
                    </div>
                    <div class="override-indicator" id="overrideIndicator" style="display: none;">
                        <i class="fas fa-user"></i> Manual Override Active
                    </div>
                </div>
                <p class="info-text">Auto-control: LED turns ON at night, OFF during day</p>
            </div>
        </div>

        <!-- Control Panel -->
        <div class="control-panel">
            <!-- Living Room Light -->
            <div class="device-card">
                <div class="device-icon">💡</div>
                <div class="device-name">Living Room Light</div>
                <div class="device-status" id="relay1Status">OFF</div>
                
                <div class="toggle-label">
                    <span>OFF</span>
                    <label class="toggle-switch">
                        <input type="checkbox" id="relay1Toggle" onchange="toggleDevice('relay1')">
                        <span class="toggle-slider"></span>
                    </label>
                    <span>ON</span>
                </div>
                <p class="info-text">Relay 1 • Main Lighting</p>
            </div>

            <!-- Bedroom Fan -->
            <div class="device-card">
                <div class="device-icon">🌀</div>
                <div class="device-name">Bedroom Fan</div>
                <div class="device-status" id="relay2Status">OFF</div>
                
                <div class="toggle-label">
                    <span>OFF</span>
                    <label class="toggle-switch">
                        <input type="checkbox" id="relay2Toggle" onchange="toggleDevice('relay2')">
                        <span class="toggle-slider"></span>
                    </label>
                    <span>ON</span>
                </div>
                <p class="info-text">Relay 2 • Fan Control</p>
            </div>

            <!-- Indicator LED -->
            <div class="device-card">
                <div class="device-icon">💎</div>
                <div class="device-name">Indicator LED</div>
                <div class="device-status" id="ledStatus">OFF</div>
                <div class="override-indicator" id="ledOverride" style="display: none;">
                    ⚡ Manual Control
                </div>
                
                <div class="toggle-label">
                    <span>OFF</span>
                    <label class="toggle-switch">
                        <input type="checkbox" id="ledToggle" onchange="toggleDevice('led')">
                        <span class="toggle-slider"></span>
                    </label>
                    <span>ON</span>
                </div>
                <div style="margin-top: 15px;">
                    <button class="btn-reset" onclick="resetAutoControl()" style="padding: 8px 15px; background: rgba(124, 45, 18, 0.2); color: #7c2d12; border: none; border-radius: 10px; cursor: pointer; font-size: 0.9rem;">
                        Reset to Auto-control
                    </button>
                </div>
                <p class="info-text">Toggle for manual override</p>
            </div>
        </div>

        <!-- Footer -->
        <div class="footer">
            <div class="update-info">
                🔄 Auto-refreshes every 3 seconds
            </div>
            <div class="update-time">
                Last Update: <span id="updateTime">--:--:--</span>
            </div>
            <div class="ip-display">
                Connected to: <span id="ipAddress">ESP8266</span> • System Online
            </div>
        </div>
    </div>

    <script>
        // Store IP address
        document.getElementById('ipAddress').textContent = window.location.hostname || 'ESP8266';
        
        // Track if LED is in manual mode
        let ledManualMode = false;
        
        // Auto-refresh every 3 seconds
        setInterval(updateDashboard, 3000);
        
        // Initial load
        updateDashboard();
        
        function updateDashboard() {
            fetch('/data')
                .then(response => {
                    if (!response.ok) throw new Error('Network error');
                    return response.json();
                })
                .then(data => {
                    // Update temperature with animation
                    const tempElement = document.getElementById('temp');
                    const tempStatus = document.getElementById('tempStatus');
                    tempElement.textContent = data.t + '°C';
                    
                    if (data.t > 30) {
                        tempStatus.innerHTML = '<span>🔥 Hot</span>';
                        tempStatus.className = 'status-indicator status-off';
                    } else if (data.t < 18) {
                        tempStatus.innerHTML = '<span>❄️ Cool</span>';
                        tempStatus.className = 'status-indicator status-on';
                    } else {
                        tempStatus.innerHTML = '<span>✅ Optimal</span>';
                        tempStatus.className = 'status-indicator status-on';
                    }
                    
                    // Update humidity
                    const humElement = document.getElementById('hum');
                    const humStatus = document.getElementById('humStatus');
                    humElement.textContent = data.h + '%';
                    
                    if (data.h > 70) {
                        humStatus.innerHTML = '<span>💦 High</span>';
                        humStatus.className = 'status-indicator status-off';
                    } else if (data.h < 30) {
                        humStatus.innerHTML = '<span>🏜️ Low</span>';
                        humStatus.className = 'status-indicator status-on';
                    } else {
                        humStatus.innerHTML = '<span>✅ Optimal</span>';
                        humStatus.className = 'status-indicator status-on';
                    }
                    
                    // Update light sensor
                    const lightIcon = document.getElementById('lightIcon');
                    const lightStatus = document.getElementById('lightStatus');
                    const overrideIndicator = document.getElementById('overrideIndicator');
                    
                    if (data.dark) {
                        lightIcon.textContent = '🌙';
                        lightStatus.innerHTML = '<span>🌙 Night Mode</span>';
                        lightStatus.className = 'status-indicator status-dark';
                        
                        // Show override indicator if LED is manually OFF during night
                        if (ledManualMode && !data.led) {
                            overrideIndicator.style.display = 'inline-flex';
                            lightStatus.innerHTML = '<span>🌙 Night (LED Override OFF)</span>';
                        } else {
                            overrideIndicator.style.display = 'none';
                        }
                    } else {
                        lightIcon.textContent = '☀️';
                        lightStatus.innerHTML = '<span>☀️ Day Mode</span>';
                        lightStatus.className = 'status-indicator status-light';
                        
                        // Show override indicator if LED is manually ON during day
                        if (ledManualMode && data.led) {
                            overrideIndicator.style.display = 'inline-flex';
                            lightStatus.innerHTML = '<span>☀️ Day (LED Override ON)</span>';
                        } else {
                            overrideIndicator.style.display = 'none';
                        }
                    }
                    
                    // Update relay 1
                    const relay1Status = document.getElementById('relay1Status');
                    const relay1Toggle = document.getElementById('relay1Toggle');
                    relay1Status.textContent = data.r1 ? 'ON' : 'OFF';
                    relay1Status.style.background = data.r1 ? 'rgba(245, 158, 11, 0.3)' : 'rgba(220, 38, 38, 0.3)';
                    relay1Status.style.color = data.r1 ? '#d97706' : '#b91c1c';
                    relay1Toggle.checked = data.r1;
                    
                    // Update relay 2
                    const relay2Status = document.getElementById('relay2Status');
                    const relay2Toggle = document.getElementById('relay2Toggle');
                    relay2Status.textContent = data.r2 ? 'ON' : 'OFF';
                    relay2Status.style.background = data.r2 ? 'rgba(245, 158, 11, 0.3)' : 'rgba(220, 38, 38, 0.3)';
                    relay2Status.style.color = data.r2 ? '#d97706' : '#b91c1c';
                    relay2Toggle.checked = data.r2;
                    
                    // Update LED
                    const ledStatus = document.getElementById('ledStatus');
                    const ledToggle = document.getElementById('ledToggle');
                    const ledOverride = document.getElementById('ledOverride');
                    
                    ledStatus.textContent = data.led ? 'ON' : 'OFF';
                    ledStatus.style.background = data.led ? 'rgba(245, 158, 11, 0.3)' : 'rgba(220, 38, 38, 0.3)';
                    ledStatus.style.color = data.led ? '#d97706' : '#b91c1c';
                    ledToggle.checked = data.led;
                    
                    // Check if LED is in manual mode (not matching auto logic)
                    const shouldBeOn = data.dark; // LED should be ON when dark
                    const isManuallyControlled = (data.led !== shouldBeOn);
                    
                    if (isManuallyControlled) {
                        ledManualMode = true;
                        ledOverride.style.display = 'inline-flex';
                        ledStatus.textContent = data.led ? 'ON (Manual)' : 'OFF (Manual)';
                    } else {
                        ledManualMode = false;
                        ledOverride.style.display = 'none';
                    }
                    
                    // Update time
                    const now = new Date();
                    const timeString = 
                        now.getHours().toString().padStart(2, '0') + ':' +
                        now.getMinutes().toString().padStart(2, '0') + ':' +
                        now.getSeconds().toString().padStart(2, '0');
                    document.getElementById('updateTime').textContent = timeString;
                    
                })
                .catch(error => {
                    console.error('Update error:', error);
                    document.getElementById('updateTime').textContent = 'Connection Error';
                    document.getElementById('updateTime').style.color = '#dc2626';
                });
        }
        
        function toggleDevice(device) {
            const toggle = document.getElementById(device + 'Toggle');
            const action = toggle.checked ? 'on' : 'off';
            
            // Add visual feedback
            const statusElement = document.getElementById(device + 'Status');
            const originalText = statusElement.textContent;
            statusElement.textContent = '...';
            statusElement.style.opacity = '0.7';
            
            // If controlling LED, set manual mode
            if (device === 'led') {
                ledManualMode = true;
            }
            
            fetch('/control?device=' + device + '&action=' + action)
                .then(response => {
                    if (response.ok) {
                        // Update immediately for better UX
                        setTimeout(() => {
                            updateDashboard();
                            statusElement.style.opacity = '1';
                        }, 300);
                    } else {
                        // Revert toggle on error
                        toggle.checked = !toggle.checked;
                        statusElement.textContent = originalText;
                        statusElement.style.opacity = '1';
                    }
                })
                .catch(error => {
                    console.error('Control error:', error);
                    toggle.checked = !toggle.checked;
                    statusElement.textContent = originalText;
                    statusElement.style.opacity = '1';
                });
        }
        
        function resetAutoControl() {
            // Reset LED to auto-control mode
            fetch('/control?device=led&action=auto')
                .then(response => {
                    if (response.ok) {
                        ledManualMode = false;
                        updateDashboard();
                    }
                })
                .catch(error => {
                    console.error('Reset error:', error);
                });
        }
        
        // Add click sound effect
        function playClickSound() {
            // Simple click sound simulation
            const audioContext = new (window.AudioContext || window.webkitAudioContext)();
            const oscillator = audioContext.createOscillator();
            const gainNode = audioContext.createGain();
            
            oscillator.connect(gainNode);
            gainNode.connect(audioContext.destination);
            
            oscillator.frequency.value = 800;
            oscillator.type = 'sine';
            
            gainNode.gain.setValueAtTime(0.3, audioContext.currentTime);
            gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.1);
            
            oscillator.start(audioContext.currentTime);
            oscillator.stop(audioContext.currentTime + 0.1);
        }
        
        // Add click handlers to toggles
        document.querySelectorAll('.toggle-switch').forEach(toggle => {
            toggle.addEventListener('click', () => {
                playClickSound();
            });
        });
        
        // Add keyboard shortcuts
        document.addEventListener('keydown', function(e) {
            if (e.altKey) {
                switch(e.key) {
                    case '1':
                        document.getElementById('relay1Toggle').click();
                        break;
                    case '2':
                        document.getElementById('relay2Toggle').click();
                        break;
                    case '3':
                        document.getElementById('ledToggle').click();
                        break;
                    case 'r':
                        updateDashboard();
                        break;
                    case 'a':
                        resetAutoControl();
                        break;
                }
            }
        });
    </script>
</body>
</html>
)rawliteral";

// ============ SETUP FUNCTION ============
void setup() {
  Serial.begin(115200);
  Serial.println("\n🚀 Sunset Smart Home Dashboard Starting...");

  // Setup pins
  pinMode(PIN_LDR, INPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_RELAY1, OUTPUT);
  pinMode(PIN_RELAY2, OUTPUT);

  // Turn everything OFF initially
  digitalWrite(PIN_RELAY1, RELAY_OFF);
  digitalWrite(PIN_RELAY2, RELAY_OFF);
  digitalWrite(PIN_LED, LOW);

  // Initialize tracking variables
  lastDarkState = isDark;
  lastLedState = ledState;
  lastRelay1State = relay1State;
  lastRelay2State = relay2State;

  // Start DHT sensor
  dht.begin();
  delay(1000);

  // Connect to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ WiFi Connected!");
    Serial.print("📡 IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ WiFi Failed!");
    // Continue anyway for testing
  }

  // Setup web server routes
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", HTML_PAGE);
  });

  server.on("/data", HTTP_GET, []() {
    String json = "{";
    json += "\"t\":" + String(temperature, 1) + ",";
    json += "\"h\":" + String(humidity, 1) + ",";
    json += "\"dark\":" + String(isDark ? "true" : "false") + ",";
    json += "\"led\":" + String(ledState ? "true" : "false") + ",";
    json += "\"r1\":" + String(relay1State ? "true" : "false") + ",";
    json += "\"r2\":" + String(relay2State ? "true" : "false");
    json += "}";
    server.send(200, "application/json", json);
  });

  server.on("/control", HTTP_GET, []() {
    if (server.hasArg("device") && server.hasArg("action")) {
      String device = server.arg("device");
      String action = server.arg("action");

      if (device == "relay1") {
        if (action == "on" && !relay1State) {
          digitalWrite(PIN_RELAY1, RELAY_ON);
          relay1State = true;
          lastRelay1State = relay1State;
          Serial.println("🔌 Relay 1 turned ON");
        } else if (action == "off" && relay1State) {
          digitalWrite(PIN_RELAY1, RELAY_OFF);
          relay1State = false;
          lastRelay1State = relay1State;
          Serial.println("🔌 Relay 1 turned OFF");
        }
      } else if (device == "relay2") {
        if (action == "on" && !relay2State) {
          digitalWrite(PIN_RELAY2, RELAY_ON);
          relay2State = true;
          lastRelay2State = relay2State;
          Serial.println("🌀 Relay 2 turned ON");
        } else if (action == "off" && relay2State) {
          digitalWrite(PIN_RELAY2, RELAY_OFF);
          relay2State = false;
          lastRelay2State = relay2State;
          Serial.println("🌀 Relay 2 turned OFF");
        }
      } else if (device == "led") {
        if (action == "on" && !ledState) {
          digitalWrite(PIN_LED, HIGH);
          ledState = true;
          manualOverride = true;
          lastLedState = ledState;
          Serial.println("💡 LED manually turned ON (Override)");
        } else if (action == "off" && ledState) {
          digitalWrite(PIN_LED, LOW);
          ledState = false;
          manualOverride = true;
          lastLedState = ledState;
          Serial.println("💡 LED manually turned OFF (Override)");
        } else if (action == "auto") {
          manualOverride = false;
          Serial.println("💡 LED reset to auto-control mode");
        }
      }

      server.send(200, "text/plain", "OK");
    }
  });

  server.begin();
  Serial.println("🌐 Web server started!");
  Serial.println("📊 Open browser and go to: http://" + WiFi.localIP().toString());
}

// ============ READ SENSORS (CORRECTED LOGIC) ============
void readSensors() {
  // Read DHT every 2 seconds
  static unsigned long lastDHTRead = 0;
  if (millis() - lastDHTRead > 2000) {
    lastDHTRead = millis();

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t)) temperature = t;
    if (!isnan(h)) humidity = h;
  }

  // Read LDR (Light Dependent Resistor)
  // LDR reads LOW when dark, HIGH when bright
  bool dark = (digitalRead(PIN_LDR) == LOW);

  // Update global dark state
  isDark = dark;

  // Track dark state changes (optional - remove if you don't want these logs)
  if (lastDarkState != isDark) {
    lastDarkState = isDark;
    // Uncomment below if you want to see light/dark state changes
    // Serial.println(isDark ? "🌙 Environment changed to DARK" : "☀️ Environment changed to LIGHT");
  }

  // Auto-control LED based on light conditions
  // ONLY if manual override is NOT active
  if (!manualOverride) {
    if (isDark && !ledState) {
      // Night mode: Turn LED ON (only if it's currently OFF)
      digitalWrite(PIN_LED, HIGH);
      ledState = true;
      if (lastLedState != ledState) {
        Serial.println("🌙 Night mode detected - LED turned ON");
        lastLedState = ledState;
      }
    } else if (!isDark && ledState) {
      // Day mode: Turn LED OFF (only if it's currently ON)
      digitalWrite(PIN_LED, LOW);
      ledState = false;
      if (lastLedState != ledState) {
        Serial.println("☀️ Day mode detected - LED turned OFF");
        lastLedState = ledState;
      }
    }
  }
  // If manualOverride is true, the LED state stays as manually set
}

// ============ MAIN LOOP ============
void loop() {
  server.handleClient();
  readSensors();
  delay(10);
}