
#kmeans function
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

#get color of clusters in shape
def getShapeColors(res2, numKlusters):
	#get the color of each cluster
	color =np.array(([res2[10,10,0],res2[10,10,1],res2[10,10,2]],[0,0,0],[0,0,0],[0,0,0],[0,0,0],[0,0,0]))
	fC=[False,False,False,False,False]

	for i in range(0,res2.shape[0]):

		for j in range(0,res2.shape[1]):

			if(fC[0]==False and (res2[i,j,]!=color[0]).any()):
				color[1]=res2[i,j,]
				fC[0]=True

			if(fC[0]==True and fC[1]==False and (res2[i,j,]!=color[0]).any() and (res2[i,j,]!=color[1]).any()):
				color[2]=res2[i,j,]
				fC[1]=True

			if(fC[0]==True and fC[1]==True and fC[2]==False and (res2[i,j,]!=color[0]).any()
			   and (res2[i,j,]!=color[1]).any() and (res2[i,j,]!=color[2]).any() ):
				color[3]=res2[i,j,]
				fC[2]=True

			if(numKlusters==3 and fC[2]==True): break


			if(fC[0]==True and fC[1]==True and fC[2]==True and fC[3]==False and (res2[i,j,]!=color[0]).any()
			   and (res2[i,j,]!=color[1]).any() and (res2[i,j,]!=color[2]).any()  and (res2[i,j,]!=color[3]).any()):
				color[4]=res2[i,j,]
				fC[3]=True
			if(numKlusters==4 and fC[3]==True): break

			if(fC[0]==True and fC[1]==True and fC[2]==True and fC[3]==True and  fC[4]==False and (res2[i,j,]!=color[0]).any()
			   and (res2[i,j,]!=color[1]).any() and (res2[i,j,]!=color[2]).any()  and (res2[i,j,]!=color[3]).any() and (res2[i,j,]!=color[4]).any()):
				color[5]=res2[i,j,]
				fC[4]=True
			if(numKlusters==5 and fC[4]==True): break

		if(numKlusters==3 and fC[2]==True): break
		if(numKlusters==4 and fC[3]==True): break
		if(numKlusters==5 and fC[4]==True): break

	return color

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

#Find how many pixels of each cluster touch the perimeter

def numPixTouchPerm(clusterMaskss, vert, hori, numKlusters):
	m1=clusterMaskss[0]
	m2=clusterMaskss[1]
	m3=clusterMaskss[2]
	m4=clusterMaskss[3]
	m5=clusterMaskss[4]
	m6=clusterMaskss[5]
	perM1= perM2=perM3=perM4=perM4=perM5=perM6=0.0

	for i in range(0,vert):
		perM1= perM1 + m1[i,0,] +m1[i,hori-1,]
		perM2= perM2 + m2[i,0,] +m2[i,hori-1,]
		perM3= perM3 + m3[i,0,] +m3[i,hori-1,]
		perM4= perM4 + m4[i,0,] +m4[i,hori-1,]
		perM5= perM5 + m5[i,0,] +m5[i,hori-1,]
		perM6= perM6 + m6[i,0,] +m6[i,hori-1,]
	for i in range(0,hori):
		perM1= perM1 + m1[0,i,] +m1[vert-1,i,]
		perM2= perM2 + m2[0,i,] +m2[vert-1,i,]
		perM3= perM3 + m3[0,i,] +m3[vert-1,i,]
		perM4= perM4 + m4[0,i,] +m4[vert-1,i,]
		perM5= perM5 + m5[0,i,] +m5[vert-1,i,]
		perM6= perM6 + m6[0,i,] +m6[vert-1,i,]


	return perM1,perM2,perM3,perM4,perM5,perM6


def doSegmentation(cropImg, optionalArgs):
	
	imrows,imcols = cropImg.shape[:2]
	print "Monal-python-segmentation is processing an image of size: " + str(imcols) + "x" + str(imrows)
	print "note: currently this only returns color-clustered images, not the shape or char masks"
	
	#-------------------------------------------------------------------------
	lim = cropImg
	#cv2.imshow("1.Original Image", lim)
	lab=cv2.cvtColor(lim,cv2.COLOR_BGR2LAB)
	#cv2.imshow("1A.CIE LAB", lab)
	labf=cv2.bilateralFilter(lab,15,50,15)
	#cv2.imshow("2.Bilateral FIlter", labf)
	#15,65,15 for bilat 2/3, orig val i chose

	res2=kmeans(labf,3)

	color=getShapeColors(res2,3)

	"""
	clusterMaskss,cowunts=makeMasks(res2, 3, color)


	vert=res2.shape[0]
	hori=res2.shape[1]
	tot_per=vert*2+hori*2
	tot_area=vert*hori

	maskPixelFraction=np.zeros(6,np.uint8)
	for i in range(0,6):
		maskPixelFraction[i]=100*cowunts[i]/tot_per

	pAm1=maskPixelFraction[0]
	pAm2=maskPixelFraction[1]
	pAm3=maskPixelFraction[2]

	m1=clusterMaskss[0]
	m2=clusterMaskss[1]
	m3=clusterMaskss[2]
	#Find how many pixels of each cluster touch the perimeter
	perM1,perM2,perM3,perM4,perM5,perM6= numPixTouchPerm(clusterMaskss, vert, hori, 3)



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
   
	print "perm test A"
	print bul1
	print bul2
	print bul3
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


	"""
	print "perm test B"
	print bul1
	print bul2
	print bul3
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

	"""
	print "Cluster Size test"
	print bul1
	print bul2
	print bul3
	print pAm2
	print pAm3
	#"""


	clusterMasks=(m1,m2,m3)

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
	buuL=(bul1,bul2,bul3)
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
							print permTouch/ar
							print permTouch
							print ar
							temp[0]=x[j]
							temp[1]=y[j]
							temp[2]=w[j]
							temp[3]=h[j]

							x[j],y[j],w[j],h[j] = cv2.boundingRect(cnts[i])

							cv2.drawContours(keep[j],cnts,i,1, -1 )
							numCont[j]=numCont[j]+1
							contbool[j]=True
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


	# A bunch of test to eliminate and narrow the number of contour masks to one
	if(int(contbool[0])+int(contbool[1])+int(contbool[2])!=1):
		#"""
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
		#"""
		print contbool[1]
		print contbool[2]
		#"""
		if((contareas[1]>contareas[0]*2.28 and contbool[1] and contbool[0] and (contcirc[1]>contcirc[0]*.87 or contareas[1]>contareas[0]*3.4 )) or (contcirc[1]>contcirc[0]*1.8 and contbool[1] and contbool[0] and 2.4*contareas[1]>contareas[0])):
			contbool[0]=False
			numCont[0]=0
		if((contareas[1]>contareas[2]*2.28 and contbool[1] and contbool[2] and (contcirc[1]>contcirc[2]*.87 or contareas[1]>contareas[2]*3.4) ) or (contcirc[1]>contcirc[2]*1.8 and contbool[1] and contbool[2] and 2.4*contareas[1]>contareas[2])):
			contbool[2]=False
			numCont[2]=0

		if(int(contbool[0])+int(contbool[1])+int(contbool[2])==2 and bul1+bul2+bul3==1):

			if(bul1):
				contareas[1]=0
				contareas[2]=0
				contbool[1]=False
				contbool[2]=False
				numCont[1]=0
				numCont[2]=0
			if(bul2):
				contareas[0]=0
				contareas[2]=0
				contbool[0]=False
				contbool[2]=False
				numCont[0]=0
				numCont[2]=0
			if(bul3):
				contareas[0]=0
				contareas[1]=0
				contbool[0]=False
				contbool[1]=False
				numCont[0]=0
				numCont[1]=0
		print numCont[1]
		print numCont[2]

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
		if(int(contbool[0])+int(contbool[1])+int(contbool[2])==2 and bul1+bul2+bul3==2):

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



	keep=(keep[0]*255,keep[1]*255,keep[2]*255)

	#Find the right mask number and use one of the cluster masks if there are 0 acceptable contour masks
	maskNumber=7
	for i in range(0,3):
		if(contbool[i]):
			maskNumber =i
	if (maskNumber==7):
		keep=list(keep)
		for i in range(0,3):
			if(buuL[i]):
				maskNumber =i
				keep[maskNumber]=clusterMasks[i]*255
				contbool[i]=True

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



	print contcirc [maskNumber]

	KEq2=True;
	#convexhull
	if(numCont[maskNumber]>1 or contcirc[maskNumber]<.16 ):
		hulll=np.zeros((lim.shape[0],lim.shape[1],1),np.uint8)
		(kontours,_)=cv2.findContours(keep[maskNumber].copy(),cv2.RETR_EXTERNAL,cv2.CHAIN_APPROX_NONE)
		KEq2=False;
		unicon=np.vstack(kontours)
		convex=cv2.convexHull(unicon,False)
		cv2.drawContours(hulll,[convex],0,1,-1)
	else:
		hulll=keep[maskNumber]

	sideVer=lim.shape[0]
	factorMag=(3 if (300.0/sideVer>=2.5) else 2) if (300.0/sideVer>=1.5) else 1
	hulll2=cv2.resize(hulll,(lim.shape[0]*factorMag,lim.shape[1]*factorMag),0,0)


	kernnal=cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(10,10))
	hulll2=cv2.erode(hulll2,kernnal,iterations=1)



	#labf2=cv2.bilateralFilter(lab,-1,3,70)

	lab2=cv2.resize(lab,(lim.shape[0]*factorMag,lim.shape[1]*factorMag),0,0,cv2.INTER_CUBIC)

	cropf32=np.float32(lab2)/255

	shapeSegMask=hulll2

	if(KEq2):
		shapeSeg=pykmeansppcpplib.ClusterKmeansPPwithMask(cropf32,shapeSegMask,2,14,24)
	else:
		shapeSeg=pykmeansppcpplib.ClusterKmeansPPwithMask(cropf32,shapeSegMask,3,14,24)
	#shapeSeg=pykmeansppcpplib.ClusterKmeansPPwithMask(shapeSeg,shapeSegMask,6,14,24)
	#shapeSeg=pykmeansppcpplib.ClusterKmeansPPwithMask(shapeSeg,shapeSegMask,3,14,24)
	#shapeSeg=pykmeansppcpplib.ClusterKmeansPPwithMask(shapeSeg,shapeSegMask,2,14,24)


	writingImg=np.uint8(shapeSeg*255)
	writingImg=cv2.cvtColor(writingImg,cv2.COLOR_LAB2BGR)
	if(KEq2):
		charColors=getShapeColors(writingImg, 3)
		charMasks,charCowunts=makeMasks(writingImg, 3, charColors)
  
	else:
		charColors=getShapeColors(writingImg, 4)
		charMasks,charCowunts=makeMasks(writingImg, 4, charColors)
   
	#cv2.imshow("cpp kmean",writingImg)
	cm1=np.zeros((writingImg.shape[0],writingImg.shape[1],1),np.uint8)
	cm2=np.zeros((writingImg.shape[0],writingImg.shape[1],1),np.uint8)
	cm3=np.zeros((writingImg.shape[0],writingImg.shape[1],1),np.uint8)
	cm4=np.zeros((writingImg.shape[0],writingImg.shape[1],1),np.uint8)

	charKeep=(cm1,cm2,cm3,cm4)

	charNumCont=np.zeros(4,np.uint8)

	charContbool=np.array([False,False,False,False])
	charContareas=np.array([0.0,0.0,0.0,0.0])
	charContcirc=np.array([0.0,0.0,0.0,0.0])


	j=0
	for clusters in charMasks:
		(cnts,_)=cv2.findContours(clusters.copy(),cv2.RETR_CCOMP,cv2.CHAIN_APPROX_NONE)
		print "Cont "+ str(j+1)
		print len(cnts)
		artot=0
		for i in range (len(cnts)):

			perimC=cv2.arcLength(cnts[i],True)
			ar= cv2.contourArea(cnts[i])
			dem=(np.power(perimC,2))
			numer=4.0*3.14159265*cv2.contourArea(cnts[i])
			artot=artot+ar
			if(dem!=0):
				circularity= numer/dem


				if(.028*tot_area<=ar and ar<=.83*tot_area):

					if(circularity>.13):
						print "Circularity: " + str(circularity)
						print "Area: " +str(ar/tot_area)

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
		
		for i in range(0,3):
			if(charCowunts[i]<smallest):
				smallest=charCowunts[i]
				charMaskNum=i
			#cv2.imshow("charmask",charKeep[charMaskNum]*255)
	else:
		for i in range(0,4):
			if(charCowunts[i]<smallest):
				smallest=charCowunts[i]
				charMaskNum=i
			#cv2.imshow("charmask",charKeep[charMaskNum]*255)

	#shaype=cv2.add(keep[maskNumber],charKeep[charMaskNum])
	#keep[maskNumber]=keep[maskNumber]+charKeep[charMaskNum]
	#keep=tuple(keep)
	#cv2.imshow("c++ cluster",writingImg)
	#cv2.imshow("winner",charMasks[charMaskNum]*255)
	
	
	shaype=cv2.resize( keep[maskNumber],(lim.shape[0]*factorMag,lim.shape[1]*factorMag),0,0,cv2.INTER_CUBIC)
	shaype=cv2.bitwise_or(shaype/255,charKeep[charMaskNum])  
	noisedel=charMasks[charMaskNum]
	charSeg=charKeep[charMaskNum]
	cv2.bitwise_not(charMasks[charMaskNum],noisedel,charKeep[charMaskNum])	
	cv2.bitwise_not(charMasks[charMaskNum],charSeg,noisedel)
	if(not KEq2):
		kernnal=cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5))
		shaype=cv2.dilate(shaype,kernnal,iterations=1)
	#cv2.imwrite(str(tc)+"_characterMask.png",charSeg*255)
	#cv2.imwrite(str(tc)+"_cpluskmeans.png",writingImg)
	#cv2.imwrite(str(tc)+"_shapeMask.png",shaype*255)
	shapeSeg=shaype
	#-------------------------------------------------------------------------
	"""	
	cropf32 = np.float32(cropImg)
	cropf32copy = cropf32.copy() #the cluster function messes up this original image, so make a copy
	
	shapeSegMask = np.ones((imrows,imcols), np.uint8)
	charSegMask = np.zeros((imrows,imcols), np.uint8)
	
	for ii in range(imrows):
		if ii > int(float(imrows)*0.25) and ii < int(float(imrows)*0.75):
			for jj in range(imcols):
				if jj > int(float(imcols)*0.25) and jj < int(float(imcols)*0.75):
					charSegMask[ii][jj] = 255
	
	shapeSeg = pykmeansppcpplib.ClusterKmeansPPwithMask(cropf32, shapeSegMask, 3, 8, 20)
	charSeg = pykmeansppcpplib.ClusterKmeansPPwithMask(cropf32copy, charSegMask, 3, 8, 20)
	"""
	shapeColor = color[maskNumber]
	charColor = charColors[charMaskNum]
	#-------------------------------------------------------------------------
	
	return (shapeSeg, shapeColor, charSeg, charColor)


