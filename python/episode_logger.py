
import os
import csv

class EpisodeLogger:

    log_counter = 0

    episode_count = 0
    pieces_placed_sum = 0
    lines_cleared_sum = 0

    def __init__(self, log_counter_max, log_path, data_logs):
        self.log_counter_max = log_counter_max
        self.log_path = log_path

        
        os.makedirs(os.path.dirname(log_path), exist_ok=True)
        
        with open(self.log_path, mode='w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(data_logs)

    @classmethod
    def console_log_episode(cls, pieces_placed, lines_cleared) :

        cls.pieces_placed_sum += pieces_placed
        cls.lines_cleared_sum += lines_cleared

        print("EPISODE = "                            + str(cls.episode_count)     + 
            " | TOTAL PIECES PLACED = "               + str(cls.pieces_placed_sum) + 
            " | PIECES PLACED IN EPISODE INTERVAL = " + str(pieces_placed) +
            " | TOTAL LINES = "                       + str(cls.lines_cleared_sum) +
            " | LINES CLEARED IN EPISODE INTERVAL = " + str(lines_cleared))
        cls.reset_log_counter()


    def csv_log(cls, log_data) :

        # row = [interval_time,
        #        episode_interval, 
        #        pieces_placed, 
        #        lines_cleared,
        #        epsilon] + [action_selection_counts[i] for i in range(len(action_selection_counts))]
        
        # row = [interval_time,
        #        episode_interval, 
        #        pieces_placed, 
        #        lines_cleared,
        #        epsilon]
        with open(cls.log_path, mode='a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(log_data)
