# IoT Server Room Monitoring

Sistem **monitoring suhu dan kelembapan ruang server** berbasis **Internet of Things (IoT)** menggunakan **ESP8266**, **DHT22**, **MQTT**, **Node-RED**, dan **SQLite**. Sistem mengirim data setiap **15 detik**, menampilkan kondisi secara **real-time**, serta menyimpan data historis.

---

## Teknologi

**Hardware**
- ESP8266
- DHT22

**Software**
- Arduino IDE
- MQTT (Mosquitto)
- Node-RED
- SQLite
- Docker

---

## Implementasi Hardware

Prototype sistem monitoring suhu dan kelembapan berbasis ESP8266 yang digunakan pada proses implementasi dan pengujian.

<p align="center">
  <img src="images/Prototype_Perangkat_Keras.png" width="850">
</p>

---

## Dashboard Monitoring

Dashboard Node-RED menampilkan data suhu dan kelembapan secara real-time serta histori pengukuran.

<p align="center">
  <img src="images/Dashboard_NodeRED.png" width="900">
</p>

---

## Hasil Pengujian

Sistem berhasil mengirim data sensor ke MQTT Broker, menampilkan data pada dashboard secara real-time, serta menyimpan data historis ke database SQLite.

<p align="center">
  <img src="images/Hasil_Pengujian.png" width="900">
</p>

---

## Database Monitoring

<p align="center">
<img src="images/Database_SQLite.png" width="800">
</p>

---

## Fitur

- Real-time Monitoring
- MQTT Communication
- Node-RED Dashboard
- SQLite Data Logging
- Docker Deployment

---

## Kompetensi

- Internet of Things (IoT)
- ESP8266
- MQTT
- Node-RED
- SQLite
- Docker
- Embedded System
