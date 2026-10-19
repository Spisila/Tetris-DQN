from pathlib import Path

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

parent_directory = Path("logs/runs")
file_name = "training_metrics.csv"

csv_files = list(parent_directory.glob(f"**/{file_name}"))

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

  sns.lineplot(data=combined, 
               x="total_time", 
               y="pieces_placed_interval", 
               hue="Test"
              )
  

  plt.title("Evolução Temporal por Categoria")
  plt.xticks(rotation=45)
  plt.tight_layout()
  plt.show()

else :
  print("No dataframes")