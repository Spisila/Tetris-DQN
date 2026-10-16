

class Counter :

  counter = 0

  max_count = 0

  def __init__(self, _max):
    self.max_count = _max
    
  def increase_counter(self) :
    self.counter += 1

    if (self.counter >= self.max_count) :
      self.reset_counter()
      return True

    return False

  def reset_counter(self) :
    self.counter = 0
