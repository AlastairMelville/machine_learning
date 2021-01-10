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

while True:

    directory = os.fsencode("Inbox")

    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        if filename.endswith(".png") or filename.endswith(".jpg"): 
            filepath = "Inbox/" + filename;
            print(filepath)

    #filename = 'thefile' + str(num) + '.png'
    #if os.path.isfile(filename):
            startTime = time.time()
            img = cv2.imread(filepath, cv2.IMREAD_COLOR)
            img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
            results = tfnet.return_predict(img)
            for color, result in zip(colors, results):
                tl = (result['topleft']['x'], result['topleft']['y'])
                br = (result['bottomright']['x'], result['bottomright']['y'])
                label = result['label']
                confidence = result['confidence']
                text = '{}: {:.0f}%'.format(label, confidence * 100)
            img = cv2.rectangle(img, tl, br, color, 5)
            img = cv2.putText(img, text, tl, cv2.FONT_HERSHEY_COMPLEX, 1, (0, 0, 0), 2)
            cv2.imshow('frame', img)
            with open("Outbox/" + filename[:-4] + "_Detection" + ".txt", "w") as f: # with open("ImageResults" + str(num) + ".txt", "w") as f:
                f.write(label + "\n")
                f.write(str(confidence) + "\n")
                f.write(str(tl) + "\n")#tl[0]
                f.write(str(br))
        
            #num= num + 1
            print('Produced in {:.1f} seconds'.format(time.time() - startTime))
            os.remove(filepath) # remove used image


    cv2.waitKey(0);
    break;
    #print("has left file in directory for loop")
    #break # temp

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cv2.destroyAllWindows()
#VictoryCreateProc()