
import os, csv, json, datetime
from dataclasses import asdict

from python.counter import Counter

class CSVLogger:

    def __init__(self,  log_path, data_logs, log_in):
        self.log_path = log_path
        self.counter = Counter(log_in)
        
        with open(self.log_path, mode='w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(data_logs)

    def log(cls, log_data) :
        with open(cls.log_path, mode='a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(log_data)


class Run:

    def __init__(self, configs, extra=None, base="logs"):
        time_stamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        self.file_name = f"{time_stamp}_{configs.tag}" if configs.tag else time_stamp
        self.directory = os.path.join(base, self.file_name)
        self.log_count = 1
        os.makedirs(self.directory)
        self.save_configs(configs=configs, extra=extra or {} )

    def csv(self, name, header, log_every) :
        return CSVLogger(os.path.join(self.directory, f"{self.log_count}_{name}"), header, log_every)

    def save_configs(self, configs, extra) :
        data = {**asdict(configs), **extra}
        json_path = os.path.join(self.directory, "config.json")
        with open(json_path, "w") as f:
            json.dump(data, f, indent=2)

