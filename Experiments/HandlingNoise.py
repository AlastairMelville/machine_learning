import cv2
import numpy as np
from matplotlib import pyplot as plt

img = cv2.imread("C:/Users/alime/Pictures/MachineLearning/Games/BlurredImage.jpg")

blur = cv2.GaussianBlur(img,(5,5),0)
median = cv2.medianBlur(img,5)
blurWithEdges = cv2.bilateralFilter(img,9,75,75)

gray = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
AnitBlur = cv2.Laplacian(gray, cv2.CV_8U)#.var();

cv2.imshow('image', img);
cv2.imshow('image blurred', blur);
cv2.imshow('image median', median);
cv2.imshow('image blurWithEdges', blurWithEdges);
cv2.imshow('image AnitBlur', AnitBlur);
cv2.waitKey(0);