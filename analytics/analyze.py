import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import os

# Step 1: Load the data
df = pd.read_json('analytics/collected_data.json')

# Step 2: Convert timestamp to datetime
df['timestamp'] = pd.to_datetime(df['timestamp'])

# Step 3: Basic Statistics
print("\n=== Basic Statistics ===")
stats = df.describe()
print(stats)

# Save basic statistics
if not os.path.exists('analytics/output'):
    os.makedirs('analytics/output')
stats.to_csv('analytics/output/basic_statistics.csv')

# Step 4: Daily Max/Min Analytics
df['date'] = df['timestamp'].dt.date
daily_max = df.groupby('date').max()
daily_min = df.groupby('date').min()

print("\n=== Daily Maximum Values ===")
print(daily_max)
daily_max.to_csv('analytics/output/daily_max.csv')

print("\n=== Daily Minimum Values ===")
print(daily_min)
daily_min.to_csv('analytics/output/daily_min.csv')

# Step 5: Anomaly Detection
high_turbidity = df[df['turbidity'] > 2]
unsafe_ph = df[(df['ph'] < 6.5) | (df['ph'] > 8.5)]

# Save anomalies to files
high_turbidity.to_csv('analytics/output/high_turbidity_anomalies.csv', index=False)
unsafe_ph.to_csv('analytics/output/unsafe_ph_anomalies.csv', index=False)

# Step 6: Correlation Matrix
correlation_matrix = df[['ph', 'tds', 'turbidity', 'temperature']].corr()
print("\n=== Correlation Matrix ===")
print(correlation_matrix)
correlation_matrix.to_csv('analytics/output/correlation_matrix.csv')

# Step 7: Moving Average Smoothing
window_size = 10
df['ph_smooth'] = df['ph'].rolling(window=window_size).mean()
df['tds_smooth'] = df['tds'].rolling(window=window_size).mean()
df['turbidity_smooth'] = df['turbidity'].rolling(window=window_size).mean()
df['temperature_smooth'] = df['temperature'].rolling(window=window_size).mean()

# Step 8: Visualization Functions
def plot_single(title, x, y, ylabel, color='blue', filename=None):
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.plot(x, y, color=color)
    ax.set_title(title)
    ax.set_xlabel('Timestamp')
    ax.set_ylabel(ylabel)
    ax.grid(True)
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d %H:%M:%S'))
    plt.xticks(rotation=45)
    plt.tight_layout()
    if filename:
        plt.savefig(filename)
    plt.show(block=False)

def plot_smoothed(title, x, y_orig, y_smooth, ylabel, color_orig='gray', color_smooth='blue', filename=None):
    fig, ax = plt.subplots(figsize=(12, 6))
    ax.plot(x, y_orig, alpha=0.5, label='Original', color=color_orig)
    ax.plot(x, y_smooth, label='Smoothed', linewidth=2, color=color_smooth)
    ax.set_title(title)
    ax.set_xlabel('Timestamp')
    ax.set_ylabel(ylabel)
    ax.legend()
    ax.grid(True)
    ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d %H:%M:%S'))
    plt.xticks(rotation=45)
    plt.tight_layout()
    if filename:
        plt.savefig(filename)
    plt.show(block=False)

# Step 9: Plotting
directory = 'analytics/output/'

plot_single('pH Over Time', df['timestamp'], df['ph'], 'pH', color='blue', filename=directory+'ph_over_time.png')
plot_single('TDS Over Time', df['timestamp'], df['tds'], 'TDS (ppm)', color='green', filename=directory+'tds_over_time.png')
plot_single('Turbidity Over Time', df['timestamp'], df['turbidity'], 'Turbidity (NTU)', color='purple', filename=directory+'turbidity_over_time.png')
plot_single('Temperature Over Time', df['timestamp'], df['temperature'], 'Temperature (Celsius)', color='red', filename=directory+'temperature_over_time.png')

plot_smoothed('Smoothed pH', df['timestamp'], df['ph'], df['ph_smooth'], 'pH', filename=directory+'smoothed_ph.png')
plot_smoothed('Smoothed TDS', df['timestamp'], df['tds'], df['tds_smooth'], 'TDS (ppm)', filename=directory+'smoothed_tds.png')
plot_smoothed('Smoothed Turbidity', df['timestamp'], df['turbidity'], df['turbidity_smooth'], 'Turbidity (NTU)', filename=directory+'smoothed_turbidity.png')
plot_smoothed('Smoothed Temperature', df['timestamp'], df['temperature'], df['temperature_smooth'], 'Temperature (Celsius)', filename=directory+'smoothed_temperature.png')

# Step 10: Generate a simple analysis report
with open('analytics/output/final_report.txt', 'w') as f:
    f.write("Smart Water Quality Monitoring System - Data Analysis Report\n\n")
    f.write("1. Basic Statistics (Mean, STD, Min, Max) saved in 'basic_statistics.csv'.\n")
    f.write("2. Daily Maximum Values saved in 'daily_max.csv'.\n")
    f.write("3. Daily Minimum Values saved in 'daily_min.csv'.\n")
    f.write("4. High Turbidity Events (>2 NTU) saved in 'high_turbidity_anomalies.csv'.\n")
    f.write("5. Unsafe pH Events (pH <6.5 or >8.5) saved in 'unsafe_ph_anomalies.csv'.\n")
    f.write("6. Correlation Matrix saved in 'correlation_matrix.csv'.\n")
    f.write("7. Plots (both original and smoothed) saved as PNG images.\n")
    f.write("\nThis analysis helps to visualize the trends, detect unsafe water conditions,\n")
    f.write("and understand interdependencies between various water quality parameters.\n")

print("\nAll analytics completed and saved in 'analytics/output/' folder.")