import random
from collections import deque

import numpy as np
import torch

class ReplayBuffer:
    def __init__(self, capacity=50_000, state_dim=67):
        self.capacity, self.n, self.i = capacity, 0, 0
        self.states      = np.zeros((capacity, state_dim), dtype=np.float32)
        self.next_states = np.zeros((capacity, state_dim), dtype=np.float32)
        self.rewards     = np.zeros(capacity, dtype=np.float32)
        self.dones       = np.zeros(capacity, dtype=np.float32)

    def push(self, state, action, reward, next_state, done):
        i = self.i
        self.states[i], self.next_states[i] = state, next_state
        self.rewards[i], self.dones[i] = reward, done
        self.i = (i + 1) % self.capacity
        self.n = min(self.n + 1, self.capacity)

    def sample(self, batch_size):
        idx = np.random.randint(0, self.n, size=batch_size)
        return (torch.from_numpy(self.states[idx]), torch.from_numpy(self.rewards[idx]),
                torch.from_numpy(self.next_states[idx]), torch.from_numpy(self.dones[idx]))

    def size(self):
        return self.n