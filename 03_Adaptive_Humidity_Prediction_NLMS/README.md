# Adaptive Humidity Prediction using NLMS

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

![Architecture](images/Arsitektur_NLMS.png)

---

## Hasil Implementasi

Model berhasil melakukan prediksi kelembapan menggunakan adaptive learning dengan hasil terbaik:

- **Accuracy : 95.93%**
- **MAPE : 4.07%**
- **Best Parameter :** Epoch = 800, μ = 0.05, Lag = 50

![Prediction](images/Hasil_Prediksi.png)

---

## Kompetensi

- Adaptive Signal Processing
- Normalized Least Mean Square (NLMS)
- Time Series Prediction
- Python Programming
- Data Preprocessing
- Hyperparameter Tuning
- Data Visualization
