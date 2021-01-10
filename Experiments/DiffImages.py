import cv2
import numpy as np

img1 = cv2.imread("C:/Users/alime/Pictures/MachineLearning/Games/gameplay-1-tour-de-san-andreas.png")
img1 = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
cv2.resize(img1, (1920, 1080), interpolation=cv2.INTER_LINEAR)
print(type(img1));

img2 = cv2.imread("C:/Users/alime/Pictures/MachineLearning/Games/FSW-Career-Hub-UI.jpg")
img2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)
cv2.resize(img2, (1920, 1080), interpolation=cv2.INTER_LINEAR)
print(type(img2));

diff = cv2.absdiff(img1, img2);
mask = cv2.cvtColor(diff, cv2.COLOR_BGR2GRAY)

th = 1
imask =  mask>th

canvas = np.zeros_like(img2, np.uint8)
canvas[imask] = img2[imask]

cv2.imshow('canvas',canvas)
cv2.waitKey(0);