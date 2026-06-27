# IoT Server Room Monitoring

Sistem monitoring suhu dan kelembapan ruang server berbasis IoT menggunakan ESP8266, sensor DHT22, MQTT, Node-RED, dan SQLite. Data dikirim setiap 15 detik untuk ditampilkan secara real-time dan disimpan sebagai data historis.

---

## Tujuan

- Monitoring suhu dan kelembapan ruang server secara real-time.
- Menyimpan data historis ke database SQLite.
- Menampilkan dashboard monitoring menggunakan Node-RED.

---

## Teknologi

### Hardware
- ESP8266
- DHT22
- Jumper Cable

### Software
- Arduino IDE / VS Code
- MQTT (Mosquitto)
- Node-RED
- SQLite
- Docker

---

## Arsitektur Sistem

<p align="center">
<img src="images/system_architecture.png" width="700">
</p>

*Keterangan: Alur komunikasi data dari sensor hingga dashboard monitoring.*

---

## Prototype Hardware

<p align="center">
<img src="images/hardware_prototype.jpg" width="700">
</p>

*Keterangan: Implementasi perangkat monitoring pada ruang server.*

---

## Dashboard Monitoring

<p align="center">
<img src="images/dashboard_monitoring.png" width="700">
</p>

*Keterangan: Dashboard Node-RED menampilkan suhu dan kelembapan secara real-time.*

---

## Database Monitoring

<p align="center">
<img src="images/database_result.png" width="700">
</p>

*Keterangan: Data suhu dan kelembapan tersimpan otomatis pada SQLite setiap 15 detik.*

---

## Fitur

- Real-time monitoring
- MQTT communication
- Node-RED dashboard
- SQLite data logging
- Docker deployment
