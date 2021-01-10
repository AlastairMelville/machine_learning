# Partly done by Frannecklp

#https://docs.microsoft.com/en-us/visualstudio/python/working-with-c-cpp-python-in-visual-studio?view=vs-2019
#import module1
#print(module1.fast_tanh2(1));

import cv2
import numpy as np
import time
from win32 import win32gui
import win32ui, win32con, win32api

#import os
#os.chdir('../darkflow')
#import yolov2_od_image2
from directkeys import PressKey, MoveMouse, W, A, S, D
#from FaceRecModel import TryFindMatch


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
            if (abs(ChangeInX) > 0.1 and abs(ChangeInY) > 0.1):
                continue;
            
            #cv2.line(img, LeftPoint, RightPoint, [255,255,255], 1);
            cv2.line(np.zeros(img.shape), LeftPoint, RightPoint, [255,255,255], 1);
    except:
        print("error");
        pass


def roi(img, vertices):
    mask = np.zeros_like(img)
    cv2.fillPoly(mask, vertices, 255)
    masked = cv2.bitwise_and(img, mask)
    return masked


def process_img(original_image):
    processed_img = cv2.cvtColor(original_image, cv2.COLOR_BGR2GRAY)
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
        screen = grab_screen(region = (0, 40, 800, 540))
        orgScreen = screen;
        # resize to something a bit more acceptable for a CNN
        screen = cv2.resize(screen, (480, 270))
        # run a color convert:
        screen = cv2.cvtColor(screen, cv2.COLOR_BGR2RGB)
        cv2.imshow('screen', orgScreen)
        cv2.waitKey(0);
        cv2.destroyAllWindows()
        return;
        #new_screen = process_img(screen)
        
        # Object recognition start
        #if (bFoundPerson == False):
        colors = [tuple(255 * np.random.rand(3)) for _ in range(10)]
        results = []#yolov2_od_image2.AnalyseImage(orgScreen);
        for color, result in zip(colors, results):
            label = result['label']
            #print(label);
            confidence = result['confidence']
            tl = (result['topleft']['x'], result['topleft']['y'])
            br = (result['bottomright']['x'], result['bottomright']['y'])
            if (label == "person" and confidence > 0.3):
                # Move to person                
                if (orgScreen.any() and abs(tl[0] - tl[1]) > 10 and abs(br[0] - br[1]) > 10):
                    ScreenSection = np.array([]);
                    ScreenSection = orgScreen[tl[1]:br[1], tl[0]:br[0]];
                    if (ScreenSection.any()):
                        cv2.imshow('ScreenSection', ScreenSection);
                    else:
                        print("error: ScreenSection size is null");
                    #cv2.waitKey(0);
                    
                    #if (ScreenSection.any()):
                    #    dist, identity, bFound = TryFindMatch(ScreenSection, tl, br);
                    #    if (bFound is True):# and dist < 0.5):
                    #        label = identity;#continue;
                            #print(dist);
                            #if (identity == "trevor"):
                
                if (tl[1] < 500): # is the person on the left side of the screen? Do we need to turn left or right?
                    
                    TurnDirection = 0.0;
                    RelativeX = tl[0] - 400.0;
                    if (RelativeX > 50.0):
                        TurnDirection = 1.0;
                    elif (RelativeX < -50.0):
                        TurnDirection = -1.0;
                    
                    #if (TurnDirection == 0.0):  #abs(dot) < 0.1):
                    #    PressKey(W);
                    #else:
                    #    MoveMouse(TurnDirection, 0);
                        
                    if cv2.waitKey(25) & 0xFF == ord('q'):
                        cv2.destroyAllWindows()
                        break
            
            text = '{}: {:.0f}%'.format(label, confidence * 100)
            orgScreen = cv2.rectangle(orgScreen, tl, br, color, 5)
            orgScreen = cv2.putText(orgScreen, text, tl, cv2.FONT_HERSHEY_COMPLEX, 1, (0, 0, 0), 2)
        
        cv2.imshow('screen', orgScreen)
        
        #print('loop took {} seconds'.format(time.time()-last_time))
        last_time = time.time()
        
        #cv2.imshow('window', new_screen)
        
        if cv2.waitKey(25) & 0xFF == ord('q'):
            cv2.destroyAllWindows()
            break
    
    
#

if __name__ == "__main__":
    main();