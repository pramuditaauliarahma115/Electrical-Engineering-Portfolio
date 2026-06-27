# IoT Server Room Monitoring

Sistem monitoring suhu dan kelembapan ruang server berbasis **Internet of Things (IoT)** menggunakan **ESP8266**, **DHT22**, **MQTT**, **Node-RED**, dan **SQLite**. Sistem mengirim data sensor setiap **15 detik**, menampilkan kondisi lingkungan secara **real-time**, serta menyimpan data historis untuk analisis dan monitoring.

---

## Teknologi

### Hardware
- ESP8266
- DHT22

### Software
- Arduino IDE
- MQTT (Mosquitto)
- Node-RED
- SQLite
- Docker

---

## Arsitektur Sistem

Diagram berikut menunjukkan alur komunikasi data mulai dari sensor hingga dashboard monitoring dan database.

<p align="center">
  <img src="images/Arsitektur_Monitoring_IoT.png" width="1000">
</p>

---

## Implementasi Sistem Monitoring

Implementasi sistem meliputi **flow Node-RED**, **dashboard monitoring real-time**, dan **database SQLite** untuk penyimpanan data historis suhu serta kelembapan ruang server.

<p align="center">
  <img src="images/Implementasi_Monitoring.png" width="1000">
</p>

---

## Hasil Implementasi

- Monitoring suhu dan kelembapan secara **real-time** setiap **15 detik**.
- Dashboard Node-RED menampilkan **gauge** dan **grafik historis**.
- Data sensor tersimpan otomatis pada **database SQLite**.
- Komunikasi data menggunakan protokol **MQTT** dengan arsitektur **Publisher–Subscriber**.

---

## Kompetensi

- Internet of Things (IoT)
- ESP8266
- MQTT
- Node-RED
- SQLite Database
- Docker
- Embedded System
- Real-Time Monitoring
---
## File Utama

| File | Keterangan |
|------|------------|
| Program_ESP8266_Monitoring.ino | Firmware ESP8266 untuk membaca sensor DHT22, mengirim data ke MQTT Broker, dan menyediakan web monitoring. |
| platformio.ini | Konfigurasi environment PlatformIO beserta dependensi library yang digunakan pada proyek. |
---
