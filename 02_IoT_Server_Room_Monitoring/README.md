# IoT Server Room Monitoring

Sistem monitoring **suhu** dan **kelembapan ruang server** berbasis **Internet of Things (IoT)** menggunakan **ESP8266**, **sensor DHT22**, **MQTT**, **Node-RED**, dan **SQLite**. Sistem mengirimkan data setiap **15 detik**, menampilkan kondisi secara **real-time**, serta menyimpan data historis untuk mendukung pemantauan lingkungan ruang server.

---

## Tujuan

- Mengembangkan **sistem monitoring suhu dan kelembapan** secara **real-time**.
- Mengirim data sensor menggunakan **protokol MQTT**.
- Menampilkan data pada **dashboard Node-RED**.
- Menyimpan data historis ke dalam **database SQLite**.

---

## Teknologi

### Hardware

- **ESP8266**
- **Sensor DHT22**

### Software

- **Arduino IDE**
- **MQTT (Mosquitto Broker)**
- **Node-RED**
- **SQLite**
- **Docker**

---

## Arsitektur Sistem

<p align="center">
<img src="images/Arsitektur_Sistem.png" width="750">
</p>

**Keterangan:** Alur komunikasi data dari **ESP8266** menuju **MQTT Broker**, **Node-RED**, **SQLite**, dan **Dashboard Monitoring**.

---

## Implementasi Hardware

<p align="center">
<img src="images/Prototype_Perangkat_Keras.jpg" width="750">
</p>

**Keterangan:** Prototype perangkat monitoring berbasis **ESP8266** dan **sensor DHT22** yang digunakan pada proses implementasi sistem.

---

## Dashboard Monitoring

<p align="center">
<img src="images/Dashboard_Monitoring.png" width="750">
</p>

**Keterangan:** Dashboard **Node-RED** menampilkan data **suhu**, **kelembapan**, dan **grafik monitoring** secara **real-time**.

---

## Database Monitoring

<p align="center">
<img src="images/Database_SQLite.png" width="750">
</p>

**Keterangan:** Data sensor disimpan secara otomatis ke dalam **database SQLite** setiap **15 detik**.

---

## Fitur

- **Real-time Temperature Monitoring**
- **Real-time Humidity Monitoring**
- **MQTT Communication**
- **Node-RED Dashboard**
- **SQLite Data Logging**
- **Docker Deployment**

---

## Dokumentasi

- **Arsitektur Sistem**
- **Prototype Perangkat Keras**
- **Dashboard Monitoring**
- **Database SQLite**
- **Source Code ESP8266**
- **Flow Node-RED**
- **Konfigurasi Docker**

---

## Kompetensi yang Ditunjukkan

- **Internet of Things (IoT)**
- **Embedded System**
- **ESP8266 Programming**
- **MQTT Protocol**
- **Node-RED**
- **Database Management (SQLite)**
- **Docker**
- **Real-Time Monitoring System**
