import numpy as np
import cv2
import pykmeansppcpplib
import libpypolyshaperec
import time
import os
dirname = 'test 6-13'
dirname = 'junk char'
#one in pywork
#kmeans function
"""
def kmeans(img,numklust):
	#start kmeans
	Z = img.reshape((-1,3))
	Z = np.float32(Z)
	criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 35, .1)
	K = numklust
	ret,label,center = cv2.kmeans(Z,K,criteria,10,cv2.KMEANS_RANDOM_CENTERS)

	# Now convert back into uint8, and make original image
	center = np.uint8(center)
	res = center[label.flatten()]
	res2 = res.reshape((img.shape))
	#cv2.imshow('3.Color quantized image (in CIELab space)',res2)
	#end kmeans

	#back to BGR space
	res2=cv2.cvtColor(res2,cv2.COLOR_LAB2BGR)

	return res2
#"""
#get color of clusters in shape
def getColor(ShapeMask,CharMask, original, isShape,shapecolor):
	#get the colorfS of each cluster
	listR = []
	listG = []
	listB = []
	#listp = []
	
	if(isShape):
		for i in range(0,original.shape[0]):
	
			for j in range(0,original.shape[1]):
	
				if(ShapeMask[i,j,]==1 and CharMask[i,j,]==0):
					listR.append(original[i,j,2])
					listG.append(original[i,j,1])
					listB.append(original[i,j,0])
	else:
		
		kern=cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(8,8))
		charEM=cv2.erode(CharMask,kern,iterations=1)
	
		for i in range(0,original.shape[0]):
	
			for j in range(0,original.shape[1]):
	
				dist = abs(shapecolor[2]-original[i,j,0])+abs(shapecolor[1]-original[i,j,1])+abs(shapecolor[0]-original[i,j,2])
				
				
				if(charEM[i,j,]==1 and dist>74):
					#listp.append([original[i,j,2],original[i,j,1],original[i,j,0]])
					listR.append(original[i,j,2])
					listG.append(original[i,j,1])
					listB.append(original[i,j,0])
		
	listB.sort()
	listG.sort()
	listR.sort()
	"""print len(listp)
	print listp
	if(not isShape):
		greatest = 0
		gdist = 100000000000000000000000
		for x in range(0,len(listp)):
			dist = 0
			for y in range(0,len(listp)):
				check = listp[x]
				other = listp[y]
				dist+=abs(check[2]-other[2])
				dist+=abs(check[1]-other[1])
				dist+=abs(check[0]-other[0])
			if dist < gdist:
				greatest = x
				gdist = dist
		print gdist
		return(listp[greatest][0],listp[greatest][1],listp[greatest][2])
	"""
	#print listR
	#print listG
	#print listB
	#print
	color= (listR[len(listR)/2],listG[len(listG)/2],listB[len(listB)/2])
	return color





"""
#make Masks
def makeMasks(res2,numKlusters, color):
	#make a mask for each cluster, get size of each, get sum of pixel values for test 3
	m1=np.zeros((res2.shape[0],res2.shape[1],1),np.uint8)
	m2=np.zeros((res2.shape[0],res2.shape[1],1),np.uint8)
	m3=np.zeros((res2.shape[0],res2.shape[1],1),np.uint8)
	m4=np.zeros((res2.shape[0],res2.shape[1],1),np.uint8)
	m5=np.zeros((res2.shape[0],res2.shape[1],1),np.uint8)
	m6=np.zeros((res2.shape[0],res2.shape[1],1),np.uint8)
	c1=c2=c3=c4=c5=c6=0
	for i in range(0,res2.shape[0]):

		for j in range(0,res2.shape[1]):

			if((res2[i,j,]==color[0]).all()):
				m1[i,j,0]=1
				c1+=1

			if((res2[i,j,]==color[1]).all()):
				m2[i,j,0]=1
				c2+=1

			if((res2[i,j,]==color[2]).all()):
				m3[i,j,0]=1
				c3+=1
			if(numKlusters>3 and (res2[i,j,]==color[3]).all()):
				m4[i,j,0]=1
				c4+=1
			if(numKlusters>4 and (res2[i,j,]==color[4]).all()):
				m5[i,j,0]=1
				c5+=1
			if(numKlusters>5 and (res2[i,j,]==color[5]).all()):
				m6[i,j,0]=1
				c6+=1
	clusterMaskss=(m1,m2,m3,m4,m5,m6)
	cowunts=(c1,c2,c3,c4,c5,c6)

	return clusterMaskss,cowunts
	"""
#Find how many pixels of each cluster touch the perimeter
#"""
def numPixTouchPerm(clusterMaskss, vert, hori, numKlusters):
	m1=clusterMaskss[0]
	m2=clusterMaskss[1]
	m3=clusterMaskss[2]

	#m4=clusterMaskss[3]
	#m5=clusterMaskss[4]
	#m6=clusterMaskss[5]

	perM1= perM2=perM3=0.0

	for i in range(0,vert):
		perM1= perM1 + m1[i,0,] +m1[i,hori-1,]
		perM2= perM2 + m2[i,0,] +m2[i,hori-1,]
		perM3= perM3 + m3[i,0,] +m3[i,hori-1,]

		#perM4= perM4 + m4[i,0,] +m4[i,hori-1,]
		#perM5= perM5 + m5[i,0,] +m5[i,hori-1,]

	for i in range(0,hori):
		perM1= perM1 + m1[0,i,] +m1[vert-1,i,]
		perM2= perM2 + m2[0,i,] +m2[vert-1,i,]
		perM3= perM3 + m3[0,i,] +m3[vert-1,i,]

		#perM4= perM4 + m4[0,i,] +m4[vert-1,i,]
		#perM5= perM5 + m5[0,i,] +m5[vert-1,i,]
		#perM6= perM6 + m6[0,i,] +m6[vert-1,i,]


	return perM1,perM2,perM3
	#"""

def doSegmentation(cropImg, optionalArgs, tc=None):
	start=time.time()
	imrows,imcols = cropImg.shape[:2]
	print "Monal-python-segmentation is processing an image of size: " + str(imcols) + "x" + str(imrows)
	print "note: currently this only returns color-clustered images, not the shape or char masks"
	#print "kmeans 1, start"	
	#print start-start
	#-------------------------------------------------------------------------
	lim = cropImg
	origIMG=np.copy(lim)
	originalLimShape = lim.shape
	#cv2.imshow("1.Original Image", lim)
	lab=np.copy(cv2.cvtColor(lim,cv2.COLOR_BGR2LAB))
	labfor2=np.copy(cv2.cvtColor(lim,cv2.COLOR_BGR2LAB))
	#cv2.imshow("1A.CIE LAB", lab)
	labf=cv2.bilateralFilter(lab,15,50,15)
	#cv2.imshow("2.Bilateral FIlter", labf)
	#15,65,15 for bilat 2/3, orig val i chose

	#15,65,15 for bilat 2/3, orig val i chose



	allones=np.ones((lim.shape[0],lim.shape[1],1),np.uint8)


	"""
	print color1
	print color2
	print color3
	print res2.shape
	"""
	cropfirstRun32=np.float32(labf)/255
	firstRunShapeTupleResults = pykmeansppcpplib.ClusterKmeansPPwithMask(cropfirstRun32,allones,3,24,24,False,-1)
	#color=firstRunShapeTupleResults[1]
	clusterMaskss =firstRunShapeTupleResults[2]
	res2=firstRunShapeTupleResults[0]

	#clusterMa,cowunts=makeMasks(res2, 3, color)
	#cv2.imshow("1st clustered color",temmp)

	vert=res2.shape[0]
	hori=res2.shape[1]
	tot_per=vert*2+hori*2
	tot_area=vert*hori


	m1=clusterMaskss[0]
	m2=clusterMaskss[1]
	m3=clusterMaskss[2]
	#Find how many pixels of each cluster touch the perimeter
	perM1,perM2,perM3= numPixTouchPerm(clusterMaskss, vert, hori, 3)
	cowunts=[0,0,0,0]

	for kk in range(0,3):
		cowunts[kk]=cv2.countNonZero(clusterMaskss[kk])

	maskPixelFraction=np.zeros(6,np.uint8)
	
	
	for i in range(0,3):
		maskPixelFraction[i]=100*cowunts[i]/tot_per

	pAm1=maskPixelFraction[0]
	pAm2=maskPixelFraction[1]
	pAm3=maskPixelFraction[2]
	
	
	#Set booleans for masks, eliminate through tests
	bul1=bul2=bul3=True

	#Test 1A: Perimeter test (if too many pixels touch perimeter relative to total perimeter, trash mask)
	per1TH=.14
	#14% was val auvsi previously used
	if((perM1/tot_per)>per1TH or not bul1):
		bul1=False
	if((perM2/tot_per)>per1TH or not bul2):
		bul2=False
	if((perM3/tot_per)>per1TH or not bul3):
		bul3=False
	"""
	print "perm test A"
	print bul1
	print bul2
	print bul3
	#"""
	#"""
	#Test 1B: Perimeter test B (if too many pixels touch perimeter relative to mask size, trash mask)
	per2TH=.006
	#.004 was val auvsi previously used
	if((perM1/cowunts[0])>per2TH or not bul1):
		bul1=False
	if((perM2/cowunts[1])>per2TH or not bul2):
		bul2=False
	if((perM3/cowunts[2])>per2TH or not bul3):
		bul3=False

	#"""
	"""
	print "perm test B"
	print bul1
	print bul2
	print bul3
	#"""

	#"""
	#Test 2: Cluster size (if x<4% of img size, trash it)
	test2TH=4
	#maybe increase TH
	if(pAm1<test2TH and (bul1+bul2+bul3!=1)):
		bul1=False
	if(pAm2<test2TH and (bul1+bul2+bul3!=1)):
		bul2=False
	if(pAm3<test2TH and (bul1+bul2+bul3!=1)):
		bul3=False
	barely_passSize=[pAm1-3,pAm2-3,pAm3-3]
	#"""
	"""
	print "Cluster Size test"
	print bul1
	print bul2
	print bul3
	print pAm2
	print pAm3
	#"""
	#print "kmeans 1 end, start contour 1"
	#print time.time()-start
	buuL=[bul1,bul2,bul3]
	clusterMasks=(m1,m2,m3)
	"""
	for i in range(0,3):
		cv2.imshow("1st mask "+str(i),clusterMasks[i]*255)
	#"""

	
	
	
	#blank masks for drawing contours
	cmm1=np.zeros((lim.shape[0],lim.shape[1],1),np.uint8)
	cmm2=np.zeros((lim.shape[0],lim.shape[1],1),np.uint8)
	cmm3=np.zeros((lim.shape[0],lim.shape[1],1),np.uint8)
	keep=(cmm1,cmm2,cmm3)

	#variable to find bounding rect for contour
	x=np.zeros((3),np.uint8)
	y=np.zeros((3),np.uint8)
	w=np.zeros((3),np.uint8)
	h=np.zeros((3),np.uint8)
	temp=np.zeros((4),np.uint8)


	#numCont is the total number of contours in image, contbool is to see if contour is still considered,
	#contareas is area of biggest cont, contcirc is circularity of cont with biggest area,
	#j is a counter
	numCont=np.zeros(3,np.uint8)
	contbool=np.array([False,False,False])
	contareas=np.array([0.0,0.0,0.0])
	contcirc=np.array([0.0,0.0,0.0])
	j=0
	artot=0
	for clusters in clusterMasks:
		(cnts,_)=cv2.findContours(clusters.copy(),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_NONE)
		#print "Cont "+ str(j+1)
		#print len(cnts)
		artot=0
		for i in range (len(cnts)):

			perimC=cv2.arcLength(cnts[i],True)
			ar= cv2.contourArea(cnts[i])
			dem=(np.power(perimC,2))
			numer=4.0*3.14159265*cv2.contourArea(cnts[i])
			artot=artot+ar
			if(dem!=0):
				circularity= numer/dem


				if(.0146*tot_area<=ar and ar<=.83*tot_area):

					if(circularity>.08 or (artot==0 and buuL[j]==1)):
						#print "Circularity: " + str(circularity)
						#print "Area: " +str(ar/tot_area)

						#Contour test (Num pixel of filled cont. within perm)/(cont area)
						permTouch=0.0
						tempmask = np.zeros(m1.shape,np.uint8)
						cv2.drawContours(tempmask,cnts,i,1,-1)


						for ik in range(0,vert):
							for m in range(0,int(.065*hori)):
								permTouch= permTouch + tempmask[ik,m,] + tempmask[ik,hori-1-m,]

						for ik in range(0,hori):
							for m in range(0,int(.065*vert)):
								permTouch= permTouch + tempmask[m,ik,] + tempmask[vert-1-m,ik,]



						if(permTouch/ar<.023):
							"""							
							print permTouch/ar
							print permTouch
							print ar
							#"""
							temp[0]=x[j]
							temp[1]=y[j]
							temp[2]=w[j]
							temp[3]=h[j]

							x[j],y[j],w[j],h[j] = cv2.boundingRect(cnts[i])

							cv2.drawContours(keep[j],cnts,i,1, -1 )
							numCont[j]=numCont[j]+1
							contbool[j]=True
							#print "contbool: " +str(j)
							#print contbool[j]
							contareas[j]= ar if (ar>contareas[j]) else contareas[j]
							contcirc[j]= circularity

							if(numCont[j]==1):
								temp[0]=x[j]
								temp[1]=y[j]
								temp[2]=w[j]
								temp[3]=h[j]


							h[j]= ((int(y[j])+int(h[j])) if (int(y[j])+int(h[j])>int(temp[1])+int(temp[3])) else (int(temp[1])+int(temp[3]))) -(int(y[j]) if(int(y[j])<int(temp[1]) or numCont[j]==1) else int(temp[1]))
							w[j]= ((int(x[j])+int(w[j])) if (int(x[j])+int(w[j])>int(temp[0])+int(temp[2])) else (int(temp[0])+int(temp[2]))) -(int(x[j]) if(int(x[j])<int(temp[0]) or numCont[j]==1) else int(temp[0]))
							x[j]= x[j] if(x[j]<temp[0] or numCont[j]==1) else temp[0]
							y[j]= y[j] if(y[j]<temp[1] or numCont[j]==1) else temp[1]
							temp=np.zeros((4),np.uint8)
							if(int(w[j])+int(x[j])>hori):
								w[j]=hori-int(x[j])
							if(int(h[j])+int(y[j])>vert):
								h[j]=vert-int(y[j])




		j=j+1

	#print ("test!!!!!")
	global indexofShapeTrue 
	global gotShape
	gotShape = False
	oo = 0;
	indexOfShapeTrue = 0
	#shaperec test
	for i in range(0,3):
		srtest = libpypolyshaperec.doBPyShapeRec([clusterMasks[i]*255])
		print (srtest+ ":  " + str(i))
		if(srtest):
			oo = oo+1
			indexOfShapeTrue= i
			print ("index: " + str(indexOfShapeTrue))
	if(oo==1):
		print "wtf"
		gotShape=True
		for i in range(0,3):
			if(i!=indexOfShapeTrue):
				contbool[i]=False
				buuL[i]=False
			else:
				contbool[i]=True
				buuL[i]=True
	
	for i in range(0,3):
		print contbool[i]
	# A bunch of test to eliminate and narrow the number of contour masks to one
	if(int(contbool[0])+int(contbool[1])+int(contbool[2])!=1):
		"""
		print contbool[0]
		print contbool[1]
		print contareas[1]
		print contcirc[1]
		print contbool[2]
		print contareas[2]
		print contcirc[2]
		#"""
		for i in range(0,2):
			if((contareas[0]>contareas[i+1]*2.28 and contbool[0] and contbool[i+1] and (contcirc[0]>contcirc[i+1]*.87 or contareas[0]>contareas[i+1]*3.4 )) or (contcirc[0]>contcirc[i+1]*1.8 and contbool[0] and contbool[i+1]and 2.4*contareas[0]>contareas[i+1])):
				contbool[i+1]=False
				numCont[i+1]=0
			if((contareas[2]>contareas[i]*2.28 and contbool[2] and contbool[i] and (contcirc[2]>contcirc[i]*.87 or contareas[2]>contareas[i]*3.4 )) or (contcirc[2]>contcirc[i]*1.8 and contbool[2] and contbool[i] and 2.4*contareas[2]>contareas[i])):
				contbool[i]=False
				numCont[i]=0
		"""
		print contbool[1]
		print contbool[2]
		#"""
		if((contareas[1]>contareas[0]*2.28 and contbool[1] and contbool[0] and (contcirc[1]>contcirc[0]*.87 or contareas[1]>contareas[0]*3.4 )) or (contcirc[1]>contcirc[0]*1.8 and contbool[1] and contbool[0] and 2.4*contareas[1]>contareas[0])):
			contbool[0]=False
			numCont[0]=0
		if((contareas[1]>contareas[2]*2.28 and contbool[1] and contbool[2] and (contcirc[1]>contcirc[2]*.87 or contareas[1]>contareas[2]*3.4) ) or (contcirc[1]>contcirc[2]*1.8 and contbool[1] and contbool[2] and 2.4*contareas[1]>contareas[2])):
			contbool[2]=False
			numCont[2]=0

		if(int(contbool[0])+int(contbool[1])+int(contbool[2])==2 and buuL[0]+buuL[1]+buuL[2]==1):

			if(buuL[0]):
				contareas[1]=0
				contareas[2]=0
				contbool[1]=False
				contbool[2]=False
				numCont[1]=0
				numCont[2]=0
			if(buuL[1]):
				contareas[0]=0
				contareas[2]=0
				contbool[0]=False
				contbool[2]=False
				numCont[0]=0
				numCont[2]=0
			if(buuL[2]):
				contareas[0]=0
				contareas[1]=0
				contbool[0]=False
				contbool[1]=False
				numCont[0]=0
				numCont[1]=0
		#print numCont[1]
		#print numCont[2]

		#need to add another condition just incase their is 2 cont + 2 mask and they both are small
		if(int(contbool[0])+int(contbool[1])+int(contbool[2])==2 ):
			if(barely_passSize[0]<3.5):
				contbool[0]=False
				numCont[0]=0
			if(barely_passSize[1]<3.5):
				contbool[1]=False
				numCont[1]=0
			if(barely_passSize[2]<3.5):
				contbool[2]=False
				numCont[2]=0

		# might not be such a good idea
		#"""
		if(int(contbool[0])+int(contbool[1])+int(contbool[2])==2 and buuL[0]+buuL[1]+buuL[2]==2):

			for i in range(0,2):
				if((contareas[0]>contareas[i+1])):
					contbool[i+1]=False
					numCont[i+1]=0
				if((contareas[2]>contareas[i])):
					contbool[i]=False
					numCont[i]=0
			if((contareas[1]>contareas[0])):
				contbool[0]=False
				numCont[0]=0
			if((contareas[1]>contareas[2])):
				contbool[2]=False
				numCont[2]=0
		#"""


	keep=(keep[0],keep[1],keep[2])

	#Find the right mask number and use one of the cluster masks if there are 0 acceptable contour masks
	maskNumber=7

	for i in range(0,3):
		if(contbool[i]):
			maskNumber =i
		#cv2.imshow("1kaftercont "+str(i),keep[i]*255)
		

		
	if (maskNumber==7):
		keep=list(keep)
		for i in range(0,3):
			if(buuL[i]):
				maskNumber =i
				keep[maskNumber]=clusterMasks[i]*255
				contbool[i]=True
		
	#print "coutour 1 end, start kmeans 2"
	#print time.time()-start
	
	
	if(maskNumber==7):
		return (np.zeros((10,10,1),np.uint8), (0,0,0), np.zeros((10,10,1),np.uint8), (0,0,0), "")
	
	
	
	"""
	#if 4 cont in mask morph image appropriately
	if(maskNumber!=7 and (numCont[maskNumber]>1 or (contcirc[maskNumber]<.16 and contcirc[maskNumber]!=0))):
		kernel=cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(12,12))
		print "wtf!!"
		if(contbool[0]):
			cmm1=keep[0]
			cmm1=cv2.morphologyEx(cmm1, cv2.MORPH_CLOSE, kernel)
		if(contbool[1]):
			cmm2=keep[1]
			cmm2=cv2.morphologyEx(cmm2, cv2.MORPH_CLOSE, kernel)
		if(contbool[2]):
			cmm3=keep[2]
			cmm3=cv2.morphologyEx(cmm3, cv2.MORPH_CLOSE, kernel)
		keep=(cmm1,cmm2,cmm3)
	#"""

	
	"""
	#Display mask
	for i in range(0,3):
		if(contbool[i]):
			#cv2.rectangle(keep[i],(int(x[i]),int(y[i])),(int(x[i])+int(w[i]),int(y[i])+int(h[i])),(255,0,0),2)
			cv2.imshow(str(1+i)+"cont",keep[i])
	#"""
	findcont2ndkmean=np.copy((keep[maskNumber]).copy())
	extraErode=False
	#"""
	if(numCont[maskNumber]>1):
		#print "over 2 contours"
		keal=cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(12,12))
		findcont2ndkmean=cv2.dilate(findcont2ndkmean.copy(),keal,iterations=1)
		extraErode=True
	#"""
	KEq2=True;
	#convexhull
	if((numCont[maskNumber]>1 or contcirc[maskNumber]<.16) and (not gotShape or numCont[maskNumber]!=1)):
		hulll=np.zeros((lim.shape[0],lim.shape[1],1),np.uint8)
		(kontours,_)=cv2.findContours(findcont2ndkmean.copy(),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_NONE)
		KEq2=False;
		#print "KEq2=False"
		if(len(kontours)==0):
			return (np.zeros((10,10,1), np.uint8), (0,0,0),np.zeros((10,10,1),np.uint8), (0,0,0), "")
		unicon=np.vstack(kontours)
		convex=cv2.convexHull(unicon,False)
		cv2.drawContours(hulll,[convex],0,1,-1)
	else:
		hulll=np.copy(keep[maskNumber])

	sideVer=lim.shape[0]
	factorMag=(3 if (300.0/sideVer>=2.5) else 2) if (300.0/sideVer>=1.5) else 1
	#factorMag=3
	hulll2=cv2.resize(hulll,(originalLimShape[1]*factorMag,originalLimShape[0]*factorMag),0,0)
	resizedOrig=cv2.resize(origIMG,(originalLimShape[1]*factorMag,originalLimShape[0]*factorMag),0,0)
	resizedOrigL=cv2.resize(lab,(originalLimShape[1]*factorMag,originalLimShape[0]*factorMag),0,0)
	if(extraErode==False):
		kernnal=cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(17,17))
		hulll2=cv2.erode(hulll2,kernnal,iterations=1)
	else:
		kernnal=cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(22,22))
		hulll2=cv2.erode(hulll2,kernnal,iterations=1)
	
	


	#labf2=cv2.bilateralFilter(lab,-1,3,70)

	lab2=cv2.resize(labfor2,(originalLimShape[1]*factorMag,originalLimShape[0]*factorMag),0,0,cv2.INTER_CUBIC)
	#lab2=cv2.bilateralFilter(lab2,20,60,20)
	cropf32=np.float32(lab2)/255

	shapeSegMask=hulll2
	
	PrintUsefulKMeansInfoToConsole = False
	
	"""
  	firstRunShapeTupleResults = pykmeansppcpplib.ClusterKmeansPPwithMask(cropfirstRun32,allones,3,14,24,False)
 	color=firstRunShapeTupleResults[1]
 	clusterMaskss =firstRunShapeTupleResults[2]
 	res2=firstRunShapeTupleResults[0]
	"""
	noCores=cv2.countNonZero(shapeSegMask)
	
	if(noCores<100):
		return	(np.zeros((10,10,1),np.uint8),(0,0,0), np.zeros((10,10,1),np.uint8), (0,0,0), "")
		 
	if(KEq2):
		jjj=2		
		shapeSegTupleResults = pykmeansppcpplib.ClusterKmeansPPwithMask(cropf32,shapeSegMask,2,14,24,PrintUsefulKMeansInfoToConsole,-1)
	else:
		shapeSegTupleResults = pykmeansppcpplib.ClusterKmeansPPwithMask(cropf32,shapeSegMask,3,14,24,False,-1)
		jjj=3
	shapeSeg = shapeSegTupleResults[0]
	
	#shapeSeg=pykmeansppcpplib.ClusterKmeansPPwithMask(shapeSeg,shapeSegMask,6,14,24,PrintUsefulKMeansInfoToConsole)
	#shapeSeg=pykmeansppcpplib.ClusterKmeansPPwithMask(shapeSeg,shapeSegMask,3,14,24,PrintUsefulKMeansInfoToConsole)
	#shapeSeg=pykmeansppcpplib.ClusterKmeansPPwithMask(shapeSeg,shapeSegMask,2,14,24,PrintUsefulKMeansInfoToConsole)
	"""	
	cv2.imshow("lab",lab2)
	cv2.imshow("color_clustered",shapeSeg)
	for ii in range(len(shapeSegTupleResults[1])):
		cv2.imshow("maskss"+str(ii), shapeSegTupleResults[2][ii]*255)
	#"""
	writingImg=np.uint8(shapeSeg*255)
	#writingImg=cv2.cvtColor(writingImg,cv2.COLOR_LAB2BGR)

	charMasks=shapeSegTupleResults[2]
	charCowunts=[0,0,0,0]

	for kk in range(0,jjj):
		charCowunts[kk]=cv2.countNonZero(charMasks[kk])


	#cv2.imshow("cpp kmean",writingImg)
	cm1=np.zeros((writingImg.shape[0],writingImg.shape[1],1),np.uint8)
	cm2=np.zeros((writingImg.shape[0],writingImg.shape[1],1),np.uint8)
	cm3=np.zeros((writingImg.shape[0],writingImg.shape[1],1),np.uint8)

	charKeep=(cm1,cm2,cm3)

	charNumCont=np.zeros(4,np.uint8)

	charContbool=np.array([False,False,False,False])
	charContareas=np.array([0.0,0.0,0.0,0.0])
	charContcirc=np.array([0.0,0.0,0.0,0.0])
	#print "end kmeans 2, start contour 2"
	#print time.time()-start
	#tot_area=writingImg.shape[0]*writingImg.shape[1]
	j=0
	tot_area=writingImg.shape[0]*writingImg.shape[1]
	for clusters in charMasks:
		(cnts,_)=cv2.findContours(clusters.copy(),cv2.RETR_CCOMP,cv2.CHAIN_APPROX_NONE)
		#print "Cont "+ str(j+1)
		#print len(cnts)
		artot=0
		for i in range (len(cnts)):

			perimC=cv2.arcLength(cnts[i],True)
			ar= cv2.contourArea(cnts[i])
			dem=(np.power(perimC,2))
			numer=4.0*3.14159265*cv2.contourArea(cnts[i])
			artot=artot+ar
			if(dem!=0):
				circularity= numer/dem


				if(.0038*tot_area<=ar and ar<=.5*tot_area):
					if(circularity>.075):
						#print "Circularity: " + str(circularity)
						#print "Area: " +str(ar/tot_area)

						#Contour test (Num pixel of filled cont. within perm)/(cont area)
						#permTouch=0.0
						#tempmask = np.zeros(m1.shape,np.uint8)
						#cv2.drawContours(tempmask,cnts,i,1,-1)

						"""
						for ik in range(0,vert):
							for m in range(0,int(.065*hori)):
								permTouch= permTouch + tempmask[ik,m,] + tempmask[ik,hori-1-m,]

						for ik in range(0,hori):
							for m in range(0,int(.065*vert)):
								permTouch= permTouch + tempmask[m,ik,] + tempmask[vert-1-m,ik,]



						if(permTouch/ar<.023):
							print permTouch/ar
							print permTouch
							print ar

						"""
						cv2.drawContours(charKeep[j],cnts,i,1, -1 )
						charNumCont[j]=charNumCont[j]+1
						charContbool[j]=True
						charContareas[j]= ar if (ar>charContareas[j]) else charContareas[j]
						charContcirc[j]= circularity
		j=j+1
	charKeep=list(charKeep)
	smallest=charCowunts[0]
	charMaskNum=0
	if(KEq2):
		
		for i in range(0,2):
			if(charCowunts[i]<smallest and charCowunts[i]!=0):
				smallest=charCowunts[i]
				charMaskNum=i
			#cv2.imshow("charmask"+str(i),charKeep[i]*255)
	else:
		for i in range(0,3):
			if(charCowunts[i]<smallest and charCowunts[i]!=0):
				smallest=charCowunts[i]
				charMaskNum=i
			#cv2.imshow("charmask"+str(i),charKeep[i]*255)

	#shaype=cv2.add(keep[maskNumber],charKeep[charMaskNum])
	#keep[maskNumber]=keep[maskNumber]+charKeep[charMaskNum]
	#keep=tuple(keep)
	#cv2.imshow("c++ cluster",writingImg)
	#cv2.imshow("winner",charMasks[charMaskNum]*255)
	#print "end contour 2, start kmeans 3"
	#print time.time()-start

	shaype=cv2.resize( np.copy(keep[maskNumber]),(originalLimShape[1]*factorMag,originalLimShape[0]*factorMag),0,0,cv2.INTER_CUBIC)
	
	shaype=cv2.bitwise_or(shaype,np.copy(charKeep[charMaskNum]))
	temPm=np.copy(charKeep[charMaskNum])
	charSeg=np.copy(charKeep[charMaskNum])
	
	#noisedel=np.zeros((charMasks[charMaskNum].shape[0],charMasks[charMaskNum].shape[1],1),np.uint8)
	noisedel=charMasks[charMaskNum]
	cv2.bitwise_not(np.copy(charMasks[charMaskNum]),noisedel,np.copy(temPm))	
	"""
	cv2.imshow("cseg1",temPm*255)	
	cv2.imshow("csegn",charSeg*255)
	cv2.imshow("noisen",noisedel*255)	
	cv2.waitKey(0)
	#"""
	cv2.bitwise_not(np.copy(charMasks[charMaskNum]),charSeg,np.copy(noisedel))
	if(not KEq2):
		kernnal=cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5))
		shaype=cv2.dilate(shaype,kernnal,iterations=1)

	shapeSeg=shaype
	shaperec=np.copy(shaype)*255
	shapefound = libpypolyshaperec.doBPyShapeRec([shaperec])
	
	#print("shapefound == \'"+shapefound+"\'")
	
	
	
	
	#-------------------------------------------------------------------------
	
	#the cluster function messes up this original image, so make a copy
	kernnal=cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(2,2))
	
	#charSeg=np.uint8(charSeg)
	testsizeofcseg=cv2.countNonZero(charSeg)
	#print testsizeofcseg
	if(testsizeofcseg<40):
		charSeg=charMasks[charMaskNum]
	
	
	cropp32=np.float32(lab2)/255
	#"""
	#print cropp32.shape
	
	
	thirdRunKmeansSegTuple = pykmeansppcpplib.ClusterKmeansPPwithMask(cropp32,charSeg,2,24,24,PrintUsefulKMeansInfoToConsole,-1)

	#"""	
	img3rdthirty=thirdRunKmeansSegTuple[0]
	img3rd=np.uint8(img3rdthirty*255)
	img3rd=cv2.cvtColor(img3rd,cv2.COLOR_LAB2BGR)
	thirdmasks=thirdRunKmeansSegTuple[2]
	#cv2.imshow("3rd ",img3rd)
	#for i in range(0,2):
		#cv2.imshow(str(i)+". 3rd",thirdmasks[i]*255)
	
	
	#thirdpassMasks=thirdRunKmeansSegTuple[2]
	#"""
	#res2=cv2.cvtColor(res2,cv2.COLOR_LAB2BGR)
	#writingImg=cv2.cvtColor(writingImg,cv2.COLOR_LAB2BGR)
	#"""
	#cv2.imwrite(str(tc)+"_characterMask2.png",thirdpassMasks[1]*255)
	#cv2.imwrite(str(tc)+"_characterMask1.png",thirdpassMasks[0]*255)
	#cv2.imwrite(os.path.join(dirname,str(tc)+"_characterMask.png"),charSeg*255)
	#cv2.imwrite(os.path.join(dirname,str(tc)+"_original.png"),lim)

	#cv2.imwrite(os.path.join(dirname,str(tc)+"_shapeMask.png"),shaype*255)

	#"""
	#cv2.imshow("mask2",thirdpassMasks[1]*255)
	#cv2.imshow("mask1",thirdpassMasks[0]*255)
	#cv2.imshow("shape",shapeSeg*255);
	#cv2.imshow("char*255",charSeg*255);
	#cv2.imshow("char",charSeg);
	#"""
	sc=(0,0,0)
	shapeColor = getColor(np.copy(shapeSeg), np.copy(charSeg), resizedOrig, True, sc)
	
	charColor = getColor(np.copy(shapeSeg), np.copy(charSeg), resizedOrig, False, shapeColor)
	#print charColor
	
	#-------------------------------------------------------------------------
	#print "end"
	#print time.time() - start
	#cv2.waitKey(0);
	return (shapeSeg*255, shapeColor, charSeg*255, charColor, shapefound)





