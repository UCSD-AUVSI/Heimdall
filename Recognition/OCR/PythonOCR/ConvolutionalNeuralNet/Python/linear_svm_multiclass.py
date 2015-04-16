"""
Linear Support Vector Machines (SVM) for multi-class classification,
trainable with stochastic gradient descent (SGD) or other optimization.

Uses Theano, a python machine learning library in which symbolic math
for optimization in machine learning can be compiled to either CPU or fast GPU code.

References:
	Theano tutorials at:  http://deeplearning.net/software/theano/tutorial/
	Mathematics of implementing SVM with SGD:  http://www.vlfeat.org/api/svm-fundamentals.html

"""
__docformat__ = 'restructedtext en'

import os
import time
import numpy
import theano
import theano.tensor as T
import cPickle


class LinearSVMMulticlass(object):
	""" Linear SVMs multi-class classifier class
	
	A set of hyperplanes described by a weight matrix W
	and bias vector b, as an extended form of the binary SVM which uses a weight vector and bias scalar.
	Classification is done by projecting data points onto the set of hyperplanes defined by the columns of W,
	the distances to which are used to determine a class membership.
	"""
	
	
	def GetZeroWeights(self):
		Wzeros = numpy.zeros((self.nfeatures,self.nclasses), dtype=theano.config.floatX)
		bzero = numpy.zeros((self.nclasses,), dtype=theano.config.floatX)
		return Wzeros, bzero
	
	
	def __init__(self, input, nfeatures, nclasses, C):
		""" Initialize the parameters of the SVM
		
		input: theano.tensor.TensorType
			symbolic variable that describes the input of the architecture (one minibatch)
		
		nfeatures: number of input units, the dimension of the space in which the datapoints lie
		
		nclasses: number of output classes
		
		C: error penalty
		"""
		self.nfeatures = nfeatures
		self.nclasses = nclasses
		Wzeros, bzero = self.GetZeroWeights()
		
		#create a matrix with nfeatures rows and nclasses columns
		self.W = theano.shared(value=Wzeros, name='W', borrow=True)
		
		# initialize bias: a vector with nclasses rows
		self.b = theano.shared(value=bzero, name='b', borrow=True)
		
		# initialize the error penalty C
		self.C = C
		
		# hyperplane projection used in classification;
		# matrix dot product shape: (nsamples,nfeatures)*(nfeatures,nclasses) == shape (nsamples,nclasses)
		# bias vector is broadcast (copied) across all rows (samples)
		self.hplaneproject = T.dot(input, self.W) + self.b
		
		# symbolic description of how to compute prediction as class
		# get max across the columns, returning one prediction per sample row
		self.y_pred = T.argmax(self.hplaneproject, axis=1)
		
		self.params = [self.W,self.b]
	
	
	def clearWeights(self):
		Wzeros, bzero = self.GetZeroWeights()
		self.W.set_value(Wzeros, borrow=True)
		self.b.set_value(bzero, borrow=True)
	
	def saveParams(self, filename):
		fout = file(filename,'wb')
		cPickle.dump(self.W.get_value(borrow=True), fout, protocol=cPickle.HIGHEST_PROTOCOL)
		cPickle.dump(self.b.get_value(borrow=True), fout, protocol=cPickle.HIGHEST_PROTOCOL)
		cPickle.dump(self.C, fout, protocol=cPickle.HIGHEST_PROTOCOL)
		fout.close()
	def loadParams(self, filename):
		fin = file(filename,'rb')
		self.W.set_value(cPickle.load(fin), borrow=True)
		self.b.set_value(cPickle.load(fin), borrow=True)
		self.C = cPickle.load(fin)
		fin.close()
	
	
	def CostToMinimize(self, y):
		"""This is the symbolic expression that we want to minimize.
		
		y: theano.tensor.TensorType
			corresponds to a vector that gives for each example the correct label
		"""
		
		# minimize margins and the mean of all hinge losses in the minibatch
		
		# W has shape (nfeatures,nclasses)
		# the * symbol does element-by-element multiplication;
		# then sum along the rows to produce a vector with nclasses elements
		marginterm = 0.5 * T.sum(self.W*self.W,axis=0)
		
		nclasses = self.W.shape[1]
		
		# produce a matrix of shape (nsamples,nclasses)
		# where each row is the truth class index repeated nclasses times e.g. [[1 1 1];[0 0 0];[2 2 2];[1 1 1]...] for 3 classes
		yprocessA = T.reshape(y,[y.shape[0],1],2)*T.alloc(1,y.shape[0],nclasses)
		
		# produce a matrix of shape (nsamples,nclasses)
		# where each row is the range from 0 to nclasses-1, e.g. [[0 1 2];[0 1 2];[0 1 2];[0 1 2]...] for 3 classes
		yprocessB = T.alloc(1,y.shape[0],nclasses)*T.arange(nclasses)
		
		# do the equals operation ("==") element-by-element for the above two matrices
		# for the previously used 3-class example, produces [[0 1 0];[1 0 0];[0 0 1];[0 1 0]...]
		# resultant shape is again (nsamples,nclasses)
		yprocessTot = T.eq(yprocessA, yprocessB) * 2 - 1  #convert range from (0,1) to (-1,1)
		
		# L1-SVM hinge losses
		# maximum() is an element-by-element operation
		# shape of yprocessTot and hplaneproject are the same: (nsamples,nclasses) since we do elementwise multiply
		L1hingelosses = T.maximum(0, 1 - yprocessTot*self.hplaneproject)
		
		# slack term: C * mean of slack variables for all samples
		# will be a vector with nclasses elements
		slackTerms = self.C * T.mean(L1hingelosses, axis=0)
		
		# return the sum of the results of all SVMs; we want to improve them all at once
		return T.sum(marginterm + slackTerms)
	
	
	def calculateErrors(self, y):
		"""Return a float representing the number of errors in the minibatch
		over the total number of examples of the minibatch
		
		y: theano.tensor.TensorType
			corresponds to a vector that gives for each example the correct label
		"""
		
		#print('y.ndim == '+str(y.ndim)+', y_pred.ndim == '+str(self.y_pred.ndim))
		#print('y.shape[0] == '+str(y.shape[0])+', y_pred.shape[0] == '+str(self.y_pred.shape[0]))
		
		# check if y has same dimension of y_pred
		if y.ndim != self.y_pred.ndim:
			raise TypeError('y should have the same shape as self.y_pred', ('y', y.type, 'y_pred', self.y_pred.type))
			quit()
		
		# check if y is of the correct datatype
		if y.dtype.startswith('int'):
			# the T.neq ("not-equal") operator returns a vector of 0s and 1s, where 1
			# represents a mistake in prediction
			return T.mean(T.neq(self.y_pred, y))
		else:
			raise NotImplementedError()
			quit()






