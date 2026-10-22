
import random
import numpy as np 

class EpsilonGreedy() :

    def __init__(self, output_size, epsilon_min, reduction_amount, seed):
        self.epsilon = 1
        self.max_choice = output_size - 1
        self.min = epsilon_min
        self.reduction_amount = reduction_amount

        random.seed(seed)

    def reduce_epsilon(self, custom_amount = 0.00005) :
        
        if self.epsilon > self.min : 
            self.epsilon -= custom_amount 

    def linear_epsilon_reduction(self, value, max) :
        self.epsilon = np.interp(value, [0, max], [1, self.min])

    def greedy(self) :
        if random.random() > self.epsilon :
            return -1
        else :
            return random.randint(0, self.max_choice)