"""
This functions deliver closest matching name for requested RGB color
It matches by Euclidian distance in the RGB space.
Python version 2.7.6
"""
import webcolors
import cv2

#takes in an array
def get_colour_name(requested_colour):
    try:
        closest_name = actual_name = webcolors.rgb_to_name(requested_colour)
    except ValueError:
        closest_name = closest_colour(requested_colour)    

    return closest_name
#This function is called by get_colour_name
def closest_colour(requested_colour):
    min_colours = {}
    
    weights = [i*1.0/sum(requested_colour) for i in requested_colour]
    
    for key, name in webcolors.css3_hex_to_names.items():
        r_c, g_c, b_c = webcolors.hex_to_rgb(key)
        rd = abs((r_c - requested_colour[0]))*weights[0]
        gd = abs((g_c - requested_colour[1]))*weights[1]
        bd = abs((b_c - requested_colour[2]))*weights[2]
        min_colours[(rd + gd + bd)] = name
    return min_colours[min(min_colours.keys())]

"""
#Ask the user for red, green, and blue values
redValue = input("red value: ")
greenValue = input("green value: ")
blueValue = input("blue value: ")

#formatted in red, green, blue
requested_colour = (redValue, greenValue, blueValue)
closest_name = get_colour_name(requested_colour)
#program prints what it thinks the color is
print closest_name
"""

