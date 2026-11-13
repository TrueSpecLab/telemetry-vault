
# 🔋 TRUESPEC LAB | Weighted Scorecard

A Python-based data visualization tool that calculates and renders a weighted scorecard for battery performance under extreme cold conditions (-18°C). 
This script takes raw multimeter readings from stress tests, applies a weighted scoring algorithm, and generates a high-resolution, dark-mode scorecard image (`weighted_scorecard.png`) suitable for video overlays or technical reports.

---

## 📊 How It Works

The script evaluates batteries based on four key metrics, normalizing raw voltage data into a 0-10 score.

### ✨ Features

| Metric | Weight | Description |
| :--- | :--- | :--- |
| **❄️ Stability** | **40%** | **Does it work when cold?**<br>Measures voltage sag under load at -18°C. A drop of 0.20V (The `FAILURE_THRESHOLD_V`) results in a score of 0. |
| **❤️ Recovery** | **30%** | **Does it survive permanently?**<br>Measures permanent voltage loss after the battery warms back up to room temperature. |
| **⚡ Overhead** | **20%** | **Raw Power Potential.**<br>Compares the battery's starting voltage against the highest voltage in the group. Higher voltage = higher score. |
| **💰 Value** | **10%** | **ROI.**<br>A subjective score (0-10) based on price-per-unit and reusability. |

### 📐The Math

Scores are calculated using a linear penalty model based on a **0.20V Failure Threshold**:

$$Score = 10 - \left( \frac{\text{Voltage Drop}}{0.20V} \times 10 \right)$$

*Example: A voltage drop of 0.10V (half the limit) results in a score of 5.0/10.*

---

## 📂 Project Structure

- `📁 figures`: Directory with generated graphs.
- `📁 fonts`: Directory for custom fonts used.
- `weighted_scorecard.png`: The final exported image of the table.

---

## 🚀 Setup & Execution

**1. Create & Activate Environment**:

```bash
# Create
python -m venv .venv

# Activate (Windows)
.venv\Scripts\activate

# Activate (Mac/Linux)
source .venv/bin/activate
```

**2. Install Dependencies**:
```bash
pip install -r requirements.txt
```

**3. Run the Script**:
```bash
python weighted_scorecard.py
```
