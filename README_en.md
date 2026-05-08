# OTA Base Firmware

[English Version](README_en.md) | [Bahasa Indonesia](README.md)

---

This project is a base system for ESP32 that supports OTA (Over-The-Air) updates with the help of the MQTT protocol to receive remote update commands.

## Setup Instructions for Manual Firmware Upload

The following steps are used if you want to flash the firmware directly (via USB cable) to your ESP32 device.

1. **Credential Preparation:**
   - Duplicate or copy the `secret.h.example` file and rename it to `secret.h`.
   - Open the `secret.h` file and adjust the following parameters with the network and server configurations you will use:
     - **WiFi Credentials:** `ssid` and `password` for your WiFi network.
     - **MQTT Config:** `mqtt_server`, `mqtt_port`, `mqtt_user`, `mqtt_pass` for your MQTT broker.
     - **Device ID:** `device_id` (a unique ID to identify the device).
     - **OTA Config:** `ota_server` (domain or IP API to download the OTA firmware).

2. **Upload to ESP32:**
   - Open the main project file `ota-base-firmware.ino` using the Arduino IDE or the Arduino extension in VS Code.
   - Ensure the required libraries are installed in your IDE, such as:
     - `PubSubClient`
     - `ArduinoJson`
   - Make sure the selected board is **ESP32** (e.g., ESP32 Dev Module).
   - Connect the ESP32 board to your PC/Laptop using a USB cable.
   - Compile and upload (Flash) the code.

> [!NOTE]
> *Important Note:* During the manual flash process via USB (hardware reset / power-on detected), the credentials in `secret.h` will be rewritten to the ESP32's internal memory (using the `Preferences` library). If the device reboots due to an OTA update, the ESP32 will retain and use the previous credentials stored in its memory.

---

## How to Use GitHub Actions (Automated Build & OTA)

This project is equipped with a GitHub Actions workflow that automatically compiles the code into a `.bin` file and sends it to your OTA Server whenever there is a code update.

1. **GitHub Secrets Configuration:**
   For GitHub Actions to be allowed to send the binary file to the OTA server, you must add Secrets in your GitHub repository.
   - Open your repository page on GitHub.
   - Go to **Settings** > **Secrets and variables** > **Actions**.
   - Click **New repository secret** and add the following two variables:
     - `OTA_API_KEY`: The API Key used by the server to verify the firmware upload authentication.
     - `OTA_UPLOAD_URL`: The URL endpoint to POST the firmware upload (e.g., `https://otaapi.example.com/api/upload`).

2. **Triggering the Workflow:**
   The workflow (*Build ESP32 .bin*) will be triggered and run through the following methods:
   - **Automatic (Push):** When you commit and push code to the `master` or `releases` branch.
   - **Automatic (Pull Request):** When there is a pull request targeted at the `master` or `releases` branch.
   - **Manual (Workflow Dispatch):** Go to the **Actions** tab in your GitHub repository, select **Build ESP32 .bin** on the left sidebar, and click **Run workflow**.

3. **How the Workflow Works:**
   - Downloads the source code and runs on an `ubuntu-latest` environment.
   - Sets up `arduino-cli` along with the core system for ESP32.
   - Downloads the required libraries (`PubSubClient` & `ArduinoJson`).
   - Copies `secret.h.example` to `secret.h` (because the actual secret is ignored by `.gitignore`).
   - Compiles the source code into a ready-to-flash `.bin` file.
   - Automatically detects the firmware version by reading the `current_version` variable in `ota-base-firmware.ino`.
   - Sends (uploads) the `.bin` file along with its version information to the `OTA_UPLOAD_URL` endpoint with an authentication header using the `OTA_API_KEY`.
   - The resulting `.bin` artifact will also be available for manual download on the summary page of each workflow run in GitHub Actions.
