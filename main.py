
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

        # scaled_height = sigmoid_scale(state_data[i][0], 30)
        # scaled_holes = sigmoid_scale(state_data[i][1], 5)
        # scaled_rugosity = sigmoid_scale(state_data[i][2], 10)
        # scaled_current_piece = state_data[i][3] / 6
        
        landing_height = sigmoid_scale(state_data[i][0], 10)
        eroded_cells =sigmoid_scale( state_data[i][1], 10)
        row_transitions = sigmoid_scale(state_data[i][2], 10)
        column_transitions = sigmoid_scale(state_data[i][3], 10)
        holes = sigmoid_scale(state_data[i][4], 10)
        wells = sigmoid_scale(state_data[i][5], 10)
        scaled_current_piece = state_data[i][6] / 6

        queue_tensor = torch.tensor(state_data[i][7:])
        hot_one_piece_queue = F.one_hot(queue_tensor, num_classes=7)

        flat_hot_one = hot_one_piece_queue.flatten()

        # concat = np.concatenate([[scaled_height], [scaled_holes], [scaled_rugosity], [scaled_current_piece], flat_hot_one])

        concat = np.concatenate([
            [landing_height], 
            [eroded_cells], 
            [row_transitions], 
            [column_transitions], 
            [holes], 
            [wells], 
            [scaled_current_piece], 
            flat_hot_one])
        
        scaled_states.append(concat)

    return scaled_states

def scale_test_states(tests) :
    
    scaled_tests = []

    for i in range(len(tests)) :

        # scaled_height = sigmoid_scale(tests[i][0], 30)
        # scaled_holes = sigmoid_scale(tests[i][1], 5)
        # scaled_rugosity = sigmoid_scale(tests[i][2], 10)
        # scaled_current_piece = tests[i][3] / 6
        
        # queue_tensor = torch.tensor(tests[i][4:])
        # hot_one_piece_queue = F.one_hot(queue_tensor, num_classes=7)

        # flat_hot_one = hot_one_piece_queue.flatten()

        # concat = np.concatenate([[scaled_height], [scaled_holes], [scaled_rugosity], [scaled_current_piece], flat_hot_one])

        # scaled_tests.append(concat)

        landing_height = sigmoid_scale(tests[i][0])
        eroded_cells = sigmoid_scale(tests[i][1])
        row_transitions = sigmoid_scale(tests[i][2])
        column_transitions = sigmoid_scale(tests[i][3])
        holes = sigmoid_scale(tests[i][4])
        wells = sigmoid_scale(tests[i][5])
        scaled_current_piece = tests[i][6] / 6

        queue_tensor = torch.tensor(tests[i][7:])
        hot_one_piece_queue = F.one_hot(queue_tensor, num_classes=7)

        flat_hot_one = hot_one_piece_queue.flatten()

        # concat = np.concatenate([[scaled_height], [scaled_holes], [scaled_rugosity], [scaled_current_piece], flat_hot_one])

        concat = np.concatenate([
            [landing_height], 
            [eroded_cells], 
            [row_transitions], 
            [column_transitions], 
            [holes], 
            [wells], 
            [scaled_current_piece], 
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

    base_qs = []

    bellman_results = []

    old_states_matrix =  np.array([sample[0] for sample in samples])
    old_actions_matrix = np.array([sample[1] for sample in samples])
    old_rewards_matrix = np.array([sample[2] for sample in samples])
    new_states_matrix =  np.array([sample[3] for sample in samples])

    old_states_tenson = torch.from_numpy(old_states_matrix)
    old_actions_tensor = torch.from_numpy(old_actions_matrix)
    old_rewards_tensor = torch.from_numpy(old_rewards_matrix)
    new_states_tensor = torch.from_numpy(new_states_matrix)

    q_values = model(old_states_tenson)
    base_qs =  q_values.gather(1, old_actions_tensor.unsqueeze(-1)).squeeze(-1)

    target_q_values = target_model(new_states_tensor)

    max_target_qs = torch.max(target_q_values, dim=1)[0]

    bellman_results = old_rewards_tensor + (0.99 * max_target_qs)

    optimizer.zero_grad()

    loss = criterion(base_qs, bellman_results)

    loss.backward()

    optimizer.step()

print("Started")

NUM_NEURONS = 128
NUM_GAMES   = 8
SET_TARGET_IN_ACTIONS = 500

LOG_PATH = "logs/training_metrics.csv"
LOG_TRAINING_IN_EPISODES = 10

WATCH_TRANING = False

INPUT_DIMENSIONS = 42
OUTPUT_DIMENSIONS = 41

EPSILON_MIN = 0.05
EPSILON_REDUCTION = 0.00005

BUFFER_SAMPLE_SIZE = 128

games = Tetris_AGENT.MultiGame(NUM_GAMES)

model        = DQN(input_dim=INPUT_DIMENSIONS, output_dim=OUTPUT_DIMENSIONS, target_max=SET_TARGET_IN_ACTIONS, number_of_neurons=NUM_NEURONS)
target_model = DQN(input_dim=INPUT_DIMENSIONS, output_dim=OUTPUT_DIMENSIONS, target_max=SET_TARGET_IN_ACTIONS, number_of_neurons=NUM_NEURONS)
optimizer    = optim.Adam(model.parameters(), lr=0.001)
criterion    = nn.MSELoss()

buffer         = ReplayBuffer()
epsilon_greedy = EpsilonGreedy(output_size=OUTPUT_DIMENSIONS, epsilon_min=EPSILON_MIN, reduction_amount=EPSILON_REDUCTION) 


action_selection_counts = np.zeros(OUTPUT_DIMENSIONS, dtype=int)

watcher = EpisodeWatcher(games=games, watch=WATCH_TRANING)

action_count     = 0
sum_action_count = 0

pieces_placed         = 0
pieces_placed_sum     = 0
pieces_placed_counter = 0

episode_interval = 0
placed_total = 0

games.resetAll()
state = np.array(get_all_current_state(games), dtype=np.float32)

main_logs = [
            "time_between_intervals", 
            "episode_interval", 
            "pieces_placed_interval", 
            "lines_interval", 
            "epsilon"
            ]

action_headers = [f"a_{i}_count" for i in range(41)]

main_logger = EpisodeLogger(LOG_TRAINING_IN_EPISODES, log_path=LOG_PATH, data_logs=main_logs)

log_counter = Counter(LOG_TRAINING_IN_EPISODES)

interval_timer = Timer()
interval_timer.start_timer()

while True:

    action_count = 0

    model.check_if_should_set_new_target(policy_net=model, target_net=target_model)
    
    watcher.check_should_watch_episodes()

    look_ahead = games.lookAheadAll()
    test_qs = []

    for i in range(len(look_ahead)) :
        scaled_tests = scale_test_states(look_ahead[i])
        py_array = np.array(scaled_tests)
        test_t = torch.FloatTensor(py_array)
        test_qs.append(model(test_t)) # 8x41

          
    action_count = 8
    output_count = 41
    action_indexes = []

    for i in range(action_count) :

        best = []

        greedy = epsilon_greedy.greedy()
        
        if greedy == -1 :
            for j in range(output_count) :
                best.append(torch.argmax(test_qs[i][j]).item())
            action_indexes.append(max(best))
            action_selection_counts[max(best)] += 1
        else :
            action_indexes.append(greedy)
            action_selection_counts[greedy] += 1


    action_enums = []

    for i in range(len(action_indexes)) :
        action_enums.append(Tetris_AGENT.PreciseActions(action_indexes[i]))

    step_data = games.stepAll(action_enums)

    next_state = np.array(get_all_current_state(games), dtype=np.float32)

    for i in range(len(step_data)) :

        step_i = step_data[i]

        if step_i.piece_placed :

            model.increase_target_counter()

            action_count += 1
            pieces_placed += 1
            pieces_placed_counter += 1

            if (buffer.size() > 1000) :
                experience_samples = buffer.sample(BUFFER_SAMPLE_SIZE)
                back_propagation(experience_samples)

        if step_i.lost :
            games.resetThis(i)
            episode_interval += 1
            if log_counter.increase_counter() :
                log_data = [
                    interval_timer.restart_timer(),
                    episode_interval,
                    pieces_placed,
                    games.getLinesCleared(),
                    epsilon_greedy.epsilon
                ]
                main_logger.csv_log(log_data=log_data)
                sum_action_count = 0
            
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
