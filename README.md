# OTA Base Firmware

[English Version](README_en.md) | [Bahasa Indonesia](README.md)

---

Proyek ini adalah sistem dasar untuk ESP32 yang mendukung OTA (Over-The-Air) update dengan bantuan protokol MQTT untuk menerima perintah *update* jarak jauh.

## Cara Setup untuk Upload Firmware Manual

Langkah-langkah berikut digunakan jika Anda ingin melakukan *flash* firmware secara langsung (via kabel USB) ke perangkat ESP32 Anda.

1. **Persiapan Kredensial:**
   - Duplikasi atau *copy* file `secret.h.example` dan ubah namanya menjadi `secret.h`.
   - Buka file `secret.h` dan sesuaikan parameter berikut dengan konfigurasi jaringan dan server yang akan digunakan:
     - **WiFi Credentials:** `ssid` dan `password` jaringan WiFi Anda.
     - **MQTT Config:** `mqtt_server`, `mqtt_port`, `mqtt_user`, `mqtt_pass` untuk broker MQTT Anda.
     - **Device ID:** `device_id` (ID unik untuk mengidentifikasi perangkat).
     - **OTA Config:** `ota_server` (domain atau IP API untuk *download* firmware OTA).

2. **Upload ke ESP32:**
   - Buka file proyek utama `ota-base-firmware.ino` menggunakan Arduino IDE atau ekstensi Arduino di VS Code.
   - Pastikan pustaka (libraries) yang dibutuhkan sudah terinstal di IDE Anda, seperti:
     - `PubSubClient`
     - `ArduinoJson`
   - Pastikan *board* yang terpilih adalah **ESP32** (misal: ESP32 Dev Module).
   - Sambungkan board ESP32 ke PC/Laptop menggunakan kabel USB.
   - Lakukan kompilasi dan *upload* (Flash).

> [!NOTE]
> *Catatan Penting:* Saat proses flash manual via USB (*power on* / reset terdeteksi dari hardware), kredensial yang ada di `secret.h` akan ditulis ulang ke memori internal ESP32 (menggunakan fitur `Preferences`). Jika *device* mengalami *reboot* akibat OTA update, ESP32 akan tetap menggunakan kredensial sebelumnya yang tersimpan di dalam memori.

---

## Cara Menggunakan GitHub Actions (Otomatisasi Build & OTA)

Proyek ini dilengkapi dengan *workflow* GitHub Actions yang bertugas melakukan *compile code* menjadi file `.bin` lalu secara otomatis mengirimkan file tersebut ke Server OTA Anda kapanpun ada pembaruan *code*.

1. **Konfigurasi GitHub Secrets:**
   Agar GitHub Actions diizinkan untuk mengirim *file binary* ke server OTA, Anda harus menambahkan *Secrets* di repositori GitHub.
   - Buka halaman repositori Anda di GitHub.
   - Pergi ke menu **Settings** > **Secrets and variables** > **Actions**.
   - Klik **New repository secret** dan tambahkan dua variabel berikut:
     - `OTA_API_KEY`: API Key yang digunakan server untuk memverifikasi autentikasi pengunggahan firmware.
     - `OTA_UPLOAD_URL`: *Endpoint* URL untuk melakukan POST upload *firmware* (misal: `https://otaapi.example.com/api/upload`).

2. **Trigger (Menjalankan) Workflow:**
   Workflow (*Build ESP32 .bin*) akan aktif dan berjalan melalui metode berikut:
   - **Otomatis (Push):** Ketika Anda melakukan komit (commit) dan push ke branch `master` atau `releases`.
   - **Otomatis (Pull Request):** Ketika ada *pull request* yang ditargetkan ke branch `master` atau `releases`.
   - **Manual (Workflow Dispatch):** Pergi ke tab **Actions** di repositori GitHub, pilih **Build ESP32 .bin** di sebelah kiri, lalu klik **Run workflow**.

3. **Cara Kerja Workflow:**
   - Mengunduh *source code* dan menjalankan `ubuntu-latest`.
   - Melakukan setup `arduino-cli` beserta sistem *core* untuk ESP32.
   - Mengunduh library yang dibutuhkan (`PubSubClient` & `ArduinoJson`).
   - Menyalin `secret.h.example` menjadi `secret.h` (karena *secret* yang asli diabaikan oleh `.gitignore`).
   - Meng-*compile* source code menjadi file `.bin` yang siap di-flash.
   - Mendeteksi versi firmware otomatis dengan membaca variabel `current_version` pada `ota-base-firmware.ino`.
   - Mengirim (*upload*) file `.bin` beserta informasi versinya ke `OTA_UPLOAD_URL` dengan _header_ autentikasi menggunakan `OTA_API_KEY`.
   - *Artifact* dari hasil `.bin` juga akan tersedia untuk di-*download* manual di halaman *summary* masing-masing *workflow run* di GitHub Actions.
