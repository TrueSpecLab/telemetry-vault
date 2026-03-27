# 🛠️ TRUESPEC LAB | Data Processing Scripts

This directory contains the Python utilities used by TrueSpec Lab to process, clean, and normalize raw hardware telemetry.

## 🎯 Purpose
Raw sensor data—whether from oscilloscopes, digital multimeters, thermal cameras, or serial monitors—is rarely formatted uniformly. The scripts in this folder automate the pipeline, taking messy outputs from the `raw_logs` directories and converting them into our standardized `telemetry_v1.csv` format inside the `processed` directories.

## 🧰 Standard Toolset
*(As the lab tests new hardware, specific additional scripts will be added here.)*

* **`weighted_scorecard.py`**: A Python-based data visualization tool that calculates and renders a weighted scorecard
* *(More Coming Soon)*
  
## 🚀 Usage Guide
To ensure relative file paths work correctly, always run these scripts from the **root** of the `telemetry-vault` repository.

**Example execution:**

`python .\scripts\weighted_scorecard\weighted_scorecard.py`


## 🤝 Contributing Parsers
Do you use a specific piece of test equipment (e.g., a Rigol oscilloscope, a Mooshimeter, or a custom INA219 logger) and want to share your data-cleaning script? We welcome Pull Requests! 

Please ensure your script's output complies with the `templates/telemetry_v1.csv` header standard before submitting.

---
**⚖️ License:** All scripts in this directory are open-source under the [MIT License](../LICENSE).
