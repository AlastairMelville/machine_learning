

#from MonitorScreenUtilities import grab_screen

from PythonApp_PositionAndRotation import main as PositionAndRotation

from Input.DirectInputKeys import PressKey, MoveMouse, W, A, S, D

#from Detection.ImageRecognition import TryFindMatch

# c++ modules Start
import DetectSelfPositionAndRotation
CppObj = DetectSelfPositionAndRotation.FEngineLoop()

# c++ modules End


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


def main():
    #dist, identity, bFound = TryFindMatch("Detection/Images/Area1_1.jpg", (0, 0), (0, 0));
    #print("dist: " + str(dist));
    #print("identity: " + identity);
    #print("bFound: " + str(bFound));

    while(False):#True):
        screen = grab_screen(region = (0, 40, 800, 540))
        orgScreen = screen;
        # resize to something a bit more acceptable for a CNN
        screen = cv2.resize(screen, (480, 270))
        # run a color convert:
        screen = cv2.cvtColor(screen, cv2.COLOR_BGR2RGB)
        cv2.imshow('screen', orgScreen)
        
        if cv2.waitKey(25) & 0xFF == ord('q'):
            cv2.destroyAllWindows()
            break


    #PositionAndRotation();
    CppObj.Main();

    



if __name__ == "__main__":
    main();