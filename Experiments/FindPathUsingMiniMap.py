import cv2
import numpy as np

from win32 import win32gui
import win32ui, win32con, win32api


Tolerance = 37;

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




while (True):
    img = grab_screen(region = (0, 40, 1000, 540))

    #img = cv2.imread("C:/Users/alime/OneDrive/Photos/MachineLearning/Games/Watcher3Gameplay2.jpg")
    #cv2.resize(img, (1920, 1080), interpolation=cv2.INTER_LINEAR)

    # Convert BGR to HSV
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    
    WhiteBGR = np.uint8([[[ 255, 255, 255 ]]])
    hsv_white = cv2.cvtColor(WhiteBGR, cv2.COLOR_BGR2HSV);
    lower_white = np.array([0,0,150]);
    upper_white = np.array([100,100,255]);
    MaskWhite = cv2.inRange(hsv, lower_white, upper_white)
    
    cv2.imshow('MaskWhite',MaskWhite)
    
    if cv2.waitKey(25) & 0xFF == ord('q'):
            cv2.destroyAllWindows()
            break
    
    continue;

    # define range of blue color in HSV
    GreenRgb = np.uint8([[[ 0, 255, 0 ]]])
    hsv_green = cv2.cvtColor(GreenRgb,cv2.COLOR_BGR2HSV);
    GreenHue = hsv_green[0][0][0];
    lower_green = np.array([GreenHue - Tolerance,0,0])
    upper_green = np.array([GreenHue + Tolerance,255,255])

    BrownHue = 30
    lower_blue = np.array([BrownHue - Tolerance,0,0])
    upper_blue = np.array([BrownHue + Tolerance,255,255])

    MaskGreen = cv2.inRange(hsv, lower_green, upper_green)

    # Threshold the HSV image to get only blue colors
    MaskBlue = cv2.inRange(hsv, lower_blue, upper_blue)

    #MaskBlueGreen = cv2.bitwise_or(MaskBlue, MaskGreen);

    # Bitwise-AND mask and original image
    res = cv2.bitwise_and(img, img, mask= MaskGreen);#MaskBlueGreen)

    ret, InvMaskGreen = cv2.threshold(MaskGreen,127,255,cv2.THRESH_BINARY_INV) # Convert black to white, and vice-versa. This is so that erosion and dialtion work correctly, as they focus on white pixels.
    kernel = np.ones((11,11),np.uint8)
    ReduceNoise = cv2.erode(InvMaskGreen,kernel,iterations = 2)
    ReduceNoise = cv2.dilate(ReduceNoise,kernel,iterations = 5)

    gray = cv2.cvtColor(res, cv2.COLOR_BGR2GRAY)
    contours, _ = cv2.findContours(ReduceNoise,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)

    for cnt in contours:
        size = cv2.contourArea(cnt)
        if 30000 < size:# < 30000:
            M = cv2.moments(cnt)
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
         
            # draw the contour and center of the shape on the image
            cv2.drawContours(res, [cnt], -1, (0, 255, 0), 2)
            cv2.circle(res, (cX, cY), 7, (255, 255, 255), -1)
            cv2.putText(res, "center", (cX - 20, cY - 20),
                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
            #cv2.drawContours(res, [cnt], 0, (0,255,0), 1)

    #cv2.imshow('frame',img)
    #cv2.imshow('mask',MaskGreen)
    #cv2.imshow('ReduceNoise',ReduceNoise)
    cv2.imshow('res',res)
    
    if cv2.waitKey(25) & 0xFF == ord('q'):
            cv2.destroyAllWindows()
            break