
import os
import sys
import time

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

from python.counter import Counter
from python.timer import Timer

from dataclasses import asdict
from python.config import from_cli

agent_path = os.path.abspath("./Release")
sys.path.append(agent_path)

from Release import Tetris_AGENT

torch.set_num_threads(2)

config = from_cli()

# def back_propagation(batch):
#     states, rewards, next_states, dones = batch

#     with torch.no_grad():
#         target_vs = target_model(next_states).squeeze(-1)
#         targets = rewards + config.gamma * target_vs * (1.0 - dones)

#     loss = criterion(model(states).squeeze(-1), targets)

#     optimizer.zero_grad()
#     loss.backward()
#     torch.nn.utils.clip_grad_norm_(model.parameters(), max_norm=10.0)
#     optimizer.step()
#     scheduler.step()
#     return loss.item()

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

main_logs = [
            "total_time",
            "time_between_intervals", 
            "steps",
            "episode", 
            "pieces_placed", 
            "lines", 
            "epsilon",
            "loss",
            "rewards",
            "eroded_cells",
            "landing_height",
            "row_transitions",
            "column_transitions",
            "holes",
            "wells",
            "single_clears",
            "double_clears",
            "triple_clears",
            "tetris_clears",
            ]

action_headers = [f"a_{i}_count" for i in range(config.action_space_size)]
action_selection_counts = np.zeros(config.action_space_size, dtype=int)

run = Run(config, {})

main_logger = run.csv("training_metrics.csv", main_logs)
decisions_logger = run.csv("agent_decisions.csv", action_headers)

log_counter = Counter(config.log_every)

interval_timer = Timer()
interval_timer.start_timer()

pieces_placed = 0

lines_cleared_total_last = 0

episodes = 0

total_training_time = 0

action_count = config.num_games

log_loss = 0

log_rewards = 0

steps = 0

landing_height = 0
eroded_cells = 0
row_transitions = 0
column_transitions = 0
holes = 0
wells = 0

# Performance timers

full_loop_perf_timer = Timer()

look_head_perf_timer = Timer()
step_all_perf_timer = Timer()
step_processing_perf_timer = Timer()
back_propagation_perf_timer = Timer()
buffer_batching_perf_timer = Timer()

perf_measure_counter = 0
perf_measure_max = 500
perf_measurments_taken = 0

post_training_log_counter = 0
post_training_log_max = 500

games.resetAll()
state = np.array(games.getStates())

print("Before loop")

while True:

    full_loop_perf_timer.start_timer()

    model.check_if_should_set_new_target(policy_net=model, target_net=target_model)
    
    watcher.check_should_watch_episodes()

    if episodes >= config.post_training_threshold :
        # watcher.should_watch = True
        epsilon_greedy.epsilon = 0
        SHOULD_TRAIN = False

    look_head_perf_timer.start_timer()

    look_ahead = games.lookAheadAll()
    batch = torch.from_numpy(np.asarray(look_ahead, dtype=np.float32))   # (8, 40, 67)
    with torch.inference_mode():
        best_actions = model(batch).squeeze(-1).argmax(dim=1).tolist()  #Takes the best action from each look ahead
    look_head_perf_timer.restart_timer()

    action_indexes = []

    for i in range(action_count) :

        greedy = epsilon_greedy.greedy()
        
        if greedy == -1 :
            best_index = best_actions[i]
            action_indexes.append(best_index)
            action_selection_counts[best_index] += 1
        else :
            action_indexes.append(greedy)
            action_selection_counts[greedy] += 1


    action_enums = []

    for i in range(len(action_indexes)) :
        action_enums.append(Tetris_AGENT.PreciseActions(action_indexes[i]))

    step_all_perf_timer.start_timer()
    step_data = games.stepAll(action_enums)
    step_all_perf_timer.restart_timer()

    for s in state :
        eroded_cells += s[0]
        landing_height += s[1]
        row_transitions += s[2]
        column_transitions += s[3]
        holes += s[4]
        wells += s[5]

    next_state = games.getStates()

    games_to_reset = []

    steps += len(step_data)

    step_processing_perf_timer.start_timer()
    for i in range(len(step_data)) :

        step_i = step_data[i]


        log_rewards += step_i.reward

        if step_i.piece_placed :

            model.increase_target_counter()

            pieces_placed += 1

            if buffer.size() > config.train_start and SHOULD_TRAIN == True :
                
                buffer_batching_perf_timer.start_timer()
                experience_samples = buffer.sample(config.batch_size)
                buffer_batching_perf_timer.restart_timer()
                
                back_propagation_perf_timer.start_timer()
                log_loss = model.back_propagation(experience_samples, target_model, criterion, optimizer, scheduler, config)
                back_propagation_perf_timer.restart_timer()

            if log_counter.increase_counter() :
            
                interval = interval_timer.restart_timer()
                total_training_time += interval

                lines_cleared_total_now = games.getLinesCleared()

                clear_counts = games.getAllClearCounts()

                log_data = [
                    total_training_time,
                    interval,
                    steps,
                    episodes,
                    pieces_placed,
                    lines_cleared_total_now,
                    epsilon_greedy.epsilon,
                    log_loss,
                    log_rewards,
                    eroded_cells,
                    landing_height,
                    row_transitions,
                    column_transitions,
                    holes,
                    wells,
                    clear_counts[0],
                    clear_counts[1],
                    clear_counts[2],
                    clear_counts[3],
                ]
                main_logger.log(log_data=log_data)
                decisions_logger.log(log_data=action_selection_counts)

                if SHOULD_TRAIN == False :
                    post_training_log_counter += 1

        if step_i.lost :
            games_to_reset.append(i)

            episodes += 1

    step_processing_perf_timer.restart_timer()

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

    epsilon_greedy.epsilon = np.interp(episodes, [0, config.post_training_threshold], [1, config.epsilon_min])

    full_loop_perf_timer.restart_timer()

    perf_measure_counter += 1
    
    if perf_measure_counter >= perf_measure_max :
        average_full_loop_time = full_loop_perf_timer.get_total() / perf_measure_max
    
        average_look_ahead_time = look_head_perf_timer.get_total() / perf_measure_max
        average_step_all_time = step_all_perf_timer.get_total() / perf_measure_max
        average_step_processing_time = step_processing_perf_timer.get_total() / perf_measure_max
        average_buffer_batching_time = buffer_batching_perf_timer.get_total() / perf_measure_max
        average_back_propagation_time = back_propagation_perf_timer.get_total() / perf_measure_max

        full_loop_perf_timer.reset_total()
        
        look_head_perf_timer.reset_total()
        step_all_perf_timer.reset_total()
        step_processing_perf_timer.reset_total()
        buffer_batching_perf_timer.reset_total()
        back_propagation_perf_timer.reset_total()

        perf_measurments_taken += 1
        # print(f"\nMEASUREMENT = {perf_measurments_taken}\n")
        # print(f"Average time to make a full loop = {average_full_loop_time}")
        # print(f"Average time to look ahead = {average_look_ahead_time}")
        # print(f"Average time to step all = {average_step_all_time}")
        # print(f"Average time to process steps = {average_step_processing_time}")
        # print(f"    Average time to sample replay buffer = {average_buffer_batching_time}")
        # print(f"    Average time to do backpropagation = {average_back_propagation_time}")

        perf_measure_counter = 0

    if post_training_log_counter >= post_training_log_max :
        sys.exit()
        break

