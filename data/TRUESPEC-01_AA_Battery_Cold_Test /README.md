# 🔬 RESILIENCE REPORT [ID: 001]: VARTA Super Heavy Duty AA
> **Video Report:** [Link to the YouTube Video]
> **Status:** `PUBLISHED`
> **Date:** [Month, Year]

## 🎯 Primary Objective
[One sentence explaining what you are testing and why. e.g., "To determine the actual voltage sag of Varta High Energy AA batteries when subjected to a 250mA continuous load at -18°C."]

## ⚙️ Hardware Under Test (HUT)
* **Manufacturer:** [Name]
* **Model/Component:** [Model Number]
* **Batch/Serial Number:** [If applicable]
* **Claimed Spec:** [What the box says]

## 🧪 Test Bench Setup
To ensure reproducibility, the following environmental controls and telemetry equipment were utilized:

| Category | Specifics |
| :--- | :--- |
| **Ambient Temperature** | [e.g., -18°C ± 1°C] |
| **Humidity** | [e.g., 45% Non-condensing] |
| **Load Device** | [e.g., Electronic Load Tester ZK-4KX] |
| **Telemetry Sensors** | [e.g., INA219 (Current/Voltage), DS18B20 (Thermal)] |
| **Data Logging Rate** | [e.g., 1 Sample / 500ms] |

## 📐 Methodology
1. [Step 1: e.g., Battery was acclimated to -18°C for 24 hours.]
2. [Step 2: e.g., A constant current load of 250mA was applied.]
3. [Step 3: e.g., Telemetry was logged until voltage dropped below 0.8V.]

## 📊 Data Dictionary
The `raw_logs` and `processed` folders contain the telemetry data. Here is how to read the CSV files:
* `timestamp_ms`: Time elapsed since test start (in milliseconds).
* `voltage_v`: Measured voltage drop across the component.
* `current_ma`: Current draw in milliamps.
* `temp_c`: Surface temperature of the component in Celsius.
* `state`: The current test phase (e.g., `idle`, `load_active`, `recovery`).
