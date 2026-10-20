
import os
import sys

import numpy as np

import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F

from python.DQN            import DQN
from python.replay_buffer  import ReplayBuffer
from python.episode_logger import Run
from python.episode_watcher import EpisodeWatcher
from python.epsilon_greedy import EpsilonGreedy

from python.timer import Timer
from python.counter import Counter

from dataclasses import asdict
from python.config import from_cli

agent_path = os.path.abspath("./Release")
sys.path.append(agent_path)

from Release import Tetris_AGENT

torch.set_num_threads(2)

config = from_cli()

def format_float(f, digits=3) :
    return f"{f:.{digits}f}"

print("Started")
print("Initialization")

games = Tetris_AGENT.MultiGame(config.num_games)

model        = DQN(input_dim=config.input_dimensions, output_dim=1, target_max=config.set_target_in_actions, number_of_neurons=config.num_neurons)
target_model = DQN(input_dim=config.input_dimensions, output_dim=1, target_max=config.set_target_in_actions, number_of_neurons=config.num_neurons)

optimizer    = optim.Adam(model.parameters(), lr=config.lr_start)
scheduler = optim.lr_scheduler.LambdaLR(
    optimizer,
    lr_lambda=lambda step: max(config.lr_min / config.lr_start, 0.1 ** (step / config.lr_decay_steps))
)

criterion    = nn.SmoothL1Loss()

buffer         = ReplayBuffer(capacity=config.buffer_capacity)

epsilon_greedy = EpsilonGreedy(output_size=config.action_space_size, epsilon_min=config.epsilon_min, reduction_amount=config.epsilon_reduction) 

# LOGGER

SHOULD_TRAIN = True

watcher = EpisodeWatcher(games=games, watch=config.watch_training)

run = Run(config, {})

log_names_to_data = {
    "total_time": 0,
    "steps": 0,
    "episodes": 0,
    "pieces_placed": 0,
    "epsilon": 0,
    "loss": 0,
    "rewards": 0,
    "greedy_choices": 0,
    "epsilon_choices": 0,
    "buffer_size": 0,
    "rewards": 0,
    "eroded_cells": 0,
    "landing_height": 0,
    "row_transitions": 0,
    "column_transitions": 0,
    "holes": 0,
    "wells": 0,
    "single_clears": 0,
    "double_clears": 0,
    "triple_clears": 0,
    "tetris_clears": 0,
}

action_headers = [f"a_{i}_count" for i in range(config.action_space_size)]
action_selection_counts = np.zeros(config.action_space_size, dtype=int)

main_logger = run.csv("training_metrics.csv", list(log_names_to_data.keys()), config.log_every)
decisions_logger = run.csv("agent_decisions.csv", action_headers, config.log_every)

interval_timer = Timer()
interval_timer.start_timer()

post_training_counter = Counter(config.post_training_log_max)

games.resetAll()
state = np.array(games.getStates())

print("Before loop")

is_running = True

while is_running:

    model.check_if_should_set_new_target(policy_net=model, target_net=target_model)
    
    watcher.check_should_watch_episodes()
    
    if log_names_to_data["episodes"] >= config.post_training_threshold :
        watcher.should_watch = True
        epsilon_greedy.epsilon = 0
        SHOULD_TRAIN = False

    look_ahead = games.lookAheadAll()
    batch = torch.from_numpy(np.asarray(look_ahead, dtype=np.float32))   # (8, 40, 67)
    with torch.inference_mode():
        best_actions = model(batch).squeeze(-1).argmax(dim=1).tolist()  #Takes the best action from each look ahead

    action_indexes = []

    for i in range(config.num_games) :

        greedy = epsilon_greedy.greedy()
        
        if greedy == -1 :
            best_index = best_actions[i]
            action_indexes.append(best_index)
            action_selection_counts[best_index] += 1
            log_names_to_data["greedy_choices"] += 1
        else :
            action_indexes.append(greedy)
            action_selection_counts[greedy] += 1
            log_names_to_data["epsilon_choices"] += 1


    action_enums = []

    for i in range(len(action_indexes)) :
        action_enums.append(Tetris_AGENT.PreciseActions(action_indexes[i]))

    step_data = games.stepAll(action_enums)

    for s in state :
        log_names_to_data["eroded_cells"] += s[0]
        log_names_to_data["landing_height"] += s[1]
        log_names_to_data["row_transitions"] += s[2]
        log_names_to_data["column_transitions"] += s[3]
        log_names_to_data["holes"] += s[4]
        log_names_to_data["wells"] += s[5]

    next_state = games.getStates()

    games_to_reset = []

    log_names_to_data["steps"] += len(step_data)

    for i in range(len(step_data)) :

        step_i = step_data[i]

        log_names_to_data["rewards"] += step_i.reward

        if step_i.piece_placed :

            model.increase_target_counter()

            log_names_to_data["pieces_placed"] += 1

            if buffer.size() > config.train_start and SHOULD_TRAIN == True :
                
                experience_samples = buffer.sample(config.batch_size)
                
                log_names_to_data["loss"]= model.back_propagation(experience_samples, target_model, criterion, optimizer, scheduler, config)

            if main_logger.counter.increase_counter() :

                log_names_to_data["total_time"] += interval_timer.restart_timer()

                log_names_to_data["buffer_size"] = buffer.size()

                clear_counts = games.getAllClearCounts()

                log_names_to_data["single_clears"] = clear_counts[0]
                log_names_to_data["double_clears"] = clear_counts[1]
                log_names_to_data["triple_clears"] = clear_counts[2]
                log_names_to_data["tetris_clears"] = clear_counts[3]

                log_names_to_data["epsilon"] = epsilon_greedy.epsilon

                decisions_logger.log(log_data=action_selection_counts)

                main_logger.log(log_data=list(log_names_to_data.values()))

                if SHOULD_TRAIN == False :
                    if post_training_counter.increase_counter() :
                        is_running = False

        if step_i.lost :
            games_to_reset.append(i)

            log_names_to_data["episodes"] += 1

    for i in range(len(state)) :
        buffer.push(
            state[i], 
            action_enums[i].value, 
            step_data[i].reward, 
            next_state[i], 
            step_data[i].lost
            )
    
    state = next_state

    for game in games_to_reset :
        games.resetThis(game)

    epsilon_greedy.epsilon = np.interp(log_names_to_data["episodes"], [0, config.post_training_threshold], [1, config.epsilon_min])


print("E")