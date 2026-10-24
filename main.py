import os

import pandas as pd
import optuna

from python.episode_logger import Run
from python.config import from_cli
from python.training_loop import train

config = from_cli()

run = Run(config, {})

training_folder = run.file_name

seeds = [45, 99, 150]
seed_index = 0

def objective(trial):

    # Parameters
    survival_weight = trial.suggest_float("survival_weight", 0.75, 3, log=True)
    quadratic_weight = trial.suggest_float("quadratic_weight", 10, 15, log=True)
    loss_weight = trial.suggest_float("loss_weight", 20, 45, log=True)
    
    
    lr = trial.suggest_float("lr_start", 1e-4, 1e-3, log=True)
    epsilon_reduciton = trial.suggest_float("epsilon_reduciton", 0.000005, 0.00005, log=True)
    epsilon_min = trial.suggest_float("epsilon_min", 0.01, 0.05, log=True)
    
    # batch_size = trial.suggest_categorical("batch_size", [128, 256])
    batch_size = 128
    # buffer_capacity = trial.suggest_categorical("buffer_capacity", [250_000, 500_000])
    buffer_capacity = 250_000
    # train_until = trial.suggest_categorical("train_until", [150_000, 250_000, 500_000])
    train_until = 500_000

    trials_in_seeds = []

    for i in range(len(seeds)) :
        current_seed = seeds[i]

        train(
            seed=current_seed,
            run_=run,
            lr_start=lr, 
            batch_size=batch_size, 
            buffer_capacity=buffer_capacity,
            train_until=train_until,
            survival_weight=survival_weight,
            quadratic_weight=quadratic_weight,
            loss_weight=-loss_weight,
            epsilon_reduction = epsilon_reduciton,
            epsilon_min = epsilon_min
            )
        
        

        trials_in_seeds.append(run.log_count)
        run.log_count += 1


    run_dir = f"logs/{training_folder}" 

    # csv_path = os.path.join(run_dir, f"{run.log_count}_training_metrics.csv")
    
    trials = [
    os.path.join(run_dir, f"{num}_training_metrics.csv") for num in trials_in_seeds
    ]

    trial_scores = [ ]

    for trial in trials :
        df_t = pd.read_csv(trial)
        trial_scores.append(df_t["rewards"].tail(config.post_training_log_max).mean()) 

    final_score = sum(trial_scores) / len(seeds)

    trials_in_seeds.clear()
    
    return final_score
    
    


# study = optuna.create_study(direction="maximize")
# study.optimize(objective, n_trials=20)

train(
    seed=22,
    run_=run,
    lr_start=0.00005, 
    batch_size=128, 
    buffer_capacity=250_000,
    train_until=500_000,
    survival_weight=0.8685837453230001,
    quadratic_weight=12,
    loss_weight=-30,
    epsilon_reduction = 0.000005,
    epsilon_min = 0.022958112603046425,
    watch=True
    )