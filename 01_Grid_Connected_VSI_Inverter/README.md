# Grid-Connected VSI Inverter with PI Current Control

Perancangan dan implementasi **Single-Phase Grid-Connected Voltage Source Inverter (VSI)** menggunakan **Proportional-Integral (PI) Current Control** berbasis **Arduino Uno** untuk menghasilkan arus keluaran yang sinkron dengan jaringan listrik.

---

## Ringkasan Proyek

Proyek ini merupakan tugas akhir Program Sarjana Teknik Elektro yang berfokus pada pengembangan inverter H-Bridge berbasis SPWM dengan kontrol arus PI. Sistem dirancang melalui simulasi menggunakan MATLAB/Simulink dan PSIM, kemudian diimplementasikan pada prototipe perangkat keras untuk mengevaluasi performa sistem terhadap jaringan listrik.

---

## Tujuan

- Merancang inverter H-Bridge satu fasa berbasis SPWM.
- Mengimplementasikan PI Current Controller pada Arduino Uno.
- Menganalisis performa sistem melalui simulasi dan implementasi.
- Mengevaluasi respons arus inverter terhadap referensi jaringan listrik.

---

## Teknologi

### Software

- MATLAB/Simulink
- PSIM
- Arduino IDE
- Proteus

### Hardware

- Arduino Uno
- IR2112 Gate Driver
- IRFP460 MOSFET
- TLP250 Optocoupler
- ACS712 Current Sensor
- LC Filter
- Step-Up Transformer

---

## Arsitektur Sistem

Diagram berikut menunjukkan arsitektur keseluruhan sistem Grid-Connected VSI Inverter dengan PI Current Control.

<p align="center">
  <img src="images/system_architecture.png" alt="System Architecture" width="900">
</p>

Sistem terdiri dari sumber DC 24 V, Arduino Uno sebagai pengendali PI, pembangkit SPWM, gate driver, inverter H-Bridge VSI, filter LC, transformator, serta sensor arus sebagai umpan balik (closed-loop current control).
---

## Implementasi Hardware

*(Masukkan foto prototype terbaik)*

---

## Hasil Simulasi

*(Masukkan gambar gelombang PWM, arus, tegangan, dan THD)*

---

## Hasil Implementasi

| Parameter | Nilai |
|-----------|-------|
| Kontrol | PI Current Control |
| Metode PWM | SPWM |
| Frekuensi Switching | 22 kHz |
| Power Factor | 0.83 |
| Beban Pengujian | 8 Ω |

> Isi tabel hanya dengan data yang memang didukung hasil pengujian atau laporan.

---

## Dokumentasi

- Diagram blok sistem
- Skematik inverter
- Rangkaian driver
- Hasil simulasi MATLAB/PSIM
- Prototype perangkat keras
- Hasil pengujian

---

## Struktur Repository

```text
01_Grid_Connected_VSI_Inverter
│
├── README.md
├── images
├── arduino
├── matlab
├── psim
├── proteus
└── docs
```

---

## Kompetensi yang Ditunjukkan

- Power Electronics
- Grid-Connected Inverter
- PI Current Control
- SPWM
- Embedded System
- Hardware Integration
- Power System
- MATLAB Simulation
- Industrial Control
