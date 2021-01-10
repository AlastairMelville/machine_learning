import cv2
import numpy as np


Image1 = cv2.imread("C:/Users/alime/Pictures/MachineLearning/Games/FSW-Career-Hub-UI.jpg")

Image2 = cv2.imread("C:/Users/alime/Pictures/MachineLearning/Games/ChessSet.jpg");
Image2 = cv2.resize(Image2, Image1.shape, interpolation=cv2.INTER_LINEAR);

print(Image1.shape);
print(Image2.shape);
vis = np.concatenate((Image1, Image2), axis=1)

cv2.imshow('vis',vis)
cv2.waitKey(0);