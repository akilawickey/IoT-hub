# import the necessary packages
from picamera.array import PiRGBArray
from picamera import PiCamera
import time
import sys
sys.path.append('/home/pi/opencv/data/haarcascades')


sys.path.append('/usr/local/lib/python2.7/site-packages')
import cv2

import numpy as np
from matplotlib import pyplot as plt	
def nothing(x):
    pass


         
# initialize the camera and grab a reference to the raw camera capture
camera = PiCamera()
camera.resolution = (640, 480)
camera.framerate = 4
rawCapture = PiRGBArray(camera, size=(640, 480))
 
# allow the camera to warmup
time.sleep(0.1)
 
# capture frames from the camera
for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
	# grab the raw NumPy array representing the image, then initialize the timestamp
	# and occupied/unoccupied text
	image = frame.array
 
	# show the frame
	cv2.imshow("Frame", image)
	key = cv2.waitKey(1) & 0xFF
 	cv2.imwrite('hello.jpg', image)
 	
 	
 	img = cv2.imread('hello.jpg')
 	


 	cv2.rectangle(img,(384,200),(510,128),(0,255,0),3)
   
 	
	# clear the stream in preparation for the next frame
	rawCapture.truncate(0)
	

	
		# create trackbars for color change
	cv2.createTrackbar('th1','image',0,255,nothing)
	cv2.createTrackbar('th2','image',0,255,nothing)
	cv2.namedWindow('image')
	
   	# get current positions of four trackbars
  	th1 = cv2.getTrackbarPos('th1','image')
  	th2 = cv2.getTrackbarPos('th2','image')
   	 #apply canny 
   	edges = cv2.Canny(img,th1,th2)
    	 #show the image
       	cv2.imshow(' Edge Ditection ',edges)
    	# Convert BGR to HSV
        hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
	# define range of blue color in HSV
        lower_blue = np.array([0,100,100])
        upper_blue = np.array([255,255,255])

       
	 # Threshold the HSV image to get only blue colors
        mask = cv2.inRange(hsv, lower_blue, upper_blue)


        # Bitwise-AND mask and original image
        res = cv2.bitwise_and(img,img, mask= mask)
	cv2.imshow(' Green Yellow Ditection ',res)
	
 	#img = numpy.array([[1,2,3,4],[2,3,4,5],[3,4,5,6]], 'uint8')
        #imgPIL = Image.fromarray(img)
 	#print imgPIL
	# if the `q` key was pressed, break from the loop
	if key == ord("q"):
		break

