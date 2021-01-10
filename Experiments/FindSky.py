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

    return img;#cv2.cvtColor(img, cv2.COLOR_BGRA2RGB)


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





        

def FindFence(Screen):
    Hsv = cv2.cvtColor(Screen, cv2.COLOR_BGR2HSV);
    
    lower_fence = np.array([17, 155, 0],np.uint8)
    upper_fence = np.array([17, 170, 255],np.uint8)
    MaskFence = cv2.inRange(Hsv, lower_fence, upper_fence)
    
    cv2.imshow("MaskFence", MaskFence);
    

def FindShadows(Screen):
    Hsv = cv2.cvtColor(Screen, cv2.COLOR_BGR2HSV);
    Hsv = Hsv[:, :, 2];
    
    _, Thresh = cv2.threshold(Hsv,20,255,cv2.THRESH_BINARY_INV);
    
    kernel = np.ones((3,3),np.uint8)
    ReduceNoise = cv2.erode(Thresh,kernel,iterations = 1)
    ReduceNoise = cv2.dilate(ReduceNoise,kernel,iterations = 1)
    
    cv2.imshow('Shadows', ReduceNoise)

def FindTrees(Screen):
    Grey = cv2.cvtColor(Screen, cv2.COLOR_BGR2GRAY);
    
    Blur = cv2.GaussianBlur(Grey, (5, 5), 0)
    SobelX = cv2.Sobel(Blur,cv2.CV_64F,1,0,ksize=9)
    SobelY = cv2.Sobel(Blur,cv2.CV_64F,0,1,ksize=9)#15
    
    # Source: https://answers.opencv.org/question/234/weird-result-while-finding-angle/
    mag, theta = cv2.cartToPolar(SobelX, SobelY)
    
    # display in HSV so angle has a simple mapping                                   
    theta_hsv = np.zeros((729,1025,3), dtype='f4')                                    
    # Hue is angle in degrees                                                        
    theta_hsv[...,0] = np.degrees(theta)                                             
    # S and V are 1.0                                                                
    theta_hsv[...,1:] = 1.0
    # Perform the colorspace conversion                                              
    # Note that you need to import the old                                           
    # cv modlue to get the conversion constants
    
    frame_threshold = cv2.inRange(theta_hsv, (0, 0, 0), (10, 255, 255))
    frame_threshold2 = cv2.inRange(theta_hsv, (350, 0, 0), (359, 255, 255))
    Mask = cv2.bitwise_or(frame_threshold, frame_threshold2);
    
    theta_bgr = cv2.cvtColor(theta_hsv, cv2.COLOR_HSV2BGR)
    
    new_image = theta_bgr * (Mask[:,:,None].astype(theta_bgr.dtype))
    
    cv2.imshow('SobelY', theta_bgr)#new_image)

def TestColours(Screen):
    Hsv = cv2.cvtColor(Screen, cv2.COLOR_BGR2HSV);
    Hsv = Hsv[:, :, 1];
    
    #Blur = cv2.GaussianBlur(Hsv, (5,5),0)
    
    cv2.imshow('Hsv', Hsv)


def FindGrassSaturation(Screen):
    Hsv = cv2.cvtColor(Screen, cv2.COLOR_BGR2HSV);
    Saturation = Hsv[:, :, 1];
        
    Tolerance = 27;
    lower_green = np.array([48 - Tolerance,0,0])
    upper_green = np.array([48 + Tolerance,255,255])
    
    # Get grass/tree mask to filter future images
    MaskGreen = cv2.inRange(Hsv, lower_green, upper_green)
    
    # Get grass/tree Saturation
    GrassSaturationMask = cv2.bitwise_and(Saturation, MaskGreen);
    
    contours, _ = cv2.findContours(GrassSaturationMask,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    for cnt in contours:
        size = cv2.contourArea(cnt)
        if 5000 < size:         
            # draw the contour and center of the shape on the image
            cv2.drawContours(Screen, [cnt], -1, (0, 255, 0), 2)
    
    
    cv2.imshow('GrassSaturationMask', Screen)
    


def FindDepthUsingSaturation(Screen):
    Hsv = cv2.cvtColor(Screen, cv2.COLOR_BGR2HSV);
    Saturation = Hsv[:, :, 1];
    
    BlurWithEdges = cv2.bilateralFilter(Saturation,9,75,75)
    
    #Edges = cv2.Canny(BlurWithEdges, threshold1=150, threshold2=250)
    
    #Lines = cv2.HoughLinesP(Edges, 1, np.pi/180, 40, np.array([]), minLineLength=150, maxLineGap=10);
    #draw_lines(BlurWithEdges, Lines);
    
    # Round pixel values to make things easier
    #Multiple = 150;
    #BlurWithEdges = np.floor((BlurWithEdges * Multiple) + 0.5) / Multiple;
    Mask1 = (BlurWithEdges <= 51);
    Mask2 = ((BlurWithEdges > 51) & (BlurWithEdges <= 102));
    Mask3 = ((BlurWithEdges > 102) & (BlurWithEdges <= 153));
    Mask4 = ((BlurWithEdges > 153) & (BlurWithEdges <= 204));
    Mask5 = ((BlurWithEdges > 204) & (BlurWithEdges <= 255));
    
    Mask6 = (BlurWithEdges <= 127);
    Mask7 = (BlurWithEdges > 127);
    
    #BlurWithEdges[Mask2] = 0;
    #BlurWithEdges[Mask2] = 0;
    #BlurWithEdges[Mask3] = 0;
    #BlurWithEdges[Mask4] = 255;
    #BlurWithEdges[Mask5] = 255;
    
    BlurWithEdges[Mask6] = 0;
    BlurWithEdges[Mask7] = 255;
    
    #Grey = cv2.cvtColor(Screen, cv2.COLOR_BGR2GRAY);
    #Blur = cv2.adaptiveThreshold(Saturation,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,\
    #        cv2.THRESH_BINARY,11,2)   
    
    #cv2.imshow('Hsv', BlurWithEdges)
    
    return BlurWithEdges;


while (True):
    Screen = grab_screen(region = (0, 40, 1024, 768))
    
    FindTrees(Screen);
    
    #TestColours(Screen);
    
    #FindGrassSaturation(Screen);
    
    #FindFence(Screen);
    
    if cv2.waitKey(25) & 0xFF == ord('q'):
       cv2.destroyAllWindows()
       break
    
    continue;
    
    BlurWithEdges = FindDepthUsingSaturation(Screen);
    
    # Convert BGR to HSV
    Hsv = cv2.cvtColor(Screen, cv2.COLOR_BGR2HSV)
    
    #lower_blue = np.array([10,100,75],np.uint8)
    #upper_blue = np.array([30,200,255],np.uint8) 
    lower_blue = np.array([10,100,75],np.uint8)
    upper_blue = np.array([30,200,255],np.uint8)
    
    # Threshold the HSV image to get only blue colors
    MaskBlue = cv2.inRange(Hsv, lower_blue, upper_blue)
    
    lower_white = np.array([0,0,150]);
    upper_white = np.array([100,100,255]);
    MaskWhite = cv2.inRange(Hsv, lower_white, upper_white)
    
    MaskBlueWhite = cv2.bitwise_or(MaskBlue, MaskWhite);
    MaskBlueWhite = cv2.bitwise_not(MaskBlueWhite); # Flip white and black
    
    # Reduce noise
    kernel = np.ones((3,3),np.uint8)
    ReduceNoiseMaskBlueWhite = cv2.erode(MaskBlueWhite,kernel,iterations = 1)
    ReduceNoiseMaskBlueWhite = cv2.dilate(ReduceNoiseMaskBlueWhite,kernel,iterations = 1)
    
    
    # Combine images
    MaskedDepth = cv2.bitwise_and(ReduceNoiseMaskBlueWhite, BlurWithEdges);
    
    #MaskedDepthEdges = cv2.Sobel(MaskedDepth,cv2.CV_64F,0,1,ksize=1 )
    
    #Blur = cv2.GaussianBlur(MaskedDepth, (5,5),0)
    
    #ReduceNoiseFinal = cv2.dilate(MaskedDepth,kernel,iterations = 8)
    #ReduceNoiseFinal = cv2.erode(ReduceNoiseFinal,kernel,iterations = 8)
    
    #Edges = cv2.Canny(ReduceNoiseFinal, threshold1=150, threshold2=250)
    
    #Lines = cv2.HoughLinesP(Edges, 1, np.pi/180, 90, np.array([]), minLineLength=10, maxLineGap=100);
    #draw_lines(Screen, Lines);
    
    cv2.imshow('MaskBlueWhite', MaskedDepth)#ReduceNoiseFinal)
    
    if cv2.waitKey(25) & 0xFF == ord('q'):
        cv2.destroyAllWindows()
        break
    