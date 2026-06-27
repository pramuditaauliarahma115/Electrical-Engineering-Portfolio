#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <avr/pgmspace.h> 
#include <math.h> 
 
/* ================= HARDWARE ================= */ 
#define PWM_A 9 
#define PWM_B 10 
#define I_SENSE A0 
#define ZCD_PIN 2 
#define PWM_TOP 533 
 
/* ================= TIMING ================= */ 
#define FS_CTRL 2000.0f 
#define TS (1.0f/FS_CTRL) 
 
/* ================= CURRENT PI ================= */ 
float Kp = 0.5f; 
float Ki = 15.0f; 
 
/* ================= PLL ================= */ 
float Kp_pll = 35.0f; 
float Ki_pll = 300.0f; 
 
/* ================= GLOBAL ================= */ 
volatile uint16_t adc_raw = 0; 
volatile bool zc_flag = false; 
volatile bool inject_enable = false; 
 
85 
 
/* ================= STATE ================= */ 
float theta = 0.0f; 
float omega_nom = 2.0f * PI * 50.0f; 
float omega = omega_nom; 
float pll_int = 0.0f; 
float adc_offset = 512.0f; 
float adc_per_amp = 50.0f; 
float I_filt = 0.0f; 
#define ALPHA 0.05f 
 
float Iref_rms = 0.5f; 
float Iref_peak; 
float u_prev = 0.0f; 
float e_prev = 0.0f; 
 
/* ================= SIN TABLE (256 entries) 
================= */ 
const int16_t sine_table[] PROGMEM = { 
  0, 804, 1607, 2410, 3211, 4011, 4807, 5601, 6392, 
7179, 7961, 8739, 9511, 10278, 11038, 11792, 
  12539, 13278, 14009, 14732, 15446, 16150, 16845, 
17530, 18204, 18867, 19519, 20159, 20787, 21402, 22004, 
22594, 
  23169, 23731, 24278, 24811, 25329, 25831, 26318, 
26789, 27244, 27683, 28105, 28510, 28897, 29268, 29621, 
29955, 
  30272, 30571, 30851, 31113, 31356, 31580, 31785, 
31970, 32137, 32284, 32412, 32520, 32609, 32678, 32727, 
32757, 
 
86 
  32767, 32757, 32727, 32678, 32609, 32520, 32412, 
32284, 32137, 31970, 31785, 31580, 31356, 31113, 30851, 
30571, 
  30272, 29955, 29621, 29268, 28897, 28510, 28105, 
27683, 27244, 26789, 26318, 25831, 25329, 24811, 24278, 
23731, 
  23169, 22594, 22004, 21402, 20787, 20159, 19519, 
18867, 18204, 17530, 16845, 16150, 15446, 14732, 14009, 
13278, 
  12539, 11792, 11038, 10278, 9511, 8739, 7961, 7179, 
6392, 5601, 4807, 4011, 3211, 2410, 1607, 804, 
  0, -804, -1607, -2410, -3211, -4011, -4807, -5601, -6392, -7179, -7961, -8739, -9511, -10278, -11038, 
11792, 
  -12539, -13278, -14009, -14732, -15446, -16150, 
16845, -17530, -18204, -18867, -19519, -20159, -20787, -21402, -22004, -22594, 
  -23169, -23731, -24278, -24811, -25329, -25831, 
26318, -26789, -27244, -27683, -28105, -28510, -28897, -29268, -29621, -29955, 
  -30272, -30571, -30851, -31113, -31356, -31580, 
31785, -31970, -32137, -32284, -32412, -32520, -32609, -32678, -32727, -32757, 
  -32767, -32757, -32727, -32678, -32609, -32520, 
32412, -32284, -32137, -31970, -31785, -31580, -31356, -31113, -30851, -30571, 
  -30272, -29955, -29621, -29268, -28897, -28510, 
28105, -27683, -27244, -26789, -26318, -25831, -25329, -24811, -24278, -23731, 
 
87 
  -23169, -22594, -22004, -21402, -20787, -20159, 
19519, -18867, -18204, -17530, -16845, -16150, -15446, -14732, -14009, -13278, 
  -12539, -11792, -11038, -10278, -9511, -8739, 
7961, -7179, -6392, -5601, -4807, -4011, -3211, -2410, -1607, -804 
}; 
 
inline float fast_sin(float th) { 
  // Mapping 0..2pi ke 0..255 
  uint8_t idx = (uint8_t)(th * 40.743665f);  
  return 
(float)((int16_t)pgm_read_word(&sine_table[idx])) / 
32767.0f; 
} 
 
/* ================= SETUP ================= */ 
void setupPWM() { 
  pinMode(PWM_A, OUTPUT); 
  pinMode(PWM_B, OUTPUT); 
  TCCR1A = 0; TCCR1B = 0; 
  TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << 
WGM11); 
  TCCR1B |= (1 << WGM13) | (1 << CS10); 
  ICR1 = PWM_TOP; 
} 
 
void setupTimer2() { 
  TCCR2A = 0; TCCR2B = 0; 
  TCCR2A |= (1 << WGM21); 
  TCCR2B |= (1 << CS22); // Prescaler 64 
 
88 
  OCR2A = 124;           // ~2kHz 
  TIMSK2 |= (1 << OCIE2A); 
} 
 
void setup() { 
  Iref_peak = Iref_rms * 1.4142f; 
  setupPWM(); 
  pinMode(ZCD_PIN, INPUT); 
  attachInterrupt(digitalPinToInterrupt(ZCD_PIN), 
[]() { 
    zc_flag = true; 
  }, RISING); 
  setupTimer2(); 
  sei(); 
} 
 
void loop() { 
  adc_raw = analogRead(I_SENSE); 
} 
 
/* ================= CONTROL LOOP (2kHz) 
================= */ 
ISR(TIMER2_COMPA_vect) { 
  float sin_th = fast_sin(theta); 
  float cos_th = fast_sin(theta + PI / 2.0f); 
 
  /* ===== PLL Logic ===== */ 
  float pll_err = 0.0f; 
  if (zc_flag) { 
    pll_err = -sin_th;  
    zc_flag = false; 
 
89 
  } 
  pll_int += Ki_pll * pll_err * TS; 
  omega = omega_nom + Kp_pll * pll_err + pll_int; 
  theta += omega * TS; 
  if (theta > 2.0f * PI) theta -= 2.0f * PI; 
 
  /* ===== Lock Detection ===== */ 
  static uint16_t lock_cnt = 0; 
  if (fabs(pll_err) < 0.05f) { 
    if (lock_cnt < 400) lock_cnt++; 
  } else { 
    lock_cnt = 0; 
  } 
  if (lock_cnt > 300) inject_enable = true; 
 
  /* ===== Current Measurement ===== */ 
  float I_meas = ((float)adc_raw - adc_offset) / 
adc_per_amp; 
  I_filt += ALPHA * (I_meas - I_filt); 
 
  /* ===== Current Control (PI) ===== */ 
  float I_ref = inject_enable ? Iref_peak * sin_th : 
0.0f; 
  float e = I_ref - I_filt; 
  float u = u_prev + Kp * (e - e_prev) + Ki * TS * 
e; 
 
  // Anti-windup & Saturation 
  if (u > 0.95f) u = 0.95f; 
  if (u < 0.0f) u = 0.0f; 
  u_prev = u; 
 
90 
  e_prev = e; 
 
  /* ===== Unipolar SPWM Output ===== */ 
  uint16_t duty = (uint16_t)(fabsf(sin_th) * u * 
PWM_TOP); 
  if (sin_th >= 0) { 
    OCR1A = duty; OCR1B = 0; 
  } else { 
    OCR1A = 0; OCR1B = duty; 
  } 
}