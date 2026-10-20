# python/config.py
from dataclasses import dataclass, fields
import argparse

@dataclass(frozen=True)
class Config:
  # ambiente (precisam bater com o C++)
  num_games: int = 8
  action_space_size: int = 40
  input_dimensions: int = 67

  # rede
  num_neurons: int = 128
  set_target_in_actions: int = 500

  # exploração
  epsilon_min: float = 0.05
  epsilon_reduction: float = 0.00005

  # otimização
  gamma: float = 0.99
  lr_start: float = 1e-3
  lr_min: float = 1e-5
  lr_decay_steps: int = 300_000
  batch_size: int = 256
  buffer_capacity: int = 250_000
  train_start: int = 10_000

  # logging e watch
  log_every: int = 500
  watch_training: bool = False
  watch_post_training: bool = True
  post_training_threshold: int = 10_000
  tag: str = ""

def from_cli() -> Config:
  parser = argparse.ArgumentParser()
  for f in fields(Config):
    t = (lambda s: s.lower() in ("1", "true", "yes")) if f.type is bool else f.type
    parser.add_argument(f"--{f.name}", type=t, default=f.default)
  return Config(**vars(parser.parse_args()))