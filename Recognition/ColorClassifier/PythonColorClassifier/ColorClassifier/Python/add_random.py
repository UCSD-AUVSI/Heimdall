#!/usr/bin/python
# -*- coding: iso-8859-1 -*-
from PIL import Image, ImageTk
import cv2
import numpy as np
import pickle
import sys

import Tkinter

COLOR_TO_NUMBER = {"White":1,"Black":2,"Red":3,"Orange":4,"Yellow":5,"Blue":6,"Green":7,"Purple":8,"Pink":9,"Brown":10,"Grey":11,"Teal":12}
#add teal

#save training data
def bgr_to_lab(bgr):
    #create blank image 1x1 pixel
    blank_image=np.zeros((1,1,3),np.uint8)
    #set image pixels to bgr input
    blank_image[:]= bgr
    #turn into LAB
    cielab = cv2.cvtColor(blank_image,cv2.COLOR_BGR2LAB)
    return cielab[0][0]
class color_finder(Tkinter.Tk):
    def __init__(self,parent):
        Tkinter.Tk.__init__(self,parent)
        self.parent = parent
        self.initialize()

    def get_random_image_name(self):
        train_data = np.random.randint(0,255,(1,3))
        output=[]
        for data in train_data:
            #create color imag:
            img = np.zeros((200,200,3), np.uint8)
            color = tuple(reversed(data))
        img[:]=color
        cv2.imwrite("color.jpg",img)
        return color

    def initialize(self):
        try:
            self.output=pickle.load(open("color_db.p","rb"))
        except:
            self.output=[]
        print " White = 1 \n Black = 2 \n Red = 3 \n Orange = 4 \n Yellow = 5 \n Blue = 6 \n Green = 7 \n Purple = 8 \n Pink = 9 \n Brown = 10 \n Grey = 11 \n Teal = 12"
        self.grid()
        self.entryVariable = Tkinter.StringVar()
        self.entry = Tkinter.Entry(self,textvariable=self.entryVariable)
        self.entry.grid(column=0,row=0,sticky='EW')
        self.entry.bind("<Return>", self.OnPressEnter)
        self.entryVariable.set(u"Enter text here.")

        button = Tkinter.Button(self,text=u"End and Save",
                                command=self.OnButtonClick)
        button.grid(column=1,row=0)
        self.color = self.get_random_image_name()
        self.photo=ImageTk.PhotoImage(Image.open("color.jpg"))
        self.labelVariable = Tkinter.StringVar()
        self.label = Tkinter.Label(self,textvariable=self.labelVariable,
                              anchor="w",fg="white",image=self.photo)
        self.label.grid(column=0,row=1,columnspan=2,sticky='EW')

        self.grid_columnconfigure(0,weight=1)
        self.resizable(True,False)
        self.update()
        self.geometry(self.geometry())       
        self.entry.focus_set()
        self.entry.selection_range(0, Tkinter.END)

    def OnButtonClick(self):
        pickle.dump(self.output,open('color_db.p',"wb"))
        sys.exit()

    def OnPressEnter(self,event):
        #self.labelVariable.set( self.entryVariable.get()+" (You pressed ENTER)" )
        num=int(self.entryVariable.get())
        self.output.append({"lab": bgr_to_lab(self.color),"name":num})

        self.entry.focus_set()
        self.color = self.get_random_image_name()
        self.photo=ImageTk.PhotoImage(Image.open("color.jpg"))
        self.label= Tkinter.Label(self,textvariable=self.labelVariable,
                              anchor="w",fg="white",image=self.photo)
        self.label.grid(column=0,row=1,columnspan=2,sticky='EW')

        self.label.focus_set()

if __name__ == "__main__":
    app = color_finder(None)
    app.title('Color Classification')
    app.mainloop()
