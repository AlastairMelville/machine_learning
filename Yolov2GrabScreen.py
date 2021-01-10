import cv2
from darkflow.net.build import TFNet
import numpy as np
import time
import tensorflow as tf



def LoadModel():
    config = tf.ConfigProto(log_device_placement=True)
    config.gpu_options.allow_growth = True
    with tf.Session(config=config) as sess:
        options = {
                'model': 'cfg/yolo.cfg',
                'load': 'bin/yolo.weights',
                'threshold': 0.2,
                'gpu': 1.0
                        }
        return TFNet(options)

tfnet = LoadModel(); # Ensures that the model is loaded when we load this script (before main() is called)
        

class FDectectedObject():    
    def __init__(self, InLabel, InConfidence):
        self.Label = InLabel;
        self.Confidence = InConfidence;

def ProcessPixels(Screen, bDebug = False):
    colors = [tuple(255 * np.random.rand(3)) for _ in range(10)]

    DectectedObjects = [];
    
    stime = time.time()
    results = tfnet.return_predict(Screen)
    for color, result in zip(colors, results):
        tl = (result['topleft']['x'], result['topleft']['y'])
        br = (result['bottomright']['x'], result['bottomright']['y'])
        label = result['label']
        confidence = result['confidence'] * 100; # multiply it so it's between 0-1
        DectectedObject = FDectectedObject(label, confidence);
        DectectedObjects.append(DectectedObject);
        
        if (bDebug):
            text = '{}: {:.0f}%'.format(label, confidence);
            Screen = cv2.rectangle(Screen, tl, br, color, 5);
            Screen = cv2.putText(Screen, text, tl, cv2.FONT_HERSHEY_COMPLEX, 1, (0, 0, 0), 2);
    
    if (bDebug):
        cv2.imshow('Screen', Screen);
            
    return DectectedObjects;
            
def main():
    colors = [tuple(255 * np.random.rand(3)) for _ in range(10)]

    capture = cv2.VideoCapture(0)
    capture.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
    capture.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

    while True:
        stime = time.time()
        ret, frame = capture.read()
        if ret:
            results = tfnet.return_predict(frame)
            label = "";
            for color, result in zip(colors, results):
                tl = (result['topleft']['x'], result['topleft']['y'])
                br = (result['bottomright']['x'], result['bottomright']['y'])
                label = result['label']
                confidence = result['confidence']
                text = '{}: {:.0f}%'.format(label, confidence * 100)
                frame = cv2.rectangle(frame, tl, br, color, 5)
                frame = cv2.putText(frame, text, tl, cv2.FONT_HERSHEY_COMPLEX, 1, (0, 0, 0), 2)
            cv2.imshow('frame', frame)
            
            if (label != ""):
                f = open("demofile.txt","a")
                f.write(label + "\n")
            
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    capture.release()
    cv2.destroyAllWindows()
