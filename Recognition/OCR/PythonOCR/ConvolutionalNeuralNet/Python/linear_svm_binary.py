"""
Linear Support Vector Machine (SVM) for binary classification,
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


class LinearSVMBinary(object):
	""" Linear SVM binary classifier class
	
	The Linear SVM is fully described by a weight matrix W
	and bias vector b. Classification is done by projecting data
	points onto a hyperplane, the sign of which determines
	whether it is an instance of a class or not.
	"""
	
	
	def GetZeroWeights(self):
		Wzeros = numpy.zeros((self.nfeatures,), dtype=theano.config.floatX)
		bzero = Wzeros.dtype.type(0)
		return Wzeros, bzero
	
	
	def __init__(self, input, nfeatures, C):
		""" Initialize the parameters of the SVM
		
		input: theano.tensor.TensorType
			symbolic variable that describes the input of the architecture (one minibatch)
		
		nfeatures: number of input units, the dimension of the space in which the datapoints lie
		
		C: error penalty
		"""
		self.nfeatures = nfeatures
		Wzeros, bzero = self.GetZeroWeights()
		
		#create a column vector with nfeatures rows
		self.W = theano.shared(value=Wzeros, name='W', borrow=True)
		
		# initialize bias: a scalar of the same data type as W
		self.b = theano.shared(bzero, name='b')#, borrow=True)
		
		# initialize the error penalty C
		self.C = C
		
		# hyperplane projection used in classification
		# T.dot(input,self.W) creates a vector of shape (rows,) == (# in minibatch,)
		# adding +self.b broadcasts the bias, adding it to each row, so the result is still of shape (rows,)
		self.hplaneproject = T.dot(input, self.W) + self.b
		
		# symbolic description of how to compute prediction as -1 or 1
		# the function sign() is not in Theano,
		# so I use (x>0)*2-1 using T.ge() which returns 1 when true and 0 when false
		self.y_pred = T.ge(self.hplaneproject, 0)*2 - 1
		
		
		# equivalent in scikit-learn:
		#self.scikitlearn_svc = svm.SVC(kernel='linear', C=C)
	
	
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
		
		# minimize margin and the mean of all hinge losses in the minibatch
		
		# vector-vector dot product produces a scalar
		marginterm = 0.5 * T.dot(self.W, self.W)
		
		# L1-SVM hinge loss
		# maximum() is an element-by-element operation
		# result is a vector with nsamples elements
		L1hingeloss = T.maximum(0, 1 - y*self.hplaneproject)
		
		# use the mean across n samples
		return marginterm + self.C * T.mean(L1hingeloss)
	
	
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






