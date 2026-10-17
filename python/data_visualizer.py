
import pandas as pd

import matplotlib.pyplot as plt
import seaborn as sns

df = pd.read_csv("logs/training_metrics.csv")

sns.set_theme(style="darkgrid")
plt.figure(figsize=(10, 6))

sns.lineplot(data=df, x="episode_interval", y="lines_per_piece")
plt.title("Evolução Temporal por Categoria")
plt.xticks(rotation=45)
plt.tight_layout()
plt.show()