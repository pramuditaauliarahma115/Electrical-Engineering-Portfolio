# Sistem Prediksi Kelembapan Harian menggunakan algoritma NLMS

Sistem prediksi kelembapan harian menggunakan algoritma **Normalized Least Mean Square (NLMS)** berbasis Python. Model menerapkan adaptive filtering untuk memprediksi kelembapan berdasarkan data historis melalui preprocessing, pembentukan lag feature, dan optimasi parameter.

---

## Teknologi

**Programming Language**
- Python

**Libraries**
- NumPy
- Pandas
- Matplotlib
- Scikit-learn
- Numba

---

## Arsitektur Sistem

Diagram berikut menunjukkan alur prediksi kelembapan menggunakan algoritma NLMS.

<p align="center">
  <img src="image/Arsitektur_NLMS.png" width="1000">
</p>

---

## Hasil Implementasi

| Parameter | Hasil |
|-----------|-------:|
| Accuracy | **95.93%** |
| MAPE | **4.07%** |
| Parameter Optimal | **Epoch = 800, μ = 0.05, Lag = 50** |

<p align="center">
  <img src="images/Hasil_Implementasi.png" width="900">
</p>

---

## Kompetensi

- Adaptive Signal Processing
- Normalized Least Mean Square (NLMS)
- Time Series Prediction
- Python Programming
- Data Preprocessing
- Hyperparameter Tuning
- Data Visualization
