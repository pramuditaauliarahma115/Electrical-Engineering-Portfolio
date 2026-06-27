import numpy as np
import pandas as pd
from sklearn.preprocessing import StandardScaler
import matplotlib.pyplot as plt
import os
from numba import njit
# ======================================================
# 0. GLOBAL CONFIGURATION
# ======================================================
FILE_PATH = r"E:\UNSOED\SEMESTER 7\adaptif\humidity-only.csv"
# --- CHIRP SETTINGS ---
CHIRP_SR = 44100 # Sampling Rate
CHIRP_DURATION = 1.0 # Seconds
CHIRP_F0 = 100 # Start Frequency
CHIRP_F1 = 2500 # End Frequency
# ======================================================
# 1. LOAD DATA + SMOOTHING + NORMALISASI (HUMIDITY)
# ======================================================
if not os.path.exists(FILE_PATH):
print(f"Error: File tidak ditemukan di {FILE_PATH}")
y_raw = np.zeros(100)
scaler = StandardScaler()
y_norm = np.zeros(100)
y_smooth = np.zeros(100)
else:
df = pd.read_csv(FILE_PATH)
y_raw = df.iloc[:, 0].values
WINDOW_SMOOTH = 3
y_smooth = pd.Series(y_raw).rolling(
WINDOW_SMOOTH, center=True, min_periods=1
).mean().values
scaler = StandardScaler()
y_norm = scaler.fit_transform(y_smooth.reshape(-1, 1)).ravel()
# ======================================================
# 2. HELPER FUNCTIONS (METRICS)
# ======================================================
def create_lag_dataset(series, lag):
X, y = [], []
for i in range(lag, len(series)):
X.append(series[i - lag:i])
y.append(series[i])
return np.array(X), np.array(y)
def mse(a, b): return np.mean((a - b) ** 2)
def mae(a, b): return np.mean(np.abs(a - b))
def rmse(a, b): return np.sqrt(mse(a, b))
def mape(y_true, y_pred):
eps = 1e-6
return np.mean(np.abs((y_true - y_pred) / (y_true + eps))) *
100
def accuracy_from_mape(y_true, y_pred):
"""
Akurasi (%) didefinisikan sebagai 100 - MAPE.
Dipotong minimum 0 agar tidak negatif.
"""
m = mape(y_true, y_pred)
acc = 100.0 - m
return max(0.0, acc)
# ======================================================
# 3. NLMS TRAINING (NUMBA OPTIMIZED)
# ======================================================
@njit
def nlms_train_numba(X, y, epoch, mu, w0):
np.random.seed(42)
w = w0.copy()
mse_history = []
eps = 1e-6
N = len(X)
n_features = X.shape[1]
for _ in range(epoch):
idx = np.random.permutation(N)
for i in idx:
# Predict
pred = 0.0
for j in range(n_features):
pred += w[j] * X[i, j]
error = y[i] - pred
# Norm
norm = 0.0
for j in range(n_features):
norm += X[i, j] * X[i, j]
norm += eps
# Update weights
step = (mu / norm) * error
for j in range(n_features):
w[j] += step * X[i, j]
# MSE Calculation per epoch
mse_sum = 0.0
for i in range(N):
p = 0.0
for j in range(n_features):
p += w[j] * X[i, j]
mse_sum += (y[i] - p) ** 2
mse_history.append(mse_sum / N)
return w, np.array(mse_history)
# ======================================================
# 4. PLOTTING HELPERS
# ======================================================
def plot_convergence(mse_history, title="Convergence",
filename="convergence.png"):
plt.figure(figsize=(10, 4))
plt.plot(mse_history, color='blue', linewidth=1.5)
plt.title(title)
plt.xlabel("Epoch")
plt.ylabel("MSE (Mean Squared Error)")
plt.yscale("log")
plt.grid(True, which="both", ls="--", alpha=0.6)
plt.tight_layout()
plt.savefig(filename, dpi=300)
print(f"Convergence plot saved: {filename}")
plt.show()
def plot_raw_smooth_pred(hari_ke, y_test_real, pred_real):
plt.figure(figsize=(14, 6))
plt.plot(np.arange(1, len(y_raw) + 1), y_raw,
label="Raw Data (Noisy)", alpha=0.5, color='gray')
plt.plot(np.arange(1, len(y_smooth) + 1), y_smooth,
label="Smoothened Target", alpha=0.8, color='blue')
plt.plot(hari_ke, pred_real, label="Predicted (NLMS)",
linestyle="--", color="red")
plt.xlabel("Time Index")
plt.ylabel("Humidity")
plt.title("Humidity Prediction")
plt.legend()
plt.grid()
plt.savefig("pred_vs_raw_smooth.png", dpi=300)
print("Grafik disimpan: pred_vs_raw_smooth.png")
plt.show()
def plot_error_signal(error, title="Error Signal e(n)",
filename="error_plot.png"):
plt.figure(figsize=(10, 4))
plt.plot(error, color='red', linewidth=1)
plt.title(title)
plt.xlabel("Sample Index")
plt.ylabel("Error")
plt.grid(True, alpha=0.6)
plt.tight_layout()
plt.savefig(filename, dpi=300)
print(f"Error plot saved: {filename}")
plt.show()
def plot_chirp_full(t, true_sig, pred_sig,
filename="chirp_full_sweep.png"):
"""
Plots only the full 0s to 1s graph without zooming.
"""
plt.figure(figsize=(12, 6))
# Plot Full Sweep
plt.plot(t, true_sig, label="True Signal", color='black',
alpha=0.3)
plt.plot(t, pred_sig, label="NLMS Prediction",
color='red', linestyle='--', alpha=0.8)
plt.title(f"Full Frequency Sweep ({CHIRP_F0}Hz -
{CHIRP_F1}Hz)")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.legend(loc="upper right")
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.savefig(filename, dpi=300)
print(f"Chirp Plot saved: {filename}")
plt.show()
# ======================================================
# 5. CHIRP GENERATOR
# ======================================================
def chirp_generation(f0, f1, duration, sr):
t = np.linspace(0.0, duration, int(sr * duration),
endpoint=False)
k = (f1 - f0) / duration
phase = 2.0 * np.pi * (f0 * t + 0.5 * k * t**2)
signal = np.sin(phase).astype(np.float32)
return t, signal
# ======================================================
# 6. FEATURE A: MANUAL TUNING (HUMIDITY)
# ======================================================
def run_manual_tuning(epoch, mu, lag):
print("\n=== MODE MANUAL (NLMS + SMOOTHING) ===")
X, y = create_lag_dataset(y_norm, lag)
split = int(0.9 * len(X))
X_train, X_test = X[:split], X[split:]
y_train, y_test = y[:split], y[split:]
rng = np.random.default_rng(42)
w0 = rng.normal(0.0, 1e-3, size=X_train.shape[1]) #
inisialisasi white noise
w0)
# TRAIN NLMS
w, mse_history = nlms_train_numba(X_train, y_train, epoch, mu,
# PLOT KONVERGENSI
plot_convergence(
mse_history,
title=f"Manual Convergence (e={epoch}, mu={mu},
lag={lag})",
filename="convergence_manual.png"
)
# PREDIKSI NORMAL SCALE
pred_norm = X_test @ w
# KEMBALIKAN KE SKALA ASLI
pred_real = scaler.inverse_transform(pred_norm.reshape(-1,
1)).ravel()
y_test_real = scaler.inverse_transform(y_test.reshape(-1,
1)).ravel()
start_index = split + lag
y_test_raw_slice = y_raw[start_index: start_index +
len(pred_real)]
mse_smooth_val = mse(y_test_real, pred_real)
mse_r = mse(y_test_raw_slice, pred_real)
mape_r = mape(y_test_raw_slice, pred_real)
acc_r = accuracy_from_mape(y_test_raw_slice, pred_real)
print(f"MSE vs Smoothed Target: {mse_smooth_val:.4f}")
print(f"MSE vs Raw Data: {mse_r:.4f}")
print(f"MAPE vs Raw Data: {mape_r:.2f}%")
print(f"Akurasi Prediksi (100-MAPE): {acc_r:.2f}%")
# ====== PLOT KESELURUHAN DATA ======
hari_ke = np.arange(start_index + 1, start_index + 1 +
len(y_test_real))
plot_raw_smooth_pred(hari_ke, y_test_real, pred_real)
# ====== ERROR e(n) ======
error_signal = y_test_raw_slice - pred_real
plot_error_signal(
error_signal,
title="Error Signal e(n) - Manual Tuning (vs Raw)",
filename="error_manual.png"
)
# ======================================================
# PREDIKSI HARI BERIKUTNYA (NEXT-DAY FORECAST)
# ======================================================
last_lag_window = y_norm[-lag:] # window
terakhir (lag sampel terakhir)
next_pred_norm = np.dot(last_lag_window, w) # prediksi
dalam skala normal
next_pred_real =
scaler.inverse_transform([[next_pred_norm]])[0, 0]
print(f"\nPrediksi kelembaban untuk hari berikutnya:
{next_pred_real:.2f}%")
# ======================================================
# 7. FEATURE B: GRID TUNING (HUMIDITY)
# ======================================================
def run_grid_tuning_humidity():
print("\n=== MODE GRID TUNING (HUMIDITY) ===")
epochs_list = [500, 800, 1200, 1500]
mu_list = [0.001, 0.01, 0.05, 0.1]
lag_list = [10, 20, 30, 40, 50]
results = []
rng = np.random.default_rng(42)
# kalau scaler ada scale_
try:
std_real = scaler.scale_[0]
except Exception:
std_real = 1.0
print(f"{'Epoch':<6} {'Mu':<6} {'Lag':<4} {'MSE(Smooth)':<12}
"
f"{'MSE(Raw)':<12} {'MAPE(%)':<8}")
print("-" * 55)
for epoch in epochs_list:
for mu in mu_list:
for lag in lag_list:
X, y = create_lag_dataset(y_norm, lag)
if len(X) < 10:
continue
split = int(0.9 * len(X))
X_train, X_test = X[:split], X[split:]
y_train, y_test = y[:split], y[split:]
mu, w0)
w0 = rng.normal(0.0, 1e-3, size=X_train.shape[1])
w, _ = nlms_train_numba(X_train, y_train, epoch,
(std_real ** 2)
# Prediction in Normal Scale
pred_norm = X_test @ w
mse_smooth_val = mse(y_test, pred_norm) *
# Prediction in Real Scale (for MSE Raw & MAPE)
pred_real =
scaler.inverse_transform(pred_norm.reshape(-1, 1)).ravel()
# Get the Raw (Noisy) Slice
start_index = split + lag
y_test_raw_slice = y_raw[start_index: start_index
+ len(pred_real)]
mse_raw_val = mse(y_test_raw_slice, pred_real)
mape_val = mape(y_test_raw_slice, pred_real)
print(f"{epoch:<6} {mu:<6} {lag:<4} "
f"{mse_smooth_val:<12.4f}
{mse_raw_val:<12.4f} {mape_val:<8.2f}")
results.append([epoch, mu, lag, mse_smooth_val,
mse_raw_val, mape_val])
df_res = pd.DataFrame(
results, columns=["Epoch", "Mu", "Lag", "MSE_Smooth",
"MSE_Raw", "MAPE"]
)
df_res.to_excel("grid_results_humidity.xlsx", index=False)
if df_res.empty:
return
# Best is chosen based on MSE Smooth (Training Target)
best = df_res.loc[df_res["MSE_Smooth"].idxmin()]
print("\n=== BEST HUMIDITY MODEL ===")
print(best)
# Retrain Best
b_epoch, b_mu, b_lag = int(best["Epoch"]), float(best["Mu"]),
int(best["Lag"])
Xb, yb = create_lag_dataset(y_norm, b_lag)
split = int(0.9 * len(Xb))
X_train, X_test = Xb[:split], Xb[split:]
y_train, y_test = yb[:split], yb[split:]
w0 = rng.normal(0.0, 1e-3, size=X_train.shape[1])
w_best, mse_hist = nlms_train_numba(X_train, y_train, b_epoch,
b_mu, w0)
# Plot Convergence Rate for Best Humidity Model
plot_convergence(
mse_hist,
title="Grafik Konvergensi - Best Humidity Model",
filename="conv_best_humidity.png"
)
pred_norm = X_test @ w_best
pred_real = scaler.inverse_transform(pred_norm.reshape(-1,
1)).ravel()
y_test_real = scaler.inverse_transform(y_test.reshape(-1,
1)).ravel()
start_idx = split + b_lag
hari_ke = np.arange(start_idx + 1, start_idx + 1 +
len(y_test_real))
plot_raw_smooth_pred(hari_ke, y_test_real, pred_real)
# Error & akurasi untuk best model vs raw
y_test_raw_slice = y_raw[start_idx: start_idx +
len(pred_real)]
mape_best = mape(y_test_raw_slice, pred_real)
acc_best = accuracy_from_mape(y_test_raw_slice, pred_real)
print(f"\nMAPE Best Model vs Raw: {mape_best:.2f}%")
print(f"Akurasi Best Model (100-MAPE): {acc_best:.2f}%")
error_best = y_test_raw_slice - pred_real
plot_error_signal(
error_best,
title="Error Signal e(n) - Best Humidity Model (vs Raw)",
filename="error_best_humidity.png"
)
# ======================================================
# PREDIKSI HARI BERIKUTNYA (NEXT-DAY FORECAST)
# ======================================================
last_lag_window = y_norm[-b_lag:]
next_pred_norm = np.dot(last_lag_window, w_best)
next_pred_real =
scaler.inverse_transform([[next_pred_norm]])[0, 0]
print(f"\nPrediksi kelembaban untuk hari berikutnya (Best
Model): {next_pred_real:.2f}%")
# ======================================================
# 8. FEATURE C: FAST GRID TUNING (CHIRP)
# ======================================================
def run_grid_tuning_chirp():
print(f"\n=== FAST GRID TUNING: CHIRP SIGNAL (SR={CHIRP_SR}
Hz) ===")
# 1. Generate Data
t, sig = chirp_generation(CHIRP_F0, CHIRP_F1, CHIRP_DURATION,
CHIRP_SR)
sig = sig.ravel()
print(f"Generated Chirp: {len(sig)} samples.")
# 2. Grid Search Parameters
epochs_grid = [100, 500]
mu_list = [0.01, 0.05, 0.1, 0.2]
lag_list = [20, 40, 60]
results = []
rng = np.random.default_rng(123)
# Removed MAPE from print headers
print(f"{'Epoch':<6} {'Mu':<6} {'Lag':<4} {'MSE':<12}")
print("-" * 35)
for epoch in epochs_grid:
for mu in mu_list:
for lag in lag_list:
X, y = create_lag_dataset(sig, lag)
if len(X) < 10: continue
# Split 90/10 for validation
split = int(0.9 * len(X))
X_train, X_test = X[:split], X[split:]
y_train, y_test = y[:split], y[split:]
w0 = rng.normal(0.0, 1e-3, size=X_train.shape[1])
mu, w0)
# Training
w, _ = nlms_train_numba(X_train, y_train, epoch,
# Validation
pred_test = X_test @ w
mse_val = np.mean((y_test - pred_test)**2)
# REMOVED MAPE Calculation here
print(f"{epoch:<6} {mu:<6} {lag:<4}
{mse_val:<12.6f}")
results.append([epoch, mu, lag, mse_val])
df_res = pd.DataFrame(results, columns=["Epoch", "Mu", "Lag",
"MSE"])
df_res.to_excel("chirp_grid_results.xlsx", index=False)
if df_res.empty: return
# 3. Pick Winner
best = df_res.loc[df_res["MSE"].idxmin()]
print("\n=== BEST CHIRP PARAMETERS ===")
print(best)
b_mu = float(best["Mu"])
b_lag = int(best["Lag"])
b_epoch = int(best["Epoch"])
# 4. FINAL RETRAINING (Using Best Epoch)
print(f"\nRetraining best model with BEST EPOCH ({b_epoch})
...")
X_full, y_full = create_lag_dataset(sig, b_lag)
split = int(0.9 * len(X_full))
X_train = X_full[:split]
y_train = y_full[:split]
w0 = rng.normal(0.0, 1e-3, size=X_train.shape[1])
w_best, mse_hist = nlms_train_numba(X_train, y_train, b_epoch,
b_mu, w0)
# Plot Convergence Rate for Best Chirp Model
plot_convergence(mse_hist, title=f"Chirp Final Convergence
(e={b_epoch})", filename="conv_chirp_final.png")
# 5. PREDICT ON FULL DURATION (0s to 1s)
pred_full = X_full @ w_best
t_full = t[b_lag:]
final_mse = np.mean((y_full - pred_full)**2)
# REMOVED Final MAPE print
print(f"Final Full-Signal MSE: {final_mse:.8f}")
plot_chirp_full(t_full, y_full, pred_full,
filename="chirp_full_sweep.png")
# ======================================================
# 9. MAIN MENU
# ======================================================
if name == " main ":
print("\n============================")
print(" PROGRAM NLMS ADAPTIF")
print("============================")
print("1. Manual Tuning (Humidity)")
print("2. Grid Tuning (Humidity)")
print("3. Grid Tuning (Chirp Signal)")
print(f" (SR={CHIRP_SR}, Freq={CHIRP_F0}-{CHIRP_F1}Hz)")
print("============================")
mode = input("Pilih mode [1/2/3]: ")
if mode == "1":
try:
e = int(input("Epoch: "))
m = float(input("Mu: "))
l = int(input("Lag: "))
run_manual_tuning(e, m, l)
except ValueError:
print("Invalid input.")
elif mode == "2":
run_grid_tuning_humidity()
elif mode == "3":
run_grid_tuning_chirp()
else:
print("Mode tidak valid.")
