import cv2 as cv
import numpy as np
#import math
# import matplotlib.pyplot as plt

def do_canny(frame):
    gray = cv.cvtColor(frame, cv.COLOR_RGB2GRAY)
    blur = cv.GaussianBlur(gray, (5, 5), 0)
    canny = cv.Canny(blur, 50, 150)
    return canny

def do_segment(frame):
    height = frame.shape[0]
    polygons = np.array([
                            [(0, height), (800, height), (380, 290)]
                        ])
    mask = np.zeros_like(frame)
    cv.fillPoly(mask, polygons, 255)
    segment = cv.bitwise_and(frame, mask)
    return segment

def calculate_lines(frame, lines):
    # Empty arrays to store the coordinates of the left and right lines
    left = []
    right = []
    # Loops through every detected line
    for line in lines:
        # Reshapes line from 2D array to 1D array
        x1, y1, x2, y2 = line.reshape(4)
        # Fits a linear polynomial to the x and y coordinates and returns a vector of coefficients which describe the slope and y-intercept
        parameters = np.polyfit((x1, x2), (y1, y2), 1)
        slope = parameters[0]
        y_intercept = parameters[1]
        # If slope is negative, the line is to the left of the lane, and otherwise, the line is to the right of the lane
        if slope < 0:
            left.append((slope, y_intercept))
        else:
            right.append((slope, y_intercept))
    # Averages out all the values for left and right into a single slope and y-intercept value for each line
    left_avg = np.average(left, axis = 0)
    right_avg = np.average(right, axis = 0)
    # Calculates the x1, y1, x2, y2 coordinates for the left and right lines    
    if (left_avg.size < 2):
        left_line = np.array([0,0,0,0]);
        print("csacascsa")
    else:
        left_line = calculate_coordinates(frame, left_avg)
    
    if (right_avg.size < 2):
        right_line = np.array([0,0,0,0]);
    else:
        right_line = calculate_coordinates(frame, right_avg)
        
    return np.array([left_line, right_line])

def calculate_coordinates(frame, parameters):
    slope, intercept = parameters
    # Sets initial y-coordinate as height from top down (bottom of the frame)
    y1 = frame.shape[0]
    # Sets final y-coordinate as 150 above the bottom of the frame
    y2 = int(y1 - 150)
    # Sets initial x-coordinate as (y1 - b) / m since y1 = mx1 + b
    x1 = int((y1 - intercept) / slope)
    # Sets final x-coordinate as (y2 - b) / m since y2 = mx2 + b
    x2 = int((y2 - intercept) / slope)
    return np.array([x1, y1, x2, y2])

def visualize_lines(frame, lines):
    # Creates an image filled with zero intensities with the same dimensions as the frame
    lines_visualize = np.zeros_like(frame)
    # Checks if any lines are detected
    if lines is not None:
        for x1, y1, x2, y2 in lines:
            # Draws lines between two coordinates with green color and 5 thickness
            cv.line(lines_visualize, (x1, y1), (x2, y2), (0, 255, 0), 5)
    return lines_visualize
    
def DrawHoughLines(img, lines):
    try:
        for line in lines:
            coords = line[0];
            LeftPoint = (coords[0],coords[1]);
            RightPoint = (coords[2],coords[3]);
            
            cv.line(img, LeftPoint, RightPoint, [255,255,255], 1);
    except:
        print("error");
        pass
    

def Video():
    cap = cv.VideoCapture("input.mp4")
    while (cap.isOpened()):
        ret, frame = cap.read()
        #print(frame.size);
        canny = do_canny(frame)
        # plt.imshow(frame)
        # plt.show()
        segment = do_segment(canny)
        houghLines = cv.HoughLinesP(segment, 2, np.pi / 180, 100, np.array([]), minLineLength = 100, maxLineGap = 50)

        DrawHoughLines(frame, houghLines);
        cv.imshow("output", frame)
        
        # Averages multiple detected lines from hough into one line for left border of lane and one line for right border of lane
        #lines = calculate_lines(frame, houghLines)
        # Visualizes the lines
        #lines_visualize = visualize_lines(frame, lines)
        # Overlays lines on frame by taking their weighted sums and adding an arbitrary scalar value of 1 as the gamma argument
        #output = cv.addWeighted(frame, 0.9, lines_visualize, 1, 1)
        # Opens a new window and displays the output frame
        #cv.imshow("output", output)

        if cv.waitKey(10) & 0xFF == ord('q'):
            break

    cap.release()
    cv.destroyAllWindows()

    
def Image():
    img = cv.imread("C:/Users/alime/Pictures/MachineLearning/Games/LaneDetectionImage.jpg")
    cv.resize(img, (1920, 1080), interpolation=cv.INTER_LINEAR)
    print(type(img));
    canny = do_canny(img)
    # plt.imshow(img)
    # plt.show()
    segment = do_segment(canny)
    hough = cv.HoughLinesP(segment, 2, np.pi / 180, 100, np.array([]), minLineLength = 100, maxLineGap = 50)

    DrawHoughLines(img, hough);
    cv.imshow("output", img);
    
    # Averages multiple detected lines from hough into one line for left border of lane and one line for right border of lane
    #lines = calculate_lines(img, hough)
    # Visualizes the lines
    #lines_visualize = visualize_lines(img, lines)
    # Overlays lines on frame by taking their weighted sums and adding an arbitrary scalar value of 1 as the gamma argument
    #output = cv.addWeighted(img, 0.9, lines_visualize, 1, 1)
    # Opens a new window and displays the output frame
    #cv.imshow("output", output)
    cv.waitKey(0)
    
def main():
    Image();
    #Video();

if __name__ == "__main__":
    main();