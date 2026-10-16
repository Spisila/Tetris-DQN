
import os
import csv
import time

from python.timer import Timer

class EpisodeLogger:

    log_counter = 0

    episode_count = 0
    pieces_placed_sum = 0
    lines_cleared_sum = 0

    timer = Timer()

    def __init__(self, log_counter_max, log_path, data_logs):
        self.log_counter_max = log_counter_max
        self.log_path = log_path

        
        os.makedirs(os.path.dirname(log_path), exist_ok=True)
        
        with open(self.log_path, mode='w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(data_logs)


    @classmethod
    def increase_log_counter(cls) :
        cls.log_counter += 1
        cls.episode_count += 1

    @classmethod
    def reset_log_counter(cls) :
        cls.log_counter = 0

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


    def csv_log(cls, pieces_placed, lines_cleared, action_selection_counts, epsilon) :

        row = [cls.timer.restart_timer(),
               cls.episode_count, 
               cls.pieces_placed_sum, 
               pieces_placed, 
               cls.lines_cleared_sum, 
               lines_cleared,
               epsilon] + [action_selection_counts[i] for i in range(len(action_selection_counts))]
        
        with open(cls.log_path, mode='a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(row)

        cls.timer.restart_timer()

    def check_should_console_log_episodes(self, 
                                          pieces_placed, 
                                          lines_cleared, 
                                          action_selection_counts, 
                                          epsilon
                                          ) :

        if EpisodeLogger.log_counter >= self.log_counter_max :
            self.console_log_episode(
                pieces_placed=pieces_placed, 
                lines_cleared=lines_cleared
                )
            self.csv_log(
                pieces_placed=pieces_placed, 
                lines_cleared=lines_cleared, 
                action_selection_counts=action_selection_counts, 
                epsilon=epsilon
                )
            return True
        return False