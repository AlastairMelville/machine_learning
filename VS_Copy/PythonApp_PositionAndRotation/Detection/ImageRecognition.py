from keras.models import Sequential
from keras.layers import Conv2D, ZeroPadding2D, Activation, Input, concatenate
from keras.models import Model
from keras.layers.normalization import BatchNormalization
from keras.layers.pooling import MaxPooling2D, AveragePooling2D
from keras.layers.merge import Concatenate
from keras.layers.core import Lambda, Flatten, Dense
from keras.initializers import glorot_uniform
from keras.engine.topology import Layer
from keras import backend as K
K.set_image_data_format('channels_first')
import cv2
import os
import numpy as np
from numpy import genfromtxt
import pandas as pd
import tensorflow as tf

#import sys
#sys.path.append('C:/Users/alime/Anaconda3/FULL-TENSORFLOW-NOTES-AND-DATA/Tensorflow-Bootcamp-master/darkflow/Recognition/')
from Detection.fr_utils import *
from Detection.inception_blocks_v2 import *
from Detection.ImageRecognitionUtilities import *

import time

from IPython import get_ipython
ipy = get_ipython()
if ipy is not None:
    ipy.run_line_magic('matplotlib', 'inline')
else:
    print("Failed to import ipython")

import sys
np.set_printoptions(sys.maxsize);#threshold=np.nan)


FRmodel = faceRecoModel(input_shape=(3, 96, 96))
#print("Total Params:", FRmodel.count_params())

FRmodel.compile(optimizer = 'adam', loss = triplet_loss, metrics = ['accuracy'])
load_weights_from_FaceNet(FRmodel)


database = {}
database["danielle"] = [img_to_encoding("Detection/Images/danielle.png", FRmodel)]
database["younes"] = [img_to_encoding("Detection/Images/younes.jpg", FRmodel)]
database["tian"] = [img_to_encoding("Detection/Images/tian.jpg", FRmodel)]
database["andrew"] = [img_to_encoding("Detection/Images/andrew.jpg", FRmodel)]
database["kian"] = [img_to_encoding("Detection/Images/kian.jpg", FRmodel)]
database["dan"] = [img_to_encoding("Detection/Images/dan.jpg", FRmodel)]
database["sebastiano"] = [img_to_encoding("Detection/Images/sebastiano.jpg", FRmodel)]
database["bertrand"] = [img_to_encoding("Detection/Images/bertrand.jpg", FRmodel)]
database["kevin"] = [img_to_encoding("Detection/Images/kevin.jpg", FRmodel)]
database["felix"] = [img_to_encoding("Detection/Images/felix.jpg", FRmodel)]
database["benoit"] = [img_to_encoding("Detection/Images/benoit.jpg", FRmodel)]
database["arnaud"] = [img_to_encoding("Detection/Images/arnaud.jpg", FRmodel)]
database["kiritotfour"] = [img_to_encoding("Detection/Images/kiritotfour.jpg", FRmodel)]
database["area1"] = [img_to_encoding("Detection/Images/Area1_0.jpg", FRmodel)]
#database["trevor"] = [img_to_encoding("Images/Trevor4.jpg", FRmodel)]


class LocationData:
    def __init__(self, Identity, tl, br, time):
        self.Identity = Identity;
        self.tl = tl;
        self.br = br;
        self.time = time;
        
    Identity = "";
    tl = (0,0);
    br = (0,0);
    time = 0;
    
    
class Statics:
    def __init__(self):
        pass;

    PostfixNum = 0; # Number to append to new identities to make them all unique
    PreviouslySeenPeople = {};
    LastPersonSeen = LocationData("", (9999,9999), (9999,9999), 0);


def TryFindMatch(img, tl, br):
    min_dist, identity, bFound = who_is_it(img, database, FRmodel)
    return min_dist, identity, bFound;



    if (bFound is True):
        # If found, update the location data
        #Statics.PreviouslySeenPeople[identity] = LocationData(tl, br, time.time());
        Statics.LastPersonSeen = LocationData(identity, tl, br, time.time());
    else:
        #for key, value in Statics.PreviouslySeenPeople.items():
        Person = Statics.LastPersonSeen;
        if (time.time() - Person.time < 3.0): # We don't care about old data
            distance = abs(Person.tl[0] - tl[0]) + abs(Person.tl[1] - tl[1])# + abs(Person.br[0] - br[0]) + abs(Person.br[1] - br[1]);
            print(distance);
            if (distance < 25):
                try:
                    if (Person.Identity in database):
                        arr = [np.array([])]; # list of np.array
                        arr = database[Person.Identity];
                        arr.append(image_to_encoding(img, FRmodel));
                        print("arr.shape 3: " + str(len(arr)));
                        database[Person.Identity] = arr;
                    else:
                        database[Person.Identity] = [image_to_encoding(img, FRmodel)];
                        print("len(database): " + str(len(database)));
                except:
                    pass;
                
                min_dist = 0.4;
                identity = Person.Identity;
                bFound = True;
                #break;
            if (bFound is False):
                if (Statics.PostfixNum > 5):
                    Statics.PostfixNum = 0;
                
                identity = "person" + str(Statics.PostfixNum);
                database[identity] = [image_to_encoding(img, FRmodel)];
                #Statics.PreviouslySeenPeople[identity] = LocationData(tl, br, time.time());
                Statics.LastPersonSeen = LocationData(identity, tl, br, time.time());
                Statics.PostfixNum += 1;
    
    return min_dist, identity, bFound;
	
