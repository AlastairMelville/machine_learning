import cv2
import numpy as np

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

def GetImmediateAreaMask(Screen):
    # Convert BGR to HSV
    hsv = cv2.cvtColor(Screen, cv2.COLOR_BGR2HSV)
    
    # Filter so that faces of objects that don't get the sun are black
    Hue = hsv[:, :, 1];
    
    _, Thresh = cv2.threshold(Hue,177,255,cv2.THRESH_BINARY)
    
    kernel = np.ones((3,3),np.uint8)
    ReducedNoise = cv2.erode(Thresh,kernel,iterations = 2)
    ReducedNoise = cv2.dilate(ReducedNoise,kernel,iterations = 2)
    cv2.imshow('ImmediateAreaMask', ReducedNoise)

while (True):
    Screen = grab_screen(region = (0, 40, 1024, 768))
    
    #GetImmediateAreaMask(Screen);

    # Convert BGR to HSV
    hsv = cv2.cvtColor(Screen, cv2.COLOR_BGR2HSV)
    
    # Filter so that faces of objects that don't get the sun are black
    Hue = hsv[:, :, 0];
    
    blurred = cv2.GaussianBlur(Hue, (3,3), 0 )
    #edges = cv2.Canny(blurred, threshold1=20, threshold2=60)
    edges = cv2.Canny(blurred, threshold1=50, threshold2=150)
    
    kernel = np.ones((3,3),np.uint8)
    ReducedNoise = edges#cv2.erode(edges,kernel,iterations = 1)
    #ReducedNoise = cv2.dilate(ReducedNoise,kernel,iterations = 1)
    
    cv2.imshow('hsv', ReducedNoise)
        
    if cv2.waitKey(25) & 0xFF == ord('q'):
        cv2.destroyAllWindows()
        break