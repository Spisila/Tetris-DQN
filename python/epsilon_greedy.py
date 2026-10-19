
import random

class EpsilonGreedy() :

    def __init__(self, output_size, epsilon_min, reduction_amount):
        self.epsilon = 1
        self.max_choice = output_size - 1
        self.min = epsilon_min
        self.reduction_amount = reduction_amount

    def reduce_epsilon(self, custom_amount = 0.00005) :
        
        if self.epsilon > self.min : 
            self.epsilon -= custom_amount 

    def greedy(self) :
        if random.random() > self.epsilon :
            return -1
        else :
            return random.randint(0, self.max_choice)