
import numpy as np
import pandas as pd
#import matplotlib.pyplot as plt

from utils import *


#beta1 for V parameters used in Adam Optimizer
beta1 = 0.90

#beta2 for S parameters used in Adam Optimizer
beta2 = 0.99

def main():
    
    text : str
    with open("shakespeare.txt", "r") as text_file:
        text = text_file.read().lower()

    Tx = 40
    chars = sorted(list(set(text)))
    char_indices = dict((c, i) for i, c in enumerate(chars))
    indices_char = dict((i, c) for i, c in enumerate(chars))

    print("Creating training set...")
    X, Y = build_data(text, Tx, stride = 3)
    print("Vectorizing training set...")
    x, y = vectorization(X, Y, n_x = len(chars), char_indices = char_indices)
    print("chars: ", chars)

    #parameters = model(x, y, vocab_size = len(chars), verbose = True)


if __name__=="__main__":
    main()
