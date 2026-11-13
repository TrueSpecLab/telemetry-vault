import csv

# Set your expected min and max voltages based on the data
v_min = 0.05  # The lowest dip
v_max = 0.20  # The highest peak

pwm_values = []

with open('candle1.csv', 'r') as file:
    reader = csv.reader(file)
    for row in reader:
        try:
            # Assuming row[1] contains the voltage
            voltage = float(row[1])
            
            # 1. Normalize between 0.0 and 1.0
            normalized = (voltage - v_min) / (v_max - v_min)
            normalized = max(0.0, min(1.0, normalized)) # Clamp values
            
            # 2. Gamma correction (squaring helps mimic human eye perception)
            gamma_corrected = normalized ** 2
            
            # 3. Map to 12-bit PWM (0 - 4095)
            pwm = int(gamma_corrected * 4095)
            pwm_values.append(str(pwm))
        except ValueError:
            continue

# Write to a C++ header file
with open('candle_data.h', 'w') as file:
    file.write("const uint16_t candleData[] PROGMEM = {\n")
    file.write(", ".join(pwm_values))
    file.write("\n};\n")
    file.write(f"const int candleDataLength = {len(pwm_values)};\n")