
import time


class Timer :

  interval_timer = 0
  total_time = 0
  
  def start_timer(cls) :
    cls.interval_timer = time.perf_counter()

  def restart_timer(cls) :
    end_time = time.perf_counter()
    elapsed = end_time - cls.interval_timer
    cls.total_time += elapsed
    cls.interval_timer = end_time
    return elapsed

  def get_total(cls) :
    return cls.total_time

  def reset_total(cls) :
    cls.total_time = 0