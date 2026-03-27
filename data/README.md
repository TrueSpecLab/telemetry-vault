# 🗄️ TRUESPEC LAB | Research Data Archive

> This directory contains the forensic records for every TrueSpec Lab report.

## 📂 Directory Structure

* `📁 ARCHIVE REPORT`: Real measurements, test data, and performance checks to see if the claims actually hold up.
* `📁 PRECISION VOLUME`: This volume focuses on technical deep-dives into calibration, component-level performance, and high-resolution data analysis.
* `📁 RESILIENCE REPORT`: These reports document sustained load tests, thermal drift, and worst-case scenarios.
* `📁 EFFICIENCY INDEX`: We analyze performance-to-waste ratios, thermal-to-performance bottlenecks, and idle power draw.
* `📁 templates`:
  * **📄 README.md**: Contains the "Metadata of the Test" template.
  * **📄 telemetry_v1.csv**: A standardized header file. Use this as a reference when setting up new data loggers to ensure compatibility with our analysis scripts.

### 🧱 Project Folder Hierarchy

> [!NOTE]
> Each experiment or "Lab Report" has its own dedicated folder.
> To maintain consistency across diverse tests, we adhere to the following **directory naming convention**:
> 
> `ID-[000]-[Short-Name]` (e.g., ID-001-VARTA-Super-Heavy-Duty-AA)

Inside each project folder, you will find:

* `📁 processed`: Cleaned data. Outliers removed, units normalized, and ready for plotting or Python analysis.
* `📁 raw_logs`: The "Untouched" output. Serial logs, raw `.txt` or `.csv` files straight from the sensors or data loggers.
* `📁 tech_specs`: Wiring diagrams, CAD cross-sections, or manufacturer data sheets.
* **📄 README.md**: Contains the "Metadata of the Test"—Ambient temperature, power source (Battery vs. USB), equipment list (Multimeters/Oscilloscopes used), and specific test conditions.

---

## 🛠️ How to Use This Data

1.  **Reproducibility**: Check the `tech_specs` folder first to verify the physical layer of the test.
2.  **Context**: Always read the project-specific `README.md` to understand the environmental variables (e.g., *22°C Ambient, 3.7V LiPo*).
3.  **Analysis**: We recommend using the files in the `processed` folder for graphing to avoid noise found in the `raw_logs`.

## ⚖️ License & Attribution

To balance open-source utility with brand protection, this vault uses a dual-license:

* **Software/Scripts:** Licensed under the [MIT License](LICENSE).
* **Telemetry & Research Assets:** Licensed under [Creative Commons Attribution 4.0 (CC-BY-4.0)](https://creativecommons.org/licenses/by/4.0/).

**Attribution Requirement:** If you use this data in a video, article, or project, you must credit **TrueSpec Lab** and link back to this repository.
