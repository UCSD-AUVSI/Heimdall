import random
import numpy as np
import cv2
import os

WIDTH = 40
HEIGHT = 40
IMAGE_NAME = "trash"
SAVE_DIRECTORY = "test_images/"
TOTAL_IMAGES = 100

# create save directory
#if not os.path.exists(SAVE_DIRECTORY):
#    os.makedirs(SAVE_DIRECTORY)

def getPoints(padXpixels, padYpixels):
    total_dots = int((random.random())*5)+5
    points=[]
    for i in range(0,total_dots):
        randomx = int(random.random()*(WIDTH-(padXpixels*2)))+padXpixels
        randomy = int(random.random()*(HEIGHT-(padYpixels*2)))+padYpixels
        points.append((randomx,randomy))
    np_points = np.array(points, np.int32)
    #np_points = np_points.reshape((-1,1,2))
    return np_points

def createImage(points):
    # Create a black image
    img = np.zeros((WIDTH,HEIGHT,1), np.uint8)
    thickness = int(random.random()*2)+2
    cv2.polylines(img,[points],True,(255),thickness)
    return img

def createFillImage(points):
    img = np.zeros((WIDTH,HEIGHT,1), np.uint8)
    thickness = random.randint(1,3)
    cv2.polylines(img,[points],True,(255),thickness)
    cv2.fillConvexPoly(img,points,(255)) #todo: fix: not all polygons are convex
					#can do flood fill starting from the edges, marching inwards,
					#then invert that to get filled no matter how nonconvex the polygon was
    return img

def saveImages():
    for i in range(0,TOTAL_IMAGES):
        img = createImage(getPoints())
        cv2.imwrite(SAVE_DIRECTORY+IMAGE_NAME+str(i)+".jpg", img)
    for i in range(TOTAL_IMAGES,2*TOTAL_IMAGES):
        img = createFillImage(getPoints())
        cv2.imwrite(SAVE_DIRECTORY+IMAGE_NAME+str(i)+".jpg", img)

def ReturnOneDrawnPoly():
    padXpixels = random.uniform(2.8,WIDTH/2.9)
    padYpixels = random.uniform(2.8,HEIGHT/2.9)
    trueXpad = round(int(0.78*padXpixels + 0.22*padYpixels))
    trueYpad = round(int(0.78*padYpixels + 0.22*padXpixels))
    if( random.random() < -1.0 ):
        return createImage(getPoints(padXpixels, padYpixels))
    else:
        return createFillImage(getPoints(padXpixels, padYpixels))

def returnImage():
    return ReturnOneDrawnPoly()





