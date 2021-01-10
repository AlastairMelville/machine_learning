import cv2
from darkflow.net.build import TFNet
import numpy as np
import time
import tensorflow as tf
import os.path


config = tf.ConfigProto(log_device_placement=True)
config.gpu_options.allow_growth = True
with tf.Session(config=config) as sess:
    options = {
            'model': 'cfg/yolo.cfg',
            'load': 'bin/yolo.weights',
            'threshold': 0.1, # Recently changed from 0.4
            'gpu': 1.0
                    }
    tfnet = TFNet(options)

colors = [tuple(255 * np.random.rand(3)) for _ in range(10)]
num = 0


def AnalyseImage(img):
    startTime = time.time()
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    results = tfnet.return_predict(img)
    #for color, result in zip(colors, results):
    #    tl = (result['topleft']['x'], result['topleft']['y'])
    #    br = (result['bottomright']['x'], result['bottomright']['y'])
    #    label = result['label']
    #    confidence = result['confidence']
    #    text = '{}: {:.0f}%'.format(label, confidence * 100)
    #    img = cv2.rectangle(img, tl, br, color, 5)
    #    img = cv2.putText(img, text, tl, cv2.FONT_HERSHEY_COMPLEX, 1, (0, 0, 0), 2)
    
    #cv2.imshow('frame', img)        
    #print('Produced in {:.1f} seconds'.format(time.time() - startTime))
    return results;
    
    #cv2.waitKey(0);
    #cv2.destroyAllWindows()