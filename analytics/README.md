# Smart Water Quality Monitoring System - Big Data Analytics

## 📚 Project Objective

This project develops an IoT-based Smart Water Quality Monitoring System.  
Using ESP32 and multiple sensors, it collects real-time water parameters from a reservoir and applies Big Data Analytics to detect trends, anomalies, and provide actionable insights.

---

## 🛰️ Data Collection Details

- **Device**: ESP32 + pH Sensor, TDS Sensor, Turbidity Sensor, Temperature Sensor
- **Collection Frequency**: Every 5 seconds
- **Sessions per Day**:
  - Morning: 06:00 – 07:59
  - Afternoon: 12:00 – 13:59
  - Evening: 18:00 – 19:59
- **Data Volume**:
  - 1440 readings/session × 3 sessions/day = **4320 readings/day**
  - 7 days = **30,240 readings**

All timestamps are localized to **Asia/Colombo timezone** (UTC+5:30).

---

## 🧮 Data Processing Steps

1. **Timestamp Formatting**:  
   All timestamps are parsed and timezone-converted properly.

2. **Session Classification**:  
   Each data point is tagged as 'Morning', 'Afternoon', or 'Evening' based on timestamp.

3. **Filtering**:  
   Only readings within the defined session windows are considered.

4. **Numeric Focus**:  
   Only numeric fields (`pH`, `TDS`, `Turbidity`, `Temperature`) are used for aggregation and statistical analysis.

---

## 📈 Statistical and Mathematical Analysis

### Descriptive Statistics
For each parameter:

- **Mean**:  
  $$ \text{Mean} = \frac{\sum_{i=1}^{n} x_i}{n} $$

- **Standard Deviation**:  
  $$ \sigma = \sqrt{\frac{1}{n-1} \sum_{i=1}^{n} (x_i - \text{Mean})^2} $$

Saved in: `basic_statistics.csv`

---

### Anomaly Detection
- **Unsafe pH**:  
  pH < 6.5 or pH > 8.5 → flagged
- **High Turbidity**:  
  Turbidity > 2 NTU → flagged

Saved separately in:
- `high_turbidity_anomalies.csv`
- `unsafe_ph_anomalies.csv`

---

### Correlation Analysis
The **Pearson Correlation Coefficient** between two parameters \( x \) and \( y \) is calculated as:

$$
r_{xy} = \frac{ \sum_{i=1}^{n} (x_i - \bar{x})(y_i - \bar{y}) }{ \sqrt{ \sum_{i=1}^{n} (x_i - \bar{x})^2 \sum_{i=1}^{n} (y_i - \bar{y})^2 } }
$$

where:

$$
r_{xy} = \text{Pearson correlation coefficient between } x \text{ and } y,
$$

$$
x_i, \, y_i = \text{individual sample values of variables } x \text{ and } y,
$$

$$
\bar{x} = \frac{1}{n} \sum_{i=1}^{n} x_i \quad \text{(mean of } x \text{ values)},
$$

$$
\bar{y} = \frac{1}{n} \sum_{i=1}^{n} y_i \quad \text{(mean of } y \text{ values)},
$$

$$
n = \text{total number of observations}.
$$


**Interpretation**:
- \( r = +1 \) → Perfect positive linear correlation
- \( r = -1 \) → Perfect negative linear correlation
- \( r \approx 0 \) → No linear correlation

Saved in: `correlation_matrix.csv` and visualized via `correlation_heatmap.png`.

---

### Trend Smoothing - Moving Average

We apply **Moving Average Smoothing** to reduce noise and better observe the trend in the water quality parameters.

The formula for the simple moving average (window size = \( N \)) is:

$$
S_t = \frac{1}{N} \sum_{i=t-N+1}^{t} x_i
$$

where:
- \( S_t \) is the smoothed value at time \( t \),
- \( x_i \) are the original sensor readings,
- \( N \) is the window size (here, \( N = 10 \) readings).

This helps in visualizing long-term trends by smoothing short-term fluctuations.
---

## 📂 Outputs Generated

| File | Description |
|:---|:---|
| `basic_statistics.csv` | Mean, Std, Min, Max for each parameter |
| `daily_session_mean.csv` | Mean value per day and session |
| `session_trends.csv` | Mean per session (Morning/Afternoon/Evening) |
| `high_turbidity_anomalies.csv` | High turbidity readings |
| `unsafe_ph_anomalies.csv` | Unsafe pH readings |
| `correlation_matrix.csv` | Pearson correlations |
| `correlation_heatmap.png` | Visual heatmap of correlations |
| `.png plots` | Original and smoothed trends for all parameters |
| `final_report.txt` | Full technical and mathematical report |

---

## 🧠 Key Observations

- **TDS and Turbidity** are moderately positively correlated.
- **Temperature** tends to peak during **afternoon sessions**.
- **pH** shows slight dips during **evening sessions**.
- **High Turbidity events** occur mostly in **evening sessions**, possibly due to environmental disturbances (e.g., runoff).

---
