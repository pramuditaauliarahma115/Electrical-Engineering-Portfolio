#include <Wire.h>
#include <Servo.h>
#include <EEPROM.h>
#include <avr/wdt.h>

// ==========================================
// DEFINISI PIN I/O
// ==========================================
#define PIN_LED_R       2
#define PIN_LED_G       3
#define PIN_LED_B       4
#define PIN_LDR         A0
#define PIN_CALIB_BTN   5

// Driver Motor L298N
#define PIN_ENA         6
#define PIN_IN1         7
#define PIN_IN2         8

// Servo Aktuator
#define PIN_SERVO_1     9
#define PIN_SERVO_2     10

// Indikator Sistem
#define PIN_LED_ERR     13

// ==========================================
// KONSTANTA & KONFIGURASI
// ==========================================
#define EEPROM_ADDR_CALIB   0
#define CRC_POLY            0x07
#define FILTER_SAMPLES      5
#define DEBOUNCE_DELAY      50      // milidetik
#define TIMEOUT_CONVEYOR    10000   // 10 detik batas waktu objek tersangkut
#define TIMEOUT_SERVO       2000    // 2 detik batas waktu pergerakan servo

// Posisi Servo
#define SERVO_NETRAL        90
#define SERVO_S1_KIRI       45
#define SERVO_S2_KIRI       45
#define SERVO_S2_KANAN      135

// ==========================================
// STRUKTUR DATA & ENUM
// ==========================================
enum SystemState {
  STATE_INIT,
  STATE_SELF_TEST,
  STATE_STANDBY,
  STATE_OBJECT_DETECTED,
  STATE_READ_R,
  STATE_READ_G,
  STATE_READ_B,
  STATE_CLASSIFY,
  STATE_ENQUEUE,
  STATE_SYNCHRONIZE,
  STATE_SORTING_ACTIVATE,
  STATE_SORTING_WAIT,
  STATE_CONVEYOR_RECOVERY,
  STATE_ERROR
};

enum ColorType { COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_UNKNOWN };

// Prioritas 1: Struktur Kalibrasi 3 Warna Sebenarnya
struct CalibrationData {
  float red[3];    // [0]=R, [1]=G, [2]=B hasil normalisasi saat sampel merah
  float green[3];  // [0]=R, [1]=G, [2]=B hasil normalisasi saat sampel hijau
  float blue[3];   // [0]=R, [1]=G, [2]=B hasil normalisasi saat sampel biru
  uint8_t crc;
};

struct ObjectData {
  ColorType color;
  unsigned long detectionTime;
};

// Antrean Ring Buffer (FIFO)
#define QUEUE_SIZE 5
ObjectData fifoQueue[QUEUE_SIZE];
int queueHead = 0;
int queueTail = 0;
int queueCount = 0;

// ==========================================
// VARIABEL GLOBAL & STATISTIK (Prioritas 3)
// ==========================================
SystemState currentState = STATE_INIT;
CalibrationData calib;
Servo servo1;
Servo servo2;

int ambientBaseline = 0;
int adaptiveThreshold = 40; // Nilai awal dinamis
uint8_t samplingRetryCount = 0;
uint8_t conveyorRetryCount = 0;
uint8_t servoRetryCount = 0;

// Variabel Penjadwal Non-blocking (Prioritas 5)
unsigned long stateTimer = 0;
unsigned long debounceTimer = 0;
unsigned long servoTimer = 0;
unsigned long generalTimer = 0;

// Penyimpan Data Sementara Pembacaan Komponen Warna
int rawR = 0, rawG = 0, rawB = 0;

// Statistik Operasi (Prioritas 3)
unsigned long totalObjectsProcessed = 0;
unsigned long countRed = 0;
unsigned long countGreen = 0;
unsigned long countBlue = 0;
unsigned long countReject = 0;

// ==========================================
// FUNGSI UTILITAS & FILTERING
// ==========================================

uint8_t calculateCRC(uint8_t *data, size_t len) {
  uint8_t crc = 0x00;
  for (size_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x80) crc = (crc << 1) ^ CRC_POLY;
      else crc <<= 1;
    }
  }
  return crc;
}

bool enqueue(ObjectData obj) {
  if (queueCount >= QUEUE_SIZE) return false; 
  fifoQueue[queueTail] = obj;
  queueTail = (queueTail + 1) % QUEUE_SIZE;
  queueCount++;
  return true;
}

bool dequeue(ObjectData &obj) {
  if (queueCount <= 0) return false; 
  obj = fifoQueue[queueHead];
  queueHead = (queueHead + 1) % QUEUE_SIZE;
  queueCount--;
  return true;
}

void controlConveyor(bool run, bool forward = true, uint8_t speed = 200) {
  if (run) {
    if (forward) {
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);
    } else {
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, HIGH);
    }
    analogWrite(PIN_ENA, speed);
  } else {
    digitalWrite(PIN_IN1, LOW);
    digitalWrite(PIN_IN2, LOW);
    analogWrite(PIN_ENA, 0);
  }
}

int readFilteredLDR() {
  int samples[FILTER_SAMPLES];
  for (int i = 0; i < FILTER_SAMPLES; i++) {
    samples[i] = analogRead(PIN_LDR);
    delay(2); // Jeda sangat kecil aman untuk pemfilteran mikro
  }
  for (int i = 0; i < FILTER_SAMPLES - 1; i++) {
    for (int j = 0; j < FILTER_SAMPLES - i - 1; j++) {
      if (samples[j] > samples[j + 1]) {
        int temp = samples[j];
        samples[j] = samples[j + 1];
        samples[j + 1] = temp;
      }
    }
  }
  long sum = 0;
  for (int i = 1; i < FILTER_SAMPLES - 1; i++) {
    sum += samples[i];
  }
  return sum / (FILTER_SAMPLES - 2);
}

void printSystemDiagnostics() {
  Serial.println(F("\n=== DIAGNOSTIK SISTEM ==="));
  Serial.print(F("Waktu Aktif (s): ")); Serial.println(millis() / 1000);
  Serial.print(F("Ambient Baseline: ")); Serial.println(ambientBaseline);
  Serial.print(F("Adaptive Threshold: ")); Serial.println(adaptiveThreshold);
  Serial.print(F("Total Objek: ")); Serial.println(totalObjectsProcessed);
  Serial.print(F("R/G/B/REJ: "));
  Serial.print(countRed); Serial.print(F("/"));
  Serial.print(countGreen); Serial.print(F("/"));
  Serial.print(countBlue); Serial.print(F("/"));
  Serial.println(countReject);
  Serial.println(F("========================="));
}

// ==========================================
// PROSEDUR DIAGNOSTIK & UTAMA ARDUINO
// ==========================================
void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_LED_R, OUTPUT);
  pinMode(PIN_LED_G, OUTPUT);
  pinMode(PIN_LED_B, OUTPUT);
  pinMode(PIN_LED_ERR, OUTPUT);
  pinMode(PIN_CALIB_BTN, INPUT_PULLUP);
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_ENA, OUTPUT);
  
  servo1.attach(PIN_SERVO_1);
  servo2.attach(PIN_SERVO_2);
  
  servo1.write(SERVO_NETRAL);
  servo2.write(SERVO_NETRAL);
  
  wdt_enable(WDTO_4S); // Watchdog 4 detik aman untuk siklus non-blocking
}

void loop() {
  wdt_reset();
  
  // Deteksi Perintah Kalibrasi Manual atau Cetak Log via Serial Terminal
  if (digitalRead(PIN_CALIB_BTN) == LOW) {
    jalankanWizardKalibrasi();
  }
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    if (cmd == 'D' || cmd == 'd') {
      printSystemDiagnostics();
    }
  }

  // MESIN STATUS UTAMA (FINITE STATE MACHINE)
  switch (currentState) {
    
    case STATE_INIT:
      Serial.println(F("[INIT] Memuat Data Kalibrasi EEPROM..."));
      EEPROM.get(EEPROM_ADDR_CALIB, calib);
      if (calculateCRC((uint8_t*)&calib, sizeof(CalibrationData) - 1) != calib.crc) {
        Serial.println(F("[WARN] CRC Gagal! Memuat nilai referensi pabrik standar."));
        // Default koordinat warna ternormalisasi teoritis
        calib.red[0] = 0.60; calib.red[1] = 0.20; calib.red[2] = 0.20;
        calib.green[0] = 0.20; calib.green[1] = 0.60; calib.green[2] = 0.20;
        calib.blue[0] = 0.20; calib.blue[1] = 0.20; calib.blue[2] = 0.60;
      }
      currentState = STATE_SELF_TEST;
      stateTimer = millis();
      break;

    case STATE_SELF_TEST: // Prioritas 2: Pengujian Mandiri Penuh Konveyor & Aktuator
      Serial.println(F("[TEST] Memulai Pengujian Mandiri Seluruh Komponen..."));
      
      servo1.write(SERVO_S1_KIRI); delay(400); wdt_reset();
      servo1.write(SERVO_NETRAL); delay(400); wdt_reset();
      servo2.write(SERVO_S2_KIRI); delay(400); wdt_reset();
      servo2.write(SERVO_S2_KANAN); delay(400); wdt_reset();
      servo2.write(SERVO_NETRAL); delay(400); wdt_reset();
      
      digitalWrite(PIN_LED_R, HIGH); delay(200); digitalWrite(PIN_LED_R, LOW);
      digitalWrite(PIN_LED_G, HIGH); delay(200); digitalWrite(PIN_LED_G, LOW);
      digitalWrite(PIN_LED_B, HIGH); delay(200); digitalWrite(PIN_LED_B, LOW);
      
      controlConveyor(true, true, 150); delay(500); controlConveyor(false);
      wdt_reset();
      
      int checkLDR = analogRead(PIN_LDR);
      if (checkLDR < 10 || checkLDR > 1015) {
        Serial.println(F("[ERROR] Pengujian Mandiri Gagal: Defek Sensor Cahaya!"));
        currentState = STATE_ERROR;
      } else {
        ambientBaseline = readFilteredLDR();
        // Prioritas 1: Adaptive Threshold (8% dari baseline cahaya sekitar)
        adaptiveThreshold = (int)(ambientBaseline * 0.08);
        if (adaptiveThreshold < 20) adaptiveThreshold = 20; // Batas minimal derau (noise)
        
        Serial.print(F("[READY] Baseline: ")); Serial.print(ambientBaseline);
        Serial.print(F(" | Threshold: ")); Serial.println(adaptiveThreshold);
        
        controlConveyor(true);
        samplingRetryCount = 0;
        conveyorRetryCount = 0;
        currentState = STATE_STANDBY;
      }
      break;

    case STATE_STANDBY:
      if ((ambientBaseline - readFilteredLDR()) > adaptiveThreshold) {
        debounceTimer = millis();
        currentState = STATE_OBJECT_DETECTED;
      }
      break;

    case STATE_OBJECT_DETECTED:
      if ((ambientBaseline - readFilteredLDR()) > adaptiveThreshold) {
        if (millis() - debounceTimer >= DEBOUNCE_DELAY) {
          controlConveyor(false);
          stateTimer = millis();
          currentState = STATE_READ_R;
        }
      } else {
        currentState = STATE_STANDBY;
      }
      break;

    // Prioritas 5: Pembacaan Warna Berbasis Mesin Status Non-blocking
    case STATE_READ_R:
      digitalWrite(PIN_LED_R, HIGH);
      if (millis() - stateTimer >= 80) { // Beri jeda propagasi cahaya tanpa delay()
        rawR = ambientBaseline - readFilteredLDR();
        if (rawR < 0) rawR = 0;
        digitalWrite(PIN_LED_R, LOW);
        stateTimer = millis();
        currentState = STATE_READ_G;
      }
      break;

    case STATE_READ_G:
      digitalWrite(PIN_LED_G, HIGH);
      if (millis() - stateTimer >= 80) {
        rawG = ambientBaseline - readFilteredLDR();
        if (rawG < 0) rawG = 0;
        digitalWrite(PIN_LED_G, LOW);
        stateTimer = millis();
        currentState = STATE_READ_B;
      }
      break;

    case STATE_READ_B:
      digitalWrite(PIN_LED_B, HIGH);
      if (millis() - stateTimer >= 80) {
        rawB = ambientBaseline - readFilteredLDR();
        if (rawB < 0) rawB = 0;
        digitalWrite(PIN_LED_B, LOW);
        currentState = STATE_CLASSIFY;
      }
      break;

    case STATE_CLASSIFY: {
      long totalIntensity = rawR + rawG + rawB;
      
      // Prioritas 1: Logika Percobaan Ulang Sampling (Hingga 3 Kali Percobaan)
      if (totalIntensity < 15) {
        samplingRetryCount++;
        Serial.print(F("[WARN] Intensitas rendah, sampling ulang ke-"));
        Serial.println(samplingRetryCount);
        if (samplingRetryCount >= 3) {
          Serial.println(F("[ERROR] Gagal membaca warna setelah 3 kali percobaan."));
          currentState = STATE_ERROR;
        } else {
          stateTimer = millis();
          currentState = STATE_READ_R; // Putar balik ke urutan pembacaan awal
        }
        break;
      }
      
      samplingRetryCount = 0; // Atur ulang penghitung jika berhasil lolos parameter
      float nR = (float)rawR / totalIntensity;
      float nG = (float)rawG / totalIntensity;
      float nB = (float)rawB / totalIntensity;
      
      // Prioritas 1: Klasifikasi Riil Menggunakan Pola Data Array EEPROM
      float distRed = sqrt(pow(nR - calib.red[0], 2) + pow(nG - calib.red[1], 2) + pow(nB - calib.red[2], 2));
      float distGreen = sqrt(pow(nR - calib.green[0], 2) + pow(nG - calib.green[1], 2) + pow(nB - calib.green[2], 2));
      float distBlue = sqrt(pow(nR - calib.blue[0], 2) + pow(nG - calib.blue[1], 2) + pow(nB - calib.blue[2], 2));
      
      float toleranceLimit = 0.25;
      ObjectData detectedObj;
      detectedObj.detectionTime = millis();
      
      if (distRed < distGreen && distRed < distBlue && distRed < toleranceLimit) {
        detectedObj.color = COLOR_RED;
        countRed++;
        Serial.println(F("[LOG] Hasil Klasifikasi: MERAH"));
      } else if (distGreen < distRed && distGreen < distBlue && distGreen < toleranceLimit) {
        detectedObj.color = COLOR_GREEN;
        countGreen++;
        Serial.println(F("[LOG] Hasil Klasifikasi: HIJAU"));
      } else if (distBlue < distRed && distBlue < distGreen && distBlue < toleranceLimit) {
        detectedObj.color = COLOR_BLUE;
        countBlue++;
        Serial.println(F("[LOG] Hasil Klasifikasi: BIRU"));
      } else {
        detectedObj.color = COLOR_UNKNOWN;
        countReject++;
        Serial.println(F("[LOG] Hasil Klasifikasi: TIDAK DIKENAL"));
      }
      
      totalObjectsProcessed++;
      currentState = STATE_ENQUEUE;
      break;
    }

    case STATE_ENQUEUE: {
      ObjectData tempObj;
      // Ambil elemen data objek terbaru dari kalkulasi sebelumnya
      if (countRed > 0 && totalObjectsProcessed == (countRed + countGreen + countBlue + countReject)) tempObj.color = COLOR_RED;
      if (countGreen > 0 && totalObjectsProcessed == (countRed + countGreen + countBlue + countReject)) tempObj.color = COLOR_GREEN;
      if (countBlue > 0 && totalObjectsProcessed == (countRed + countGreen + countBlue + countReject)) tempObj.color = COLOR_BLUE;
      if (countReject > 0 && totalObjectsProcessed == (countRed + countGreen + countBlue + countReject)) tempObj.color = COLOR_UNKNOWN;
      tempObj.detectionTime = millis();

      // Prioritas 2: Logika Pemulihan Overvlow Antrean (Queue Overflow Recovery)
      if (!enqueue(tempObj)) {
        Serial.println(F("[OVERFLOW] Antrean Penuh! Melakukan Penolakan Otomatis Objek..."));
        countReject++;
        controlConveyor(true);
        stateTimer = millis();
        currentState = STATE_SYNCHRONIZE; // Paksa jalan buang ke wadah reject tanpa dipilah servo
      } else {
        controlConveyor(true);
        stateTimer = millis();
        currentState = STATE_SYNCHRONIZE;
      }
      break;
    }

    case STATE_SYNCHRONIZE:
      // Prioritas 2: Penanganan Timeout Konveyor (Deteksi Objek Tersangkut/Macet)
      if (millis() - stateTimer >= TIMEOUT_CONVEYOR) {
        Serial.println(F("[TIMEOUT] Motor Konveyor Terhambat/Macet!"));
        conveyorRetryCount++;
        if (conveyorRetryCount >= 2) {
          Serial.println(F("[FATAL] Pemulihan Gagal. Menghentikan Kerja Mesin."));
          currentState = STATE_ERROR;
        } else {
          currentState = STATE_CONVEYOR_RECOVERY;
          stateTimer = millis();
        }
      } else {
        // Asumsi waktu tempuh fisik dari area sensor ke aktuator pemilah = 2.5 Detik
        if (millis() - stateTimer >= 2500) {
          controlConveyor(false);
          currentState = STATE_SORTING_ACTIVATE;
        }
      }
      break;

    case STATE_CONVEYOR_RECOVERY: // Prioritas 2: Mekanisme Pemulihan Motor Mundur-Maju (Reverse-Forward)
      Serial.println(F("[RECOVERY] Menjalankan Mundur Motor Konveyor..."));
      controlConveyor(true, false, 220); // Jalankan terbalik (Reverse)
      if (millis() - stateTimer >= 1500) {
        controlConveyor(false);
        delay(200);
        controlConveyor(true, true, 200); // Kembalikan maju
        stateTimer = millis();
        currentState = STATE_SYNCHRONIZE; // Coba sinkronisasi ulang
      }
      break;

    case STATE_SORTING_ACTIVATE: {
      ObjectData targetObj;
      if (dequeue(targetObj)) {
        servoTimer = millis();
        if (targetObj.color == COLOR_RED) {
          servo1.write(SERVO_S1_KIRI);
        } else if (targetObj.color == COLOR_GREEN) {
          servo2.write(SERVO_S2_KIRI);
        } else if (targetObj.color == COLOR_BLUE) {
          servo2.write(SERVO_S2_KANAN);
        }
        currentState = STATE_SORTING_WAIT;
      } else {
        // Jika antrean kosong di luar dugaan, kembali ke awal
        controlConveyor(true);
        currentState = STATE_STANDBY;
      }
      break;
    }

    case STATE_SORTING_WAIT:
      // Prioritas 2: Perlindungan Servo Macet & Batas Waktu (Servo Timeout)
      if (millis() - servoTimer >= TIMEOUT_SERVO) {
        servoRetryCount++;
        Serial.println(F("[WARN] Servo Macet atau Melampaui Batas Waktu Operasi!"));
        servo1.write(SERVO_NETRAL);
        servo2.write(SERVO_NETRAL);
        if (servoRetryCount >= 2) {
          currentState = STATE_ERROR;
        } else {
          delay(200);
          currentState = STATE_SORTING_ACTIVATE; // Ulangi sekali lagi aktuasi
        }
      } else {
        // Alokasikan waktu 1.2 detik pergerakan menyortir fisik sebelum netral kembali
        if (millis() - servoTimer >= 1200) {
          servo1.write(SERVO_NETRAL);
          servo2.write(SERVO_NETRAL);
          servoRetryCount = 0;
          
          // Sinkronisasi Adaptif Baseline Cahaya Lingkungan Sekitar
          ambientBaseline = readFilteredLDR();
          adaptiveThreshold = (int)(ambientBaseline * 0.08);
          if (adaptiveThreshold < 20) adaptiveThreshold = 20;

          controlConveyor(true);
          currentState = STATE_STANDBY;
        }
      }
      break;

    case STATE_ERROR:
      controlConveyor(false);
      servo1.write(SERVO_NETRAL);
      servo2.write(SERVO_NETRAL);
      digitalWrite(PIN_LED_R, LOW);
      digitalWrite(PIN_LED_G, LOW);
      digitalWrite(PIN_LED_B, LOW);
      
      // Indikator Error Berkedip Cepat Tanpa Mereset Watchdog, Memaksa Unit Auto-restart
      while (true) {
        digitalWrite(PIN_LED_ERR, HIGH);
        delay(100);
        digitalWrite(PIN_LED_ERR, LOW);
        delay(100);
      }
      break;
  }
}

// ==========================================
// TUNTUNAN KALIBRASI (Prioritas 4 & 1 Terintegrasi)
// ==========================================
void jalankanWizardKalibrasi() {
  controlConveyor(false);
  Serial.println(F("\n=========================================="));
  Serial.println(F("      WIZARD PROSEDUR KALIBRASI SISTEM     "));
  Serial.println(F("=========================================="));
  
  // 1. Ambil Sampel Kalibrasi Objek MERAH
  Serial.println(F("[STEP 1] Tempatkan Objek MERAH di bawah sensor..."));
  Serial.println(F("-> Tekan tombol kalibrasi kembali jika objek sudah siap."));
  while(digitalRead(PIN_CALIB_BTN) == HIGH) { wdt_reset(); }
  delay(500); wdt_reset();
  
  long rT = 0, gT = 0, bT = 0;
  // Ambil multi-sampling kecil untuk meningkatkan presisi kalibrasi
  for(int i=0; i<3; i++) {
    digitalWrite(PIN_LED_R, HIGH); delay(100); rT += (ambientBaseline - readFilteredLDR()); digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_G, HIGH); delay(100); gT += (ambientBaseline - readFilteredLDR()); digitalWrite(PIN_LED_G, LOW);
    digitalWrite(PIN_LED_B, HIGH); delay(100); bT += (ambientBaseline - readFilteredLDR()); digitalWrite(PIN_LED_B, LOW);
  }
  long totalRedObj = (rT/3) + (gT/3) + (bT/3);
  calib.red[0] = (float)(rT/3) / totalRedObj;
  calib.red[1] = (float)(gT/3) / totalRedObj;
  calib.red[2] = (float)(bT/3) / totalRedObj;
  Serial.println(F("[OK] Parameter Warna Merah Berhasil Dikunci."));
  
  // 2. Ambil Sampel Kalibrasi Objek HIJAU
  Serial.println(F("\n[STEP 2] Tempatkan Objek HIJAU di bawah sensor..."));
  Serial.println(F("-> Tekan tombol kalibrasi kembali jika objek sudah siap."));
  delay(1000); wdt_reset();
  while(digitalRead(PIN_CALIB_BTN) == HIGH) { wdt_reset(); }
  delay(500); wdt_reset();
  
  rT = 0; gT = 0; bT = 0;
  for(int i=0; i<3; i++) {
    digitalWrite(PIN_LED_R, HIGH); delay(100); rT += (ambientBaseline - readFilteredLDR()); digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_G, HIGH); delay(100); gT += (ambientBaseline - readFilteredLDR()); digitalWrite(PIN_LED_G, LOW);
    digitalWrite(PIN_LED_B, HIGH); delay(100); bT += (ambientBaseline - readFilteredLDR()); digitalWrite(PIN_LED_B, LOW);
  }
  long totalGreenObj = (rT/3) + (gT/3) + (bT/3);
  calib.green[0] = (float)(rT/3) / totalGreenObj;
  calib.green[1] = (float)(gT/3) / totalGreenObj;
  calib.green[2] = (float)(bT/3) / totalGreenObj;
  Serial.println(F("[OK] Parameter Warna Hijau Berhasil Dikunci."));

  // 3. Ambil Sampel Kalibrasi Objek BIRU
  Serial.println(F("\n[STEP 3] Tempatkan Objek BIRU di bawah sensor..."));
  Serial.println(F("-> Tekan tombol kalibrasi kembali jika objek sudah siap."));
  delay(1000); wdt_reset();
  while(digitalRead(PIN_CALIB_BTN) == HIGH) { wdt_reset(); }
  delay(500); wdt_reset();
  
  rT = 0; gT = 0; bT = 0;
  for(int i=0; i<3; i++) {
    digitalWrite(PIN_LED_R, HIGH); delay(100); rT += (ambientBaseline - readFilteredLDR()); digitalWrite(PIN_LED_R, LOW);
    digitalWrite(PIN_LED_G, HIGH); delay(100); gT += (ambientBaseline - readFilteredLDR()); digitalWrite(PIN_LED_G, LOW);
    digitalWrite(PIN_LED_B, HIGH); delay(100); bT += (ambientBaseline - readFilteredLDR()); digitalWrite(PIN_LED_B, LOW);
  }
  long totalBlueObj = (rT/3) + (gT/3) + (bT/3);
  calib.blue[0] = (float)(rT/3) / totalBlueObj;
  calib.blue[1] = (float)(gT/3) / totalBlueObj;
  calib.blue[2] = (float)(bT/3) / totalBlueObj;
  Serial.println(F("[OK] Parameter Warna Biru Berhasil Dikunci."));

  // Hitung Integritas Data Menggunakan Komputasi Checksum CRC-8
  calib.crc = calculateCRC((uint8_t*)&calib, sizeof(CalibrationData) - 1);
  EEPROM.put(EEPROM_ADDR_CALIB, calib);
  
  Serial.println(F("\n[SUKSES] Seluruh Data Disimpan Permanen ke EEPROM."));
  Serial.println(F("Melanjutkan Kembali Operasi Utama...\n"));
  
  delay(1000);
  controlConveyor(true);
}
