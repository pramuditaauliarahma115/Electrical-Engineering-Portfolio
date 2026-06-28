# RGB Conveyor Sorting System

Perancangan dan implementasi sistem penyortiran objek berdasarkan warna menggunakan **Arduino Uno**, LED RGB, sensor LDR/Photodiode, motor DC conveyor, dan dua motor servo. Sistem mengidentifikasi warna objek melalui pemrosesan sinyal pantulan cahaya, kemudian melakukan penyortiran secara otomatis sesuai hasil klasifikasi.

## Ringkasan Proyek

Proyek ini bertujuan mengembangkan sistem **RGB Conveyor Sorting** berbasis embedded system sebagai media pembelajaran otomasi industri dan pengolahan sinyal sensor. Identifikasi warna dilakukan menggunakan kombinasi LED merah, hijau, dan biru sebagai sumber pencahayaan serta sensor LDR/Photodiode sebagai penerima pantulan cahaya. Data sensor diproses menggunakan filtering, normalisasi RGB, dan algoritma **Euclidean Distance** untuk menentukan warna objek sebelum dilakukan proses penyortiran menggunakan dua buah servo.

## Tujuan

* Merancang sistem penyortiran objek otomatis berbasis Arduino Uno.
* Mengimplementasikan deteksi warna menggunakan LED RGB dan LDR/Photodiode.
* Mengembangkan algoritma filtering dan normalisasi RGB untuk meningkatkan akurasi identifikasi warna.
* Mengimplementasikan mekanisme penyortiran otomatis menggunakan conveyor dan dua motor servo.
* Mengevaluasi performa sistem dalam mengklasifikasikan objek berdasarkan warna.

## Teknologi

### Software

* Arduino IDE
* C/C++
* Proteus (Hardware Simulation)

### Hardware

* Arduino Uno
* L298N Motor Driver
* Motor DC Conveyor
* Servo Motor (2 Unit)
* LED Merah
* LED Hijau
* LED Biru
* LDR / Photodiode
* Push Button
* Catu Daya 5V / 12V

## Arsitektur Sistem

Diagram blok sistem menunjukkan alur kerja mulai dari deteksi objek, pembacaan warna, pemrosesan data RGB, klasifikasi warna, hingga proses penyortiran menggunakan conveyor dan servo.

*(Tambahkan gambar Block Diagram di sini.)*

---

## Implementasi Hardware

Prototype sistem terdiri atas conveyor berbasis motor DC, mekanisme penyortiran menggunakan dua servo, LED RGB sebagai sumber pencahayaan, serta sensor LDR/Photodiode untuk membaca pantulan cahaya dari objek.

*(Tambahkan foto prototype di sini.)*

---

## Algoritma Sistem

1. Inisialisasi seluruh perangkat keras.
2. Membaca data kalibrasi dari EEPROM.
3. Mendeteksi keberadaan objek pada conveyor.
4. Mengaktifkan LED merah, hijau, dan biru secara bergantian.
5. Membaca intensitas pantulan menggunakan sensor.
6. Melakukan Median Filter dan Moving Average Filter.
7. Melakukan normalisasi RGB.
8. Mengklasifikasikan warna menggunakan Euclidean Distance.
9. Menyortir objek menggunakan dua motor servo.
10. Mengembalikan sistem ke kondisi standby untuk objek berikutnya.

## Fitur Sistem

* Finite State Machine (FSM)
* RGB Color Detection
* Median Filter
* Moving Average Filter
* Ambient Light Compensation
* RGB Normalization
* Euclidean Distance Classification
* Three-Color Calibration
* EEPROM Calibration Storage
* CRC Validation
* FIFO Queue
* Conveyor Synchronization
* Reject Bin
* Servo Timeout Protection
* Startup Self-Test
* Watchdog Timer
* Serial Diagnostic Mode

## Hasil Pengujian

Pengujian dilakukan menggunakan objek berwarna merah, hijau, dan biru dengan beberapa kali pengulangan untuk mengevaluasi kemampuan sistem dalam mengidentifikasi warna serta melakukan penyortiran secara otomatis.

**Ringkasan Hasil**

* Sistem mampu membedakan objek berwarna merah, hijau, dan biru menggunakan LED RGB dan sensor LDR/Photodiode.
* Filtering dan normalisasi RGB meningkatkan kestabilan hasil pembacaan sensor.
* Mekanisme conveyor dan dua servo berhasil menyortir objek sesuai hasil klasifikasi.
* EEPROM memungkinkan penyimpanan data kalibrasi sehingga sistem tidak memerlukan kalibrasi ulang setiap dinyalakan.
* Watchdog Timer meningkatkan keandalan sistem ketika terjadi gangguan pada program.

## File Utama

| File                            | Keterangan                                             |
| ------------------------------- | ------------------------------------------------------ |
| RGB_Conveyor_Sorting_System.ino | Program utama sistem penyortiran berbasis Arduino Uno. |
| README.md                       | Dokumentasi proyek.                                    |

