import pandas as pd
import os

from python.episode_logger import Run
from python.config import from_cli
from python.training_loop import train

config = from_cli()

run = Run(config, {})

training_folder = run.file_name


def objective(trial):

    # Parameters
    lr = trial.suggest_float("lr_start", 1e-4, 1e-3, log=True)
    
    batch_size = trial.suggest_categorical("batch_size", [128, 256])
    buffer_capacity = trial.suggest_categorical("buffer_capacity", [500_000, 1_000_000])
    train_until = trial.suggest_categorical("train_until", [20_000, 25_000, 30_000])

    train(
        run_=run,
        lr_start=lr, 
        batch_size=batch_size, 
        buffer_capacity=buffer_capacity,
        train_until=train_until
        )
    
    run_dir = f"logs/{training_folder}" 
    csv_path = os.path.join(run_dir, f"{run.log_count}_training_metrics.csv")
    
    run.log_count += 1

    # Success metric
    df = pd.read_csv(csv_path)
    final_score = df["eroded_cells"].tail(config.post_training_log_max).mean() 
        
    return final_score
    
    

# Inicia o estudo do Optuna
# study = optuna.create_study(direction="maximize")
# study.optimize(objective, n_trials=100)

train(
    run_=run,
    lr_start=0.001, 
    batch_size=256, 
    buffer_capacity=1_000_000,
    train_until=500_000,
    )