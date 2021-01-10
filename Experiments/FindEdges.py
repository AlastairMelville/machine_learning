# Partly done by Frannecklp

#https://docs.microsoft.com/en-us/visualstudio/python/working-with-c-cpp-python-in-visual-studio?view=vs-2019

import cv2
import numpy as np
import time
from win32 import win32gui
import win32ui, win32con, win32api

def grab_screen(region=None):
    hwin = win32gui.GetDesktopWindow()
    if region:
            left,top,x2,y2 = region
            width = x2 - left + 1
            height = y2 - top + 1
    else:
        width = win32api.GetSystemMetrics(win32con.SM_CXVIRTUALSCREEN)
        height = win32api.GetSystemMetrics(win32con.SM_CYVIRTUALSCREEN)
        left = win32api.GetSystemMetrics(win32con.SM_XVIRTUALSCREEN)
        top = win32api.GetSystemMetrics(win32con.SM_YVIRTUALSCREEN)

    hwindc = win32gui.GetWindowDC(hwin)
    srcdc = win32ui.CreateDCFromHandle(hwindc)
    memdc = srcdc.CreateCompatibleDC()
    bmp = win32ui.CreateBitmap()
    bmp.CreateCompatibleBitmap(srcdc, width, height)
    memdc.SelectObject(bmp)
    memdc.BitBlt((0, 0), (width, height), srcdc, (left, top), win32con.SRCCOPY)

    signedIntsArray = bmp.GetBitmapBits(True)
    img = np.fromstring(signedIntsArray, dtype='uint8')
    img.shape = (height,width,4)

    srcdc.DeleteDC()
    memdc.DeleteDC()
    win32gui.ReleaseDC(hwin, hwindc)
    win32gui.DeleteObject(bmp.GetHandle())

    return cv2.cvtColor(img, cv2.COLOR_BGRA2RGB)

def draw_lines(img, lines):
    try:
        for line in lines:
            coords = line[0];
            LeftPoint = (coords[0],coords[1]);
            RightPoint = (coords[2],coords[3]);
            ChangeInX = RightPoint[0] - LeftPoint[0];
            ChangeInY = RightPoint[1] - LeftPoint[1];
            #if (abs(ChangeInX) > 0.1 and abs(ChangeInY) > 0.1):
            #    continue;
            
            cv2.line(img, LeftPoint, RightPoint, [255,255,255], 1);
            #cv2.line(np.zeros(img.shape), LeftPoint, RightPoint, [255,255,255], 1); # draw on a new/blank image
    except:
        #print("error");
        pass


def roi(img, vertices):
    mask = np.zeros_like(img)
    cv2.fillPoly(mask, vertices, 255)
    masked = cv2.bitwise_and(img, mask)
    return masked


def process_img(original_image):
    processed_img = original_image#cv2.cvtColor(original_image, cv2.COLOR_BGR2GRAY)
    processed_img = cv2.Canny(processed_img, threshold1=50, threshold2=100) # thresholds determine how defined the edges have to be to be accepted.
    processed_img = cv2.GaussianBlur(processed_img, (3,3), 0 )
    #vertices = np.array([[10,500],[10,300], [300,200], [500,200], [800,300], [800,500]], np.int32)
    #processed_img = roi(processed_img, [vertices])

    #                       edges
    lines = cv2.HoughLinesP(processed_img, 1, np.pi/180, 180, np.array([]), 20, 15);
    #draw_lines(processed_img,lines);
    OtherImage = np.zeros(processed_img.shape);
    draw_lines(OtherImage,lines);
    return OtherImage#processed_img
    
#
bFoundPerson = False;
def main():
    last_time = time.time()
    while(True):
        screen = grab_screen(region = (0, 40, 1024, 768))
        # resize to something a bit more acceptable for a CNN
        #screen = cv2.resize(screen, (480, 270))
        # run a color convert:
        #screen = cv2.cvtColor(screen, cv2.COLOR_BGR2RGB)
        
        im = screen#cv2.imread("C:/Users/alime/OneDrive/Photos/MachineLearning/Games/Watcher3Gameplay2.jpg");
        imgray = cv2.cvtColor(im,cv2.COLOR_BGR2GRAY)
        #ret,thresh = cv2.threshold(imgray,127,255,0)
        #blurred = cv2.GaussianBlur(imgray, (13,13), 0 )
        blurred = cv2.GaussianBlur(imgray, (5,5), 0 )
        #cv2.imshow('blurred', blurred)
        
        edges = cv2.Canny(blurred, threshold1=1, threshold2=2)
        
        #kernel = np.ones((5,5),np.uint8)
        #dilation = cv2.dilate(edges,kernel,iterations = 1)
        #cv2.imshow('edges', dilation)
        
        #lines = cv2.HoughLinesP(edges, 1, np.pi/180, 40, np.array([]), 50, 15);
        #print("Number of Lines found = " + str(len(lines)))
        
        #OtherImage = np.zeros(edges.shape);
        #draw_lines(screen,lines);
        
        #draw_lines(edges, lines);
        cv2.imshow('edges', edges)
        
        #contours, hierarchy = cv2.findContours(edges,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
        #print("Number of Contours found = " + str(len(contours))) 
        
        #cnt = contours[4]
        #cv2.drawContours(im, contours, -1, (0, 255, 0), 3)
        
        #print('loop took {} seconds'.format(time.time()-last_time))
        last_time = time.time()
        
        #cv2.imshow('contours', im)
        
        if cv2.waitKey(25) & 0xFF == ord('q'):
            cv2.destroyAllWindows()
            break
    
    
#

if __name__ == "__main__":
    main();