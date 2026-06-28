# PLC Traffic Light Control System

Perancangan dan implementasi sistem pengendalian lampu lalu lintas berbasis **PLC Omron** menggunakan konsep **Finite State Machine (FSM)** dan **timer sequencing**. Sistem mengatur siklus lampu merah, kuning, dan hijau secara otomatis untuk mensimulasikan pengendalian persimpangan jalan.

---

## Ringkasan Proyek

Proyek ini mengimplementasikan sistem kontrol lampu lalu lintas menggunakan PLC sebagai media pembelajaran otomasi industri. Logika pengendalian dikembangkan menggunakan ladder diagram dengan memanfaatkan timer, internal relay, dan state sequencing untuk menghasilkan operasi lampu lalu lintas yang berjalan secara berurutan dan andal.

---

## Teknologi

### Hardware

- PLC Omron
- Push Button
- Indicator Lamp (Red, Yellow, Green)
- Power Supply 24 VDC

### Software

- CX-Programmer
- Ladder Diagram

---

## Arsitektur Sistem

Diagram berikut menunjukkan alur kerja sistem pengendalian lampu lalu lintas berbasis PLC mulai dari proses inisialisasi hingga pergantian status lampu secara otomatis.

<p align="center">
  <img src="images/Arsitektur_Sistem.png" width="900">
</p>

---

## Implementasi Hardware

Prototype terdiri atas PLC Omron yang mengendalikan indikator lampu merah, kuning, dan hijau berdasarkan urutan waktu yang telah diprogram menggunakan ladder diagram.

<p align="center">
  <img src="images/Implementasi_PLC.png" width="900">
</p>

---

## Hasil Implementasi

Sistem berhasil mengendalikan siklus lampu lalu lintas secara otomatis menggunakan PLC dengan perpindahan status yang berjalan sesuai urutan program ladder.

### Ringkasan Hasil

- Mengimplementasikan logika ladder diagram menggunakan PLC Omron.
- Pergantian lampu merah, kuning, dan hijau berlangsung secara otomatis.
- Timer PLC mengatur durasi setiap fase lampu dengan stabil.
- Konsep Finite State Machine (FSM) diterapkan untuk mengendalikan urutan siklus lampu.
- Sistem dapat digunakan sebagai media pembelajaran otomasi industri berbasis PLC.

---

## Kompetensi

- PLC Programming
- Omron PLC
- Ladder Diagram
- Industrial Automation
- Finite State Machine (FSM)
- Sequential Control
- Timer Programming
- Digital Control System

---
## File Utama

| File | Keterangan |
|------|------------|
| PLC_Traffic_Light.cxp | Program ladder diagram menggunakan Omron CX-Programmer. |
| README.md | Dokumentasi proyek. |

---
