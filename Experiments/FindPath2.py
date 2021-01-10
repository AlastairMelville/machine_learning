import cv2
import numpy as np

from win32 import win32gui
import win32ui, win32con, win32api

from directkeys import PressKey, ReleaseKey, MoveMouse, W, A, S, D


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
    

# Determines the pitch of the player camera by evaluating how far up the screen the ground reaches.
# Parameters:
# MaskGreen - an image masked for green pixels (the ground)
# Returns: Number in degrees
def DetermineCameraPitch(MaskGreen):
    return 0;

ScreenSizeX = 1000;
ScreenSizeY = 540;
CharacterApproxPos = (ScreenSizeX / 2, ScreenSizeY);

PressedKey = None;

while (True):
    img = grab_screen(region = (0, 40, ScreenSizeX, ScreenSizeY))

    #img = cv2.imread("C:/Users/alime/OneDrive/Photos/MachineLearning/Games/Watcher3Gameplay2.jpg")
    #cv2.resize(img, (1920, 1080), interpolation=cv2.INTER_LINEAR)

    # Convert BGR to HSV
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

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

    Index = -1;
    BestIndex = 0;
    ShoretestDistance = 99999.0;
    for cnt in contours:
        Index += 1;
        size = cv2.contourArea(cnt)
        if 10000 < size:# < 30000:
            M = cv2.moments(cnt)
            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
         
            # draw the contour and center of the shape on the image
            cv2.drawContours(res, [cnt], -1, (0, 255, 0), 2)
            cv2.circle(res, (cX, cY), 7, (255, 255, 255), -1)
            cv2.putText(res, "center", (cX - 20, cY - 20),
                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 2)
                
            Distance = abs(CharacterApproxPos[0] - cX) + abs(CharacterApproxPos[1] - cY);
            if (Distance < ShoretestDistance):
                BestIndex = Index;
                ShoretestDistance = Distance;
            #cv2.drawContours(res, [cnt], 0, (0,255,0), 1)
    
    if (BestIndex > -1):
        M = cv2.moments(contours[BestIndex])
        cX = int(M["m10"] / M["m00"])
        cY = int(M["m01"] / M["m00"])
        
        # Ensure that the location is on the ground (e.g. not the sky)
        if (cY < ScreenSizeY / 3):
            continue;
        
        cv2.putText(res, "best", (cX - 20, cY),
            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 255), 2)
            
        
        # TurnDirection = 0;
        # RelativeX = cX - CharacterApproxPos[0];
        # if (RelativeX > 50):
            # TurnDirection = 15;
        # elif (RelativeX < -50):
            # TurnDirection = -15;
        
        # if (TurnDirection == 0):  #abs(dot) < 0.1):
            # if (PressedKey != W):
                # PressKey(W);
        # else:
            # if (PressedKey == W):
                # ReleaseKey(W);
                # PressedKey = None;
            # MoveMouse(TurnDirection, 0);
            
        
        
    #cv2.imshow('frame',img)
    #cv2.imshow('mask',MaskGreen)
    #cv2.imshow('ReduceNoise',ReduceNoise)
    cv2.imshow('res',res)
    
    if cv2.waitKey(25) & 0xFF == ord('q'):
        cv2.destroyAllWindows()
        break