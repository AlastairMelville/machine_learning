import cv2
import numpy as np
import matplotlib.pyplot as plt

def RotateImage(InRotation, InImage):
    if (InImage == None):
        img = cv2.imread("C:/Users/alime/Pictures/MachineLearning/Games/ChessSet.jpg",0)
    else:
        img = InImage
        
    rows,cols = img.shape

    M = cv2.getRotationMatrix2D((cols/2,rows/2),InRotation,1)
    dst = cv2.warpAffine(img,M,(cols,rows))
    cv2.imshow('image', dst);
    cv2.waitKey(0);
    
    
def SetOrientation():
    img = cv2.imread("C:/Users/alime/Pictures/MachineLearning/Games/ChessSet.jpg")
    rows, cols, channels = img.shape

    pts1 = np.float32([[11,48],[200,23],[48,160]])
    pts2 = np.float32([[11,48],[200,48],[11,160]])

    M = cv2.getAffineTransform(pts1,pts2)

    dst = cv2.warpAffine(img,M,(cols,rows))

    plt.subplot(121),plt.imshow(img),plt.title('Input')
    plt.subplot(122),plt.imshow(dst),plt.title('Output')
    plt.show()
    return dst

    
def SetPerspective(InImage):
    #if (InImage.size > 0):
    #    img = cv2.imread("C:/Users/alime/Pictures/MachineLearning/Games/ChessSet.jpg",0)
    #else:
    #    img = InImage
    
    img = InImage
    rows, cols, channels = img.shape

    pts1 = np.float32([[11,48],[200,48],[11,160],[251,151]])
    pts2 = np.float32([[0,0],[300,0],[0,300],[300,300]])

    M = cv2.getPerspectiveTransform(pts1,pts2)

    dst = cv2.warpPerspective(img,M,(300,300))

    plt.subplot(121),plt.imshow(img),plt.title('Input')
    plt.subplot(122),plt.imshow(dst),plt.title('Output')
    plt.show()
    
def main():
    #RotateImage(-30);
    img = SetOrientation();
    SetPerspective(img);
    
main();