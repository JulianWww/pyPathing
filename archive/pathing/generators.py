"generators of mazes"

try:
    from pathing.scr import cy_generators
except:
    import pathing.setup
    pathing.setup.run()
    raise RuntimeError("restart the program plz as setup is compleat")

import numpy as np

def prim(x_size: int, y_size: int, seed:int=0, reprProb:float=.1) -> np.ndarray:
    """prim maze generation

        prim maze generation of a maze of size x_size, y_size

        @param reprProb: the probability of continuening computations after first connection
        @param seed: the seed of the mazes generation random function
        """
    if x_size%2==0: raise ValueError(f"x_size must be odd not {x_size}")
    if y_size%2==0: raise ValueError(f"y_size must be odd not {y_size}")
    v = cy_generators.prim(x_size, y_size, seed, int(reprProb*100))
    return np.array(v).reshape(x_size, y_size)