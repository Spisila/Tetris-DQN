
import os, csv, json, datetime
from dataclasses import asdict

class CSVLogger:

    def __init__(self,  log_path, data_logs):
        self.log_path = log_path
        
        with open(self.log_path, mode='w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(data_logs)

    def log(cls, log_data) :
        with open(cls.log_path, mode='a', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(log_data)


class Run:
    def __init__(self, configs, extra=None, base="logs/runs"):
        time_stamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
        file_name = f"{time_stamp}_{configs.tag}" if configs.tag else time_stamp
        self.directory = os.path.join(base, file_name)
        os.makedirs(self.directory)
        self.save_configs(configs=configs, extra=extra or {} )

    def csv(self, name, header) :
        return CSVLogger(os.path.join(self.directory, name), header)

    def save_configs(self, configs, extra) :
        data = {**asdict(configs), **extra}
        json_path = os.path.join(self.directory, "config.json")
        with open(json_path, "w") as f:
            json.dump(data, f, indent=2)

