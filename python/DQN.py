
import torch
import torch.nn as nn
import torch.optim as optim
import torch.nn.functional as F

class DQN(nn.Module):

    set_target_counter = 0

    def __init__(self, input_dim, output_dim, target_max, number_of_neurons):

        self.set_target_max = target_max

        super(DQN, self).__init__()
        self.net = nn.Sequential(
            nn.Linear(input_dim, number_of_neurons),
            nn.ReLU(),
            nn.Linear(number_of_neurons, number_of_neurons),
            nn.ReLU(),
            nn.Linear(number_of_neurons, output_dim)
        )

    def forward(self, x):
        return self.net(x)
    
    def back_propagation(self, batch, target_model, criterion, optimizer, scheduler, config):
        states, rewards, next_states, dones = batch

        with torch.no_grad():
            target_vs = target_model(next_states).squeeze(-1)
            targets = rewards + config.gamma * target_vs * (1.0 - dones)

        loss = criterion(self(states).squeeze(-1), targets)

        optimizer.zero_grad()
        loss.backward()
        torch.nn.utils.clip_grad_norm_(self.parameters(), max_norm=10.0)
        optimizer.step()
        scheduler.step()
        return loss.item()
    
    @classmethod
    def increase_target_counter(cls) :
        cls.set_target_counter += 1

    @classmethod
    def reset_target_counter(cls) :
        cls.set_target_counter = 0


    def check_if_should_set_new_target(self, policy_net, target_net) :

        if DQN.set_target_counter >= self.set_target_max :
            target_net.load_state_dict(policy_net.state_dict())
            DQN.reset_target_counter()
