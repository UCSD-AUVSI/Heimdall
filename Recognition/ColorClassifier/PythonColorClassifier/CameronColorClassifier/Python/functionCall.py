import module

#Ask the user for red, green, and blue values
redValue = input("red value: ")
greenValue = input("green value: ")
blueValue = input("blue value: ")

#formatted in red, green, blue
requested_colour = (redValue, greenValue, blueValue)
closest_name = module.get_colour_name(requested_colour)
#program prints what it thinks the color is
print closest_name