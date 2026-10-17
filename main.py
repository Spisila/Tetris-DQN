
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
from python.episode_logger import EpisodeLogger
from python.episode_watcher import EpisodeWatcher
from python.epsilon_greedy import EpsilonGreedy

from python.counter import Counter
from python.timer import Timer

agent_path = os.path.abspath("./Release")
sys.path.append(agent_path)

from Debug import Tetris_AGENT


def sigmoid_scale(x, k=5.0):
    return (2.0 / (1.0 + np.exp(-x / k))) - 1.0

def get_all_current_state(games: Tetris_AGENT.MultiGame) :
    
    state_data = games.getStates()
    scaled_states = []

    for i in range(len(state_data)) :
        
        landing_height = sigmoid_scale(state_data[i][0], 10)
        eroded_cells =sigmoid_scale( state_data[i][1], 10)
        row_transitions = sigmoid_scale(state_data[i][2], 10)
        column_transitions = sigmoid_scale(state_data[i][3], 10)
        holes = sigmoid_scale(state_data[i][4], 10)
        wells = sigmoid_scale(state_data[i][5], 10)
        
        current_piece_tensor = torch.tensor(state_data[i][6])
        hot_one_current_piece = F.one_hot(current_piece_tensor, num_classes=7)

        queue_tensor = torch.tensor(state_data[i][7:])
        hot_one_piece_queue = F.one_hot(queue_tensor, num_classes=7)

        flat_hot_one = hot_one_piece_queue.flatten()

        concat = np.concatenate([
            [landing_height], 
            [eroded_cells], 
            [row_transitions], 
            [column_transitions], 
            [holes], 
            [wells], 
            hot_one_current_piece.numpy(), 
            flat_hot_one])
        
        scaled_states.append(concat)

    return scaled_states

def scale_test_states(tests) :
    
    scaled_tests = []

    for i in range(len(tests)) :

        landing_height = sigmoid_scale(tests[i][0])
        eroded_cells = sigmoid_scale(tests[i][1])
        row_transitions = sigmoid_scale(tests[i][2])
        column_transitions = sigmoid_scale(tests[i][3])
        holes = sigmoid_scale(tests[i][4])
        wells = sigmoid_scale(tests[i][5])

        current_piece_tensor = torch.tensor(tests[i][6])
        hot_one_current_piece = F.one_hot(current_piece_tensor, num_classes=7)

        queue_tensor = torch.tensor(tests[i][7:])
        hot_one_piece_queue = F.one_hot(queue_tensor, num_classes=7)

        flat_hot_one = hot_one_piece_queue.flatten()

        concat = np.concatenate([
            [landing_height], 
            [eroded_cells], 
            [row_transitions], 
            [column_transitions], 
            [holes], 
            [wells], 
            hot_one_current_piece, 
            flat_hot_one])
        
        scaled_tests.append(concat)


    return scaled_tests

def print_current_state(estado):
    
    agg_height = estado[0]
    holes      = estado[1]
    rugosity   = estado[2]
    piece      = int(estado[3])
    
    queue      = [int(p) for p in estado[4:]]
    
    print(f"--- State ---")
    print(f"AGG height:         {agg_height:.4f}")
    print(f"Holes:              {holes:.4f}")
    print(f"Rugosity:           {rugosity:.4f}")
    print(f"Current piece:      {piece}")
    print(f"Queue:              {queue}")

def back_propagation(samples) :

    # base_qs = []

    # bellman_results = []

    old_states_matrix =  np.array([sample[0] for sample in samples])
    old_actions_matrix = np.array([sample[1] for sample in samples])
    old_rewards_matrix = np.array([sample[2] for sample in samples])
    new_states_matrix =  np.array([sample[3] for sample in samples])
    old_dones_matrix   = np.array([sample[4] for sample in samples], dtype=np.float32)

    old_states_tenson = torch.from_numpy(old_states_matrix)
    old_rewards_tensor = torch.from_numpy(old_rewards_matrix)
    new_states_tensor = torch.from_numpy(new_states_matrix)
    old_dones_tensor =   torch.from_numpy(old_dones_matrix)

    base_vs = model(old_states_tenson).squeeze(-1)
    target_vs = target_model(new_states_tensor).squeeze(-1)

    not_done = 1.0 - old_dones_tensor

    bellman_results = old_rewards_tensor + (0.99 * target_vs * not_done)

    optimizer.zero_grad()

    loss = criterion(base_vs, bellman_results)

    loss.backward()
    torch.nn.utils.clip_grad_norm_(model.parameters(), max_norm=10.0)
    optimizer.step()

    return loss.item()

def format_float(f, digits=3) :
    return f"{f:.{digits}f}"

print("Started")

NUM_NEURONS = 128
NUM_GAMES   = 8
SET_TARGET_IN_ACTIONS = 500

ACTION_SPACE_SIZE = 41
VALUE_OUTPUT_DIM = 1    

LOG_PATH = "logs/training_metrics.csv"
LOG_TRAINING_IN_EPISODES = 10

WATCH_TRANING = True

INPUT_DIMENSIONS = 48
OUTPUT_DIMENSIONS = 41

EPSILON_MIN = 0.05
EPSILON_REDUCTION = 0.00005

BUFFER_SAMPLE_SIZE = 128

games = Tetris_AGENT.MultiGame(NUM_GAMES)

model        = DQN(input_dim=INPUT_DIMENSIONS, output_dim=VALUE_OUTPUT_DIM, target_max=SET_TARGET_IN_ACTIONS, number_of_neurons=NUM_NEURONS)
target_model = DQN(input_dim=INPUT_DIMENSIONS, output_dim=VALUE_OUTPUT_DIM, target_max=SET_TARGET_IN_ACTIONS, number_of_neurons=NUM_NEURONS)
optimizer    = optim.Adam(model.parameters(), lr=0.001)
criterion    = nn.SmoothL1Loss()

buffer         = ReplayBuffer()
epsilon_greedy = EpsilonGreedy(output_size=ACTION_SPACE_SIZE, epsilon_min=EPSILON_MIN, reduction_amount=EPSILON_REDUCTION) 


action_selection_counts = np.zeros(ACTION_SPACE_SIZE, dtype=int)

watcher = EpisodeWatcher(games=games, watch=WATCH_TRANING)

games.resetAll()
state = np.array(get_all_current_state(games), dtype=np.float32)

# LOGGER

main_logs = [
            "total_time",
            "time_between_intervals", 
            "episode_interval", 
            "pieces_placed", 
            "pieces_placed_interval", 
            "lines_interval", 
            "lines_per_piece",
            "epsilon",
            "loss",
            "rewards_interval"
            ]

action_headers = [f"a_{i}_count" for i in range(41)]

main_logger = EpisodeLogger(LOG_TRAINING_IN_EPISODES, log_path=LOG_PATH, data_logs=main_logs)

log_counter = Counter(LOG_TRAINING_IN_EPISODES)

interval_timer = Timer()
interval_timer.start_timer()

global_steps = 0

pieces_placed = 0
pieces_placed_interval = 0

lines_cleared_total_last = 0

episode_interval = 0

total_training_time = 0

action_count = NUM_GAMES
output_count = 41

log_loss = 0

log_rewards = 0

while True:

    model.check_if_should_set_new_target(policy_net=model, target_net=target_model)
    
    watcher.check_should_watch_episodes()

    look_ahead = games.lookAheadAll()
    test_vs = []

    for i in range(len(look_ahead)) :
        scaled_tests = scale_test_states(look_ahead[i])
        py_array = np.array(scaled_tests)
        test_t = torch.FloatTensor(py_array)
        test_vs.append(model(test_t).squeeze(-1)) # 8x41


    action_indexes = []

    for i in range(action_count) :

        best = []

        greedy = epsilon_greedy.greedy()
        
        if greedy == -1 :
            # for j in range(output_count) :
            #     best.append(torch.argmax(test_qs[i][j]).item())
            # action_indexes.append(max(best))
            # action_selection_counts[max(best)] += 1
            best_index = torch.argmax(test_vs[i]).item()
            action_indexes.append(best_index)
            action_selection_counts[best_index] += 1
        else :
            action_indexes.append(greedy)
            action_selection_counts[greedy] += 1


    action_enums = []

    for i in range(len(action_indexes)) :
        action_enums.append(Tetris_AGENT.PreciseActions(action_indexes[i]))

    step_data = games.stepAll(action_enums)

    global_steps += len(step_data)

    next_state = np.array(get_all_current_state(games), dtype=np.float32)

    for i in range(len(step_data)) :

        step_i = step_data[i]
        log_rewards += step_i.reward

        if step_i.piece_placed :

            model.increase_target_counter()

            pieces_placed += 1
            pieces_placed_interval += 1

            if (buffer.size() > 1000) :
                experience_samples = buffer.sample(BUFFER_SAMPLE_SIZE)
                log_loss = back_propagation(experience_samples)

        if step_i.lost :
            games.resetThis(i)
            episode_interval += 1

            if log_counter.increase_counter() :

                interval = interval_timer.restart_timer()
                total_training_time += interval

                placed_interval = pieces_placed_interval

                lines_cleared_total_now = games.getLinesCleared()
                lines_cleared_interval = lines_cleared_total_now - lines_cleared_total_last
                lines_cleared_total_last = lines_cleared_total_now

                log_data = [
                    format_float(total_training_time),
                    format_float(interval),
                    episode_interval,
                    pieces_placed,
                    placed_interval,
                    lines_cleared_interval,
                    format_float(lines_cleared_interval / placed_interval),
                    format_float(epsilon_greedy.epsilon),
                    log_loss,
                    log_rewards
                ]
                main_logger.csv_log(log_data=log_data)

                pieces_placed_interval = 0
                log_rewards = 0
            
    for i in range(len(state)) :
        buffer.push(
            state[i], 
            action_enums[i].value, 
            step_data[i].reward, 
            next_state[i], 
            step_data[i].lost
            )
    
    state = next_state

    epsilon_greedy.reduce_epsilon()
