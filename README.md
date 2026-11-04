# BambuDeck-HA 🖨️✨
### Smart Home Assistant Touchscreen Deck for Bambu Lab Printers (CYD 2.8")

**BambuDeck-HA** is an open-source touchscreen controller and telemetry monitor for all Bambu Lab 3D printers (P1P, P1S, X1, X1-Carbon, X1E, A1, A1-Mini, and multi-nozzle / toolchanger architectures). It runs natively on the **CYD 2.8" (ESP32-2432S028R)** board.

Instead of binding directly to the printer's local MQTT server (which disconnects Bambu Studio or the Bambu Handy app), BambuDeck connects directly to **Home Assistant** and the popular `ha_bambulab` integration. All settings and WiFi configurations are saved directly to internal flash memory.

---

## 🌟 Key Features

- 🚀 **100% Standalone (No SD Card Needed)**: Boots and runs entirely from internal flash memory (**LittleFS**). Zero external media or SD card required.
- 🏠 **Direct Home Assistant Connection**: Communicates directly with Home Assistant's REST API (`/api/template` and `/api/states`) and standard MQTT broker. Zero extra bridge containers or YAML configuration files needed.
- 🌐 **Universal Bambu Lab Telemetry**:
  - Full telemetry and control for all current Bambu Lab printers (P1P, P1S, X1C, X1E, A1, A1-Mini).
  - Multi-nozzle / toolchanger telemetry support (dual nozzle temperatures, targets, and tool IDs matching `ha_bambulab` schemas).
  - Built-in chamber temperature monitoring (automatically reads built-in printer chamber sensors or an optional local DS18B20).
- 🌡️ **Optional Local DS18B20 Sensor**: Connect a DS18B20 sensor to GPIO 22 for instant chamber readings with Home Assistant auto-discovery.
- 🖼️ **Real-Time Print Cover Thumbnail**: Fetches print thumbnails in real time from Home Assistant (`image.<printer>_cover_image`) and renders them onto the LCD.
- 🔄 **AMS Management & RFID Resync**: Displays AMS filament slots, tray colors, active tray indicator, and features an interactive **Refresh AMS** button for RFID resync.
- ⚠️ **Error & HMS Notification Dialogs**: Real-time error popups with actionable Retry/Resume/Done buttons for filament runouts, AMS feeder jams, nozzle temperature errors, and print pauses.
- 📶 **Interactive Captive Portal**: Built-in WiFi and Home Assistant setup portal (`BambuDeck-Setup` AP at `192.168.4.1`) with live printer discovery dropdown.
- 🔄 **WiFi Web OTA & GitHub Releases**: Built-in Web OTA server at `http://<device-ip>/` for 1-click drag-and-drop firmware uploads, plus on-demand **Check for Updates** right from the Settings screen.
- 📦 **Single Factory Binary**: Merged standalone flash binary (`bambudeck.factory.bin`) ready for flashing at offset `0x0`.

---

## 🛠️ Hardware

- **Screen Board**: **ESP32-2432S028R** (CYD 2.8" Resistive Touchscreen, 320x240, ESP32-WROOM-32 with 4MB Flash, no PSRAM).
- **Optional Chamber Sensor**: Standard DS18B20 1-Wire temperature sensor connected to GPIO 22 (CN1 connector, Pin 22) with a 4.7kΩ pull-up resistor to 3.3V.
- **Power Connection**: Micro-USB or USB-C (5V 1A+ power source).

---

## ⚡ Quick Start & Installation

### 1. Flash Firmware to CYD
Download `bambudeck.factory.bin` from the [Releases](https://github.com/demonkazuya/BambuDeck-HA/releases) page.

Flash to your CYD using `esptool.py`:
```bash
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 write_flash 0x0 dist/bambudeck.factory.bin
```

Or build and flash directly from source with PlatformIO:
```bash
git clone https://github.com/demonkazuya/BambuDeck-HA.git
cd BambuDeck-HA
pio run -e esp32dev -t upload
```

### 2. Connect WiFi & Home Assistant
1. On initial boot, BambuDeck starts a WiFi setup access point:
   - **SSID**: `BambuDeck-Setup`
   - **IP**: `192.168.4.1`
2. Connect your phone or laptop to `BambuDeck-Setup` and browse to `http://192.168.4.1`.
3. Fill in:
   - **WiFi Network & Password**
   - **Home Assistant URL** (e.g. `http://192.168.1.100:8123`)
   - **Long-Lived Access Token** (created in Home Assistant under *Your Profile > Security > Long-Lived Access Tokens*)
4. Select your printer from the discovered printers dropdown and tap **Save & Connect**.
5. BambuDeck saves the configuration to internal flash, connects to your network, and starts displaying live telemetry!

---

## 🌐 Updating Firmware

### Web Browser OTA
BambuDeck runs an integrated web update server. Open:
```text
http://<bambudeck-ip>/
```
in any web browser on your network, select `bambudeck.bin`, and click **Upload & Flash OTA**.

### On-Device GitHub Update Check
In the **Settings** menu on the touchscreen:
- Tap **Check for Updates** to query GitHub for new releases. If an update is found, BambuDeck displays the version and offers to install it directly over WiFi.
- Enable the **OTA Updates** toggle to automatically check for updates on startup.

---

## 🧭 Screen Navigation

- **Home Screen**:
  - Print state, progress percentage, time remaining, and current layer.
  - Live model thumbnail image streamed from Home Assistant.
  - Bed, nozzle, and chamber temperatures.
  - Light toggle and Stop/Pause/Resume print buttons.
- **Control Screen**:
  - Manual X/Y/Z axis movement (1mm / 10mm increment toggle).
  - Target temperature adjustments for bed and nozzle.
  - Speed adjustments for part cooling fan, AUX fan, and chamber fan.
- **Filament / AMS Screen**:
  - Active tray indicator, slot materials, and filament colors.
  - **Refresh AMS** button (triggers RFID resync).
  - Extrude, retract, load, and unload controls.
- **Settings Screen**:
  - Brightness slider and LCD sleep timer (with wake-on-print).
  - Color invert and 180° screen rotation toggle.
  - System info, IP address, and active printer display.
  - **Check for Updates** button.
  - Setup portal launcher / WiFi reconfigure.

