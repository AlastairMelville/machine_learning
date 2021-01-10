import numpy as np
import cv2
from matplotlib import pyplot as plt
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

    return img#cv2.cvtColor(img, cv2.COLOR_BGRA2RGB)


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





def FindDepthUsingSaturation(Screen):
    Hsv = cv2.cvtColor(Screen, cv2.COLOR_BGR2HSV);
    Saturation = Hsv[:, :, 1];
    
    BlurWithEdges = cv2.bilateralFilter(Saturation,9,75,75)
    
    Edges = cv2.Canny(BlurWithEdges, threshold1=150, threshold2=250)
    
    Lines = cv2.HoughLinesP(Edges, 1, np.pi/180, 40, np.array([]), minLineLength=150, maxLineGap=10);
    draw_lines(BlurWithEdges, Lines);
    
    # Round pixel values to make things easier
    #Multiple = 150;
    #BlurWithEdges = np.floor((BlurWithEdges * Multiple) + 0.5) / Multiple;
    Mask1 = (BlurWithEdges <= 51);
    Mask2 = ((BlurWithEdges > 51) & (BlurWithEdges <= 102));
    Mask3 = ((BlurWithEdges > 102) & (BlurWithEdges <= 153));
    Mask4 = ((BlurWithEdges > 153) & (BlurWithEdges <= 204));
    Mask5 = ((BlurWithEdges > 204) & (BlurWithEdges <= 255));
    BlurWithEdges[Mask2] = 0;
    BlurWithEdges[Mask2] = 51;
    BlurWithEdges[Mask3] = 102;
    BlurWithEdges[Mask4] = 153;
    BlurWithEdges[Mask5] = 204;
    
    #Grey = cv2.cvtColor(Screen, cv2.COLOR_BGR2GRAY);
    #Blur = cv2.adaptiveThreshold(Saturation,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,\
    #        cv2.THRESH_BINARY,11,2)   
    
    cv2.imshow('Hsv', BlurWithEdges)


PreviousSceen = None;

while(True):
    Screen = grab_screen(region = (0, 40, 1024, 768))
    
    #FindDepthUsingSaturation(Screen);
    
    #if cv2.waitKey(25) & 0xFF == ord('q'):
    #    cv2.destroyAllWindows()
    #    break
    
    #continue;
    
    
    ScreenGrey = cv2.cvtColor(Screen,cv2.COLOR_BGR2GRAY)
    #imgL = cv2.imread('tsukuba_l.png',0)
    #imgR = cv2.imread('tsukuba_r.png',0)
    
    if (PreviousSceen is None):
        PreviousSceen = ScreenGrey;
        continue;

    stereo = cv2.StereoBM_create(numDisparities=80, blockSize=45)
    disparity = stereo.compute(PreviousSceen, ScreenGrey)
    
    # Overwirte previous screen
    PreviousSceen = ScreenGrey;
    
    plt.imshow(disparity,'gray')
    plt.ion()
    plt.show()
    plt.draw()
    plt.pause(0.001)
    
    if cv2.waitKey(25) & 0xFF == ord('q'):
        break
    