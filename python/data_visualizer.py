from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# parent_directory = Path("logs")
# file_name = "training_metrics.csv"

# csv_files = list(parent_directory.glob(f"**/{file_name}"))

dir_path = Path("logs/2026-10-20_20-12-57")

# Find only .csv files in the main folder
csv_files = [item for item in dir_path.glob('*.csv') if item.is_file()]

data_frames = []

identifier = 0

for path in csv_files :

  name = f"Test {identifier}"

  identifier += 1

  df = pd.read_csv(path)

  df["Test"] = name
  data_frames.append(df)

if data_frames :

  combined = pd.concat(data_frames, ignore_index=True)
  
  sns.set_theme(style="darkgrid")

  plt.figure(figsize=(10,6))

  combined["total_lines"] = \
    combined["single_clears"] + \
    combined["double_clears"] * 2 + \
    combined["triple_clears"] * 3 + \
    combined["tetris_clears"] * 4 

  combined["lines_per_piece"] = combined["total_lines"].diff() / combined["pieces_placed"].diff()

  combined["smooth_lines_per_piece"] = combined["lines_per_piece"].rolling(window=25, center=True).mean()

  sns.lineplot(data=combined, 
               x="steps", 
               y="smooth_lines_per_piece", 
               hue="Test"
              )
  

  plt.title("Evolução Temporal por Categoria")
  plt.xticks(rotation=45)
  plt.tight_layout()
  plt.show()

else :
  print("No dataframes")