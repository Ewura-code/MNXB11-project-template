from collections import defaultdict

# Read the cleaned Lund file
input_file = "smhi-opendata_1_53430_20261007_164857_Lund.csv" 
output_file = "lund_tm_1780-2022.dat"

daily_temps = defaultdict(list)

with open(input_file, 'r') as f:
    for line in f:
        parts = line.strip().split(';')
        if len(parts) >= 3:
            datetime_str = parts[0]  # e.g., "1944-07-01;07:00:00"
            date_part = datetime_str.split(' ')[0]  # "1944-07-01"
            year, month, day = map(int, date_part.split('-'))
            
            try:
                temp = float(parts[2])
                daily_temps[(year, month, day)].append(temp)
            except ValueError:
                continue

# Write out in Uppsala .dat format: Year Month Day MeanTemp MeanTemp SourceID
with open(output_file, 'w') as f:
    for (year, month, day), temps in sorted(daily_temps.items()):
        if temps:
            mean_temp = sum(temps) / len(temps)
            # Format: Year Month Day Temp TempCorr Source
            f.write(f"{year} {month} {day} {mean_temp:.2f} {mean_temp:.2f} 1\n")

print(f"Generated {output_file} successfully!")