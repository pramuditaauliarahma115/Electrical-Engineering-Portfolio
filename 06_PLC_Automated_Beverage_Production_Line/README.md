# PLC Automated Beverage Production System

Perancangan sistem otomasi lini produksi minuman berbasis PLC menggunakan ladder diagram dan Human Machine Interface (HMI). Sistem mengotomatisasi proses pengisian botol, pelabelan, penyimpanan produk berdasarkan jenis minuman, serta penanganan kondisi safety menggunakan konsep sequential control dan Finite State Machine (FSM).

---

## Ringkasan Proyek

Proyek ini mengimplementasikan sistem otomasi industri yang terdiri dari conveyor, dua silinder pneumatik, valve pengisian, oven, mekanisme labeling, serta sistem penyimpanan otomatis untuk tiga jenis produk (Strawberry, Orange, dan Melon). Operator mengatur kapasitas penyimpanan melalui HMI, sedangkan seluruh proses produksi berjalan otomatis menggunakan logika PLC.

---

## Teknologi

### Hardware

- PLC Omron
- Human Machine Interface (HMI)
- Conveyor Motor
- Pneumatic Cylinder (2 Unit)
- Solenoid Valve
- Filling Valve
- Skate Slide Actuator
- Tower Lamp
- Limit Sensor
- Push Button

### Software

- CX-Programmer
- CX-Designer
- Ladder Diagram

---

## Arsitektur Sistem

Diagram berikut menunjukkan alur proses produksi mulai dari pengisian botol hingga penyimpanan otomatis berdasarkan jenis produk.

(Tambahkan Arsitektur_Sistem.png)

---

## Implementasi Sistem

Implementasi meliputi pemrograman PLC, desain HMI, pengendalian conveyor, filling station, labeling station, serta penyimpanan otomatis berdasarkan jenis produk.

(Tambahkan Implementasi_PLC.png)

---

## Hasil Implementasi

Sistem berhasil mengotomatisasi seluruh proses produksi menggunakan PLC dan HMI.

### Ringkasan Hasil

- Filling botol otomatis menggunakan pneumatic cylinder dan valve.
- Labeling otomatis menggunakan motor dan solenoid.
- Penyimpanan otomatis berdasarkan jenis minuman.
- Kapasitas penyimpanan dapat diatur melalui HMI.
- Produk dialihkan ke jalur reject apabila penyimpanan penuh.
- Emergency Stop, Stop Cycle, dan Reset Sequence berhasil diimplementasikan.
- Tower lamp menampilkan status operasi mesin.
- Sensor posisi digunakan untuk sinkronisasi seluruh proses produksi.

---

## File Utama

| File | Keterangan |
|------|------------|
| PLC_Beverage_Production.cxp | Program ladder diagram PLC Omron. |
| HMI_Beverage_Production.cxd | Desain Human Machine Interface (HMI). |
| README.md | Dokumentasi proyek. |

---

## Kompetensi

- PLC Programming
- Ladder Diagram
- Human Machine Interface (HMI)
- Industrial Automation
- Sequential Control
- Finite State Machine (FSM)
- Conveyor Automation
- Pneumatic Control
- Process Automation
- Sensor Integration
- Industrial Safety System
