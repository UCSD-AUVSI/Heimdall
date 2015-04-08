"""This tutorial introduces the LeNet5 neural network architecture
using Theano.  LeNet5 is a convolutional neural network, good for
classifying images. This tutorial shows how to build the architecture,
and comes with all the hyper-parameters you need to reproduce the
paper's MNIST results.


This implementation simplifies the model in the following ways:

 - LeNetConvPool doesn't implement location-specific gain and bias parameters
 - LeNetConvPool doesn't implement pooling by average, it implements pooling
   by max.
 - Digit classification is implemented with a logistic regression rather than
   an RBF network
 - LeNet5 was not fully-connected convolutions at second layer

References:
 - Y. LeCun, L. Bottou, Y. Bengio and P. Haffner:
   Gradient-Based Learning Applied to Document
   Recognition, Proceedings of the IEEE, 86(11):2278-2324, November 1998.
   http://yann.lecun.com/exdb/publis/pdf/lecun-98.pdf

Copyright (c) 2008-2015, Theano Development Team All rights reserved.

Contains code from NumPy, Copyright (c) 2005-2011, NumPy Developers. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

        Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
        Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
        Neither the name of Theano nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""
import os
import sys
import time
import cPickle

import numpy

import theano
import theano.tensor as T
from theano.tensor.signal import downsample
from theano.tensor.nnet import conv

from logistic_sgd import LogisticRegression
from DatasetsLoaders import send_host_datasets_to_device
from DatasetsLoaders import load_chars74k
from DatasetsLoaders import load_MNIST
from mlp import HiddenLayer, DropoutHiddenLayer, _dropout_from_layer, _add_noise_to_input, ReLu


class LeNetConvPoolLayer(object):
	"""Pool Layer of a convolutional network """

	def __init__(self, rng, input, filter_shape, image_shape, activation, poolsize=(2, 2)):
		"""
		Allocate a LeNetConvPoolLayer with shared variable internal parameters.
		
		:type rng: numpy.random.RandomState
		:param rng: a random number generator used to initialize weights
		
		:type input: theano.tensor.dtensor4
		:param input: symbolic image tensor, of shape image_shape
		
		:type filter_shape: tuple or list of length 4
		:param filter_shape: (number of filters, num input feature maps, filter height, filter width)
		
		:type image_shape: tuple or list of length 4
		:param image_shape: (batch size, num input feature maps, image height, image width)
		
		:type poolsize: tuple or list of length 2
		:param poolsize: the downsampling (pooling) factor (#rows, #cols)
		"""
		
		assert image_shape[1] == filter_shape[1]
		self.input = input
		
		outpimsize = (image_shape[3] - filter_shape[3] + 1)
		#print("constructing LeNetConvPoolLayer... image_shape == "+str(image_shape)+", filter_shape == "+str(filter_shape)+", num outputs == "+str(filter_shape[0]*outpimsize*outpimsize/(poolsize[0]*poolsize[1])))
		
		# there are "num input feature maps * filter height * filter width"
		# inputs to each hidden unit
		fan_in = numpy.prod(filter_shape[1:])
		# each unit in the lower layer receives a gradient from:
		# "num output feature maps * filter height * filter width" / pooling size
		fan_out = (filter_shape[0] * numpy.prod(filter_shape[2:]) / numpy.prod(poolsize))
		
		# initialize weights with random weights
		W_bound = numpy.sqrt(6. / (fan_in + fan_out))
		self.W = theano.shared(
			numpy.asarray(
				rng.uniform(low=-W_bound, high=W_bound, size=filter_shape),
				dtype=theano.config.floatX
			),
			borrow=True
		)
		
		# the bias is a 1D tensor -- one bias per output feature map
		b_values = numpy.zeros((filter_shape[0],), dtype=theano.config.floatX)
		self.b = theano.shared(value=b_values, borrow=True)
		
		# convolve input feature maps with filters
		conv_out = conv.conv2d(
			input=input,
			filters=self.W,
			filter_shape=filter_shape,
			image_shape=image_shape
		)
		
		# downsample each feature map individually, using maxpooling
		pooled_out = downsample.max_pool_2d(
			input=conv_out,
			ds=poolsize,
			ignore_border=True
		)
		
		# add the bias term. Since the bias is a vector (1D array), we first
		# reshape it to a tensor of shape (1, n_filters, 1, 1). Each bias will
		# thus be broadcasted across mini-batches and feature map
		# width & height
		self.output = activation(pooled_out + self.b.dimshuffle('x', 0, 'x', 'x'))
		
		# store parameters of this layer
		self.params = [self.W, self.b]
	
	def saveFilters(self, filenamebase):
		import cv2
		numfilters = len(self.W.get_value(borrow=True))
		for i in range(numfilters):
			improcessed = numpy.copy(self.W.get_value(borrow=True)[i][0])
			filtermin = numpy.amin(improcessed)
			filtermax = numpy.amax(improcessed)
			#print("filter (min,max) == ("+str(filtermin)+", "+str(filtermax)+")")
			improcessed = (improcessed-filtermin)*(255.0/(filtermax-filtermin))
			#print("    post-processed filter (min,max) == ("+str(numpy.amin(improcessed))+", "+str(numpy.amax(improcessed))+")")
			cv2.imwrite(filenamebase+str(i)+".png", cv2.resize(improcessed, (60,60), interpolation=cv2.INTER_NEAREST))
	
	def saveParams(self, filename):
		fout = file(filename,'wb')
		cPickle.dump(self.W.get_value(borrow=True), fout, protocol=cPickle.HIGHEST_PROTOCOL)
		cPickle.dump(self.b.get_value(borrow=True), fout, protocol=cPickle.HIGHEST_PROTOCOL)
		fout.close()
	def loadParams(self, filename):
		fin = file(filename,'rb')
		self.W.set_value(cPickle.load(fin), borrow=True)
		self.b.set_value(cPickle.load(fin), borrow=True)
		fin.close()


class DropoutLeNetConvPoolLayer(LeNetConvPoolLayer):
	def __init__(self, rng, input, filter_shape, image_shape, dropout_rate, dropout_noise_rate, activation, poolsize=(2, 2)):
		super(DropoutLeNetConvPoolLayer, self).__init__(
				rng=rng, input=input, filter_shape=filter_shape, image_shape=image_shape, activation=activation, poolsize=poolsize)
		self.input = _add_noise_to_input(rng, self.input, p=dropout_noise_rate)
		self.output = _dropout_from_layer(rng, self.output, p=dropout_rate)


def gradient_updates_momentum(cost, params, learning_rate, momentum):
	'''
	Compute updates for gradient descent with momentum
	
	:parameters:
		- cost : theano.tensor.var.TensorVariable
			Theano cost function to minimize
		- params : list of theano.tensor.var.TensorVariable
			Parameters to compute gradient against
		- learning_rate : float
			Gradient descent learning rate
		- momentum : float
			Momentum parameter, should be at least 0 (standard gradient descent) and less than 1
   
	:returns:
		updates : list
			List of updates, one for each parameter
	'''
	# Make sure momentum is a sane value
	assert momentum < 1 and momentum >= 0
	# List of update steps for each parameter
	updates = []
	# Just gradient descent on cost
	for param in params:
		# For each parameter, we'll create a param_update shared variable.
		# This variable will keep track of the parameter's update step across iterations.
		# We initialize it to 0
		param_update = theano.shared(param.get_value()*0., broadcastable=param.broadcastable)
		# Each parameter is updated by taking a step in the direction of the gradient.
		# However, we also "mix in" the previous step according to the given momentum value.
		# Note that when updating param_update, we are using its old value and also the new gradient step.
		updates.append((param, param - learning_rate*param_update))
		# Note that we don't need to derive backpropagation to compute updates - just use T.grad!
		updates.append((param_update, momentum*param_update + (1. - momentum)*T.grad(cost, param)))
	return updates



#=========================================================================================================================
# note: dropout rates are applied to the output of the respective layer
#       noise rates are applied to the input and scale between 0 and 255, so should only be used as input to the very first layer
#
class myCNNParams(object):
	def __init__(self):		
		self.widthOfImages = 40
		self.activation = ReLu
		# three convolutional layers -- dimensionality 16200, 6400, 3240
		self.filtersizes = [5, 3, 3]
		self.poolsizes = [2, 2, 1]
		self.nkerns=[50, 100, 90]
		self.dropoutrates_conv = [0.2, 0.3, 0.4]
		self.noiserates_conv = [0.2, 0, 0]
		# two fully connected layers + classification layer -- dimensionality 1000, 330, 36
		self.dropoutrates_fullyconn = [0.4, 0.3]
		self.hiddenlayers = [1100, 500]
		self.numOutClasses = 36


class OCR_CNN(object):
	def __init__(self, batch_size, useDropout, params):
		
		self.rng = numpy.random.RandomState(23455)
		self.x = T.matrix('x')   # the data are presented as rasterized images
		self.batch_size = batch_size
		self.params = params
		
		######################
		# BUILD ACTUAL MODEL #
		######################
		print '... building OCR_CNN model'
		print("nkerns == "+str(params.nkerns))
		print("filter sizes:  "+str(params.filtersizes))
		print("pool sizes:  "+str(params.poolsizes))
		
		assert len(params.filtersizes) == len(params.poolsizes) and len(params.poolsizes) == len(params.nkerns)
		if useDropout:
			assert len(params.dropoutrates_conv) == len(params.filtersizes) and len(params.noiserates_conv) == len(params.filtersizes)
			assert len(params.dropoutrates_fullyconn) == len(params.hiddenlayers)
			print(" will construct layers with dropouts and noise!!!")
		
		# Reshape matrix of rasterized images of shape (batch_size, widthOfImages * widthOfImages)
		# to a 4D tensor, compatible with our LeNetConvPoolLayer
		# (widthOfImages, widthOfImages) is the size of images.
		nextinput = self.x.reshape((batch_size, 1, params.widthOfImages, params.widthOfImages))
		nextinputwidth = params.widthOfImages
		lastchannelsin = 1
		self.convLayers = []
		self.allLayers = []
		
		for clidx in range(len(params.filtersizes)):
			# construct convolutional layer
			# filtering reduces the image size to (widthOfImages-filter0Size+1 , widthOfImages-filter0Size+1) = (24, 24) for MNIST
			# maxpooling reduces this further to (24/2, 24/2) = (12, 12) for MNIST
			# 4D output tensor is thus of shape (batch_size, nkerns[0], 12, 12) for MNIST
			if useDropout == False:
				newlayer = LeNetConvPoolLayer(self.rng, input=nextinput,
								image_shape=(batch_size, lastchannelsin, nextinputwidth, nextinputwidth),
								filter_shape=(params.nkerns[clidx], lastchannelsin, params.filtersizes[clidx], params.filtersizes[clidx]),
								poolsize=(params.poolsizes[clidx], params.poolsizes[clidx]),
								activation=params.activation)
			else:
				newlayer = DropoutLeNetConvPoolLayer(self.rng, input=nextinput,
								image_shape=(batch_size, lastchannelsin, nextinputwidth, nextinputwidth),
								filter_shape=(params.nkerns[clidx], lastchannelsin, params.filtersizes[clidx], params.filtersizes[clidx]),
								poolsize=(params.poolsizes[clidx], params.poolsizes[clidx]),
								dropout_rate=params.dropoutrates_conv[clidx],
								dropout_noise_rate=params.noiserates_conv[clidx],
								activation=params.activation)
			self.convLayers.append(newlayer)
			self.allLayers.append(newlayer)
			nextinputwidth = (nextinputwidth - params.filtersizes[clidx] + 1) / params.poolsizes[clidx]
			lastchannelsin = params.nkerns[clidx]
			nextinput = self.convLayers[-1].output #last output
		
		# the HiddenLayer being fully-connected, it operates on 2D matrices of
		# shape (batch_size, num_pixels) (i.e matrix of rasterized images).
		# This will generate a matrix of shape (batch_size, nkerns[1] * 4 * 4),
		# or (batch_size, 50 * 4 * 4) = (batch_size, 800) with the default values.
		nextinput = nextinput.flatten(2)
		nextnumin = lastchannelsin * nextinputwidth * nextinputwidth
		
		#print("number of inputs to first fully-connected layer will be == "+str(nextnumin))
		
		for hlidx in range(len(params.hiddenlayers)):
			
			# construct fully-connected layer(s)
			
			if useDropout == False:
				newlayer = HiddenLayer(self.rng, input=nextinput,
							n_in = nextnumin,
							n_out = params.hiddenlayers[hlidx],
							activation = params.activation)
			else:
				newlayer = DropoutHiddenLayer(rng=self.rng, input=nextinput,
							n_in = nextnumin,
							n_out = params.hiddenlayers[hlidx],
							activation = params.activation,
							dropout_rate = params.dropoutrates_fullyconn[hlidx],
							dropout_noise_rate=0)
			
			self.allLayers.append(newlayer)
			nextinput = newlayer.output
			nextnumin = params.hiddenlayers[hlidx]
		
		# classify the values of the last fully-connected layer
		
		newlayer = LogisticRegression(input=nextinput, n_in=nextnumin, n_out=params.numOutClasses)
		self.allLayers.append(newlayer)
		
		nextinputwidth = params.widthOfImages
		print("------------------------ finished constructing network")
		for lidx in range(len(self.convLayers)):
			nextinputwidth = (nextinputwidth - params.filtersizes[lidx] + 1) / params.poolsizes[lidx]
			print("convolutional layer "+str(lidx)+" outputs a "+str(params.nkerns[lidx])+"-channel "+str(nextinputwidth)+"x"+str(nextinputwidth)+" image, i.e. "+str(params.nkerns[lidx]*nextinputwidth*nextinputwidth)+" points")
		for lidx in range(len(self.allLayers)):
			if lidx >= len(self.convLayers):
				numpinpts = len(self.allLayers[lidx].params[0].get_value(borrow=True))
				numoutpts= len(self.allLayers[lidx].params[1].get_value(borrow=True))
				if lidx == (len(self.allLayers)-1):
					print("classification layer"+str(lidx)+" has "+str(numpinpts)+" inputs and "+str(numoutpts)+" outputs")
				else:
					print("fully-connected layer"+str(lidx)+" has "+str(numpinpts)+" inputs and "+str(numoutpts)+" outputs")
		print("------------------------")
	
	def Train(self, datasets, datasetsOnDevice, learning_rate, weightmomentum, n_epochs, pickleNetworkName, filterFilesSavedBaseName="", deviceTrainSetSize=64800, deviceValidSetSize=4800):
		
		if datasetsOnDevice:
			train_set_x, train_set_y = datasets[0]
			valid_set_x, valid_set_y = datasets[1]
			#test_set_x, test_set_y = datasets[2]
			
			# compute number of minibatches for training, validation and testing
			n_train_batches = train_set_x.get_value(borrow=True).shape[0]
			n_valid_batches = valid_set_x.get_value(borrow=True).shape[0]
			#n_test_batches = test_set_x.get_value(borrow=True).shape[0]
			nTrainingImagesTotal = n_train_batches
			nValidationImagesTotal = n_valid_batches
			print("num images in training set: "+str(nTrainingImagesTotal))
			print("num images in validation set: "+str(nValidationImagesTotal))
			n_train_batches /= self.batch_size
			n_valid_batches /= self.batch_size
			#n_test_batches /= self.batch_size
		else:
			print("num images in training set: "+str(deviceTrainSetSize))
			print("num images in validation set: "+str(deviceValidSetSize))
			nTrainingImagesTotal = deviceTrainSetSize
			nValidationImagesTotal = deviceValidSetSize
			n_train_batches = (deviceTrainSetSize / self.batch_size)
			n_valid_batches = (deviceValidSetSize / self.batch_size)
			print("todo: process dataset on host and pass again-and-again to device")
			#quit()
		
		
		print("batch size == "+str(self.batch_size))
		print("num valid images actually used in training set: "+str(n_train_batches*self.batch_size))
		print("num valid images actually used in validation set: "+str(n_valid_batches*self.batch_size))
		print("num images MISSING from training set due to minibatch rounding: "+str(nTrainingImagesTotal - n_train_batches*self.batch_size))
		print("num images MISSING from validation set due to minibatch rounding: "+str(nValidationImagesTotal - n_valid_batches*self.batch_size))
		
		y = T.ivector('y')  # training labels are presented as 1D vector of [int] labels
		
		# allocate symbolic variables for the data
		index = T.lscalar()  # index to a [mini]batch
		
		if datasetsOnDevice:
			# create a function to compute the mistakes that are made by the model
			'''test_model = theano.function(
				[index],
				self.allLayers[-1].errors(y),
				givens={
					self.x: test_set_x[index * self.batch_size: (index + 1) * self.batch_size],
					y: test_set_y[index * self.batch_size: (index + 1) * self.batch_size]
				}
			)'''
			validate_model = theano.function(
				[index],
				self.allLayers[-1].errors(y),
				givens={
					self.x: valid_set_x[index * self.batch_size: (index + 1) * self.batch_size],
					y: valid_set_y[index * self.batch_size: (index + 1) * self.batch_size]
				}
			)
		
		# create a list of all model parameters to be fit by gradient descent
		params = self.allLayers[0].params
		for lidx in range(len(self.allLayers)):
			if lidx > 0:
				params = (params + self.allLayers[lidx].params)
		
		# the cost we minimize during training is the NLL of the model
		cost = self.allLayers[-1].negative_log_likelihood(y)
		
		if False:
			# create a list of gradients for all model parameters
			grads = T.grad(cost, params)
		
			# train_model is a function that updates the model parameters by
			# SGD Since this model has many parameters, it would be tedious to
			# manually create an update rule for each model parameter. We thus
			# create the updates list by automatically looping over all
			# (params[i], grads[i]) pairs.
			updates = [
				(param_i, param_i - learning_rate * grad_i)
				for param_i, grad_i in zip(params, grads)
			]
		else:
			updates = gradient_updates_momentum(cost, params, learning_rate, momentum=weightmomentum)
		
		
		if datasetsOnDevice:
			train_model = theano.function(
				[index],
				cost,
				updates=updates,
				givens={
					self.x: train_set_x[index * self.batch_size: (index + 1) * self.batch_size],
					y: train_set_y[index * self.batch_size: (index + 1) * self.batch_size]
				}
			)
	
		###############
		# TRAIN MODEL #
		###############
		print '... training'
		# early-stopping parameters
		patience = 50000  # look as this many examples regardless
		patience_increase = 2  # wait this much longer when a new best is
							   # found
		improvement_threshold = 0.995  # a relative improvement of this much is
									   # considered significant
		validation_frequency = min(n_train_batches, patience / 2)
									  # go through this many
									  # minibatches before checking the network
									  # on the validation set; in this case we
									  # check every epoch
		
		best_validation_loss = numpy.inf
		best_iter = 0
		test_score = 0.
		start_time = time.clock()
		
		epoch = 0
		done_looping = False
		
		while (epoch < n_epochs) and (not done_looping):
			
			#------------------------------------------------------------------------
			if datasetsOnDevice == False:
				# we need to produce a dataset to train this iteration
				
				train_set_x, train_set_y, valid_set_x, valid_set_y, datasets, numTrainPts, numValidationPts = send_host_datasets_to_device(datasets, deviceTrainSetSize, deviceValidSetSize)
				if numTrainPts < deviceTrainSetSize or numValidationPts < deviceValidSetSize:
					print("warning: there were fewer training points in the dataset than could be requested... num points available: "+str(numTrainPts))
					n_train_batches = numTrainPts/self.batch_size
					n_valid_batches = numValidationPts/self.batch_size
				
				train_model = theano.function(
					[index],
					cost,
					updates=updates,
					givens={
						self.x: train_set_x[index * self.batch_size: (index + 1) * self.batch_size],
						y: train_set_y[index * self.batch_size: (index + 1) * self.batch_size]
					}
				)
				validate_model = theano.function(
					[index],
					self.allLayers[-1].errors(y),
					givens={
						self.x: valid_set_x[index * self.batch_size: (index + 1) * self.batch_size],
						y: valid_set_y[index * self.batch_size: (index + 1) * self.batch_size]
					}
				)
			#------------------------------------------------------------------------
			
			epoch = epoch + 1
			for minibatch_index in xrange(n_train_batches):
				
				iter = (epoch - 1) * n_train_batches + minibatch_index
			
				if iter % 100 == 0:
					print 'training @ iter = ', iter
				cost_ij = train_model(minibatch_index)
			
				if (iter + 1) % validation_frequency == 0:
				
					# compute zero-one loss on validation set
					validation_losses = [validate_model(i) for i in xrange(n_valid_batches)]
					this_validation_loss = numpy.mean(validation_losses)
					print('epoch %i, minibatch %i/%i, validation error %f %%' %
						  (epoch, minibatch_index + 1, n_train_batches,
						   this_validation_loss * 100.))
					
					# if we got the best validation score until now
					if this_validation_loss < best_validation_loss:
						
						#improve patience if loss improvement is good enough
						if this_validation_loss < best_validation_loss *  \
						   improvement_threshold:
							patience = max(patience, iter * patience_increase)
						
						# save best validation scores and iteration number
						best_validation_loss = this_validation_loss
						best_iter = iter
						
						pfname = pickleNetworkName+"_"+str(iter)+"_score_"+str(this_validation_loss*100.)+".pkl"
						for nnlayeridx in range(len(self.allLayers)):
							self.allLayers[nnlayeridx].saveParams(pfname+".l"+str(nnlayeridx))
						
						if len(filterFilesSavedBaseName) > 1:
							for layeridx in range(len(self.convLayers)):
								self.convLayers[layeridx].saveFilters(filterFilesSavedBaseName+"_"+str(layeridx)+"_")
						
						print("====== saved new weights and filters")
						
						# test it on the test set
						'''test_losses = [test_model(i) for i in xrange(n_test_batches)]
						test_score = numpy.mean(test_losses)
						print(('   epoch %i, minibatch %i/%i, test error of '
							   'best model %f %%') %
							  (epoch, minibatch_index + 1, n_train_batches,
							   test_score * 100.))'''

				
				if patience <= iter:
					done_looping = True
					break
	
		end_time = time.clock()
		print('Optimization complete.')
		print('Best validation score of %f %% obtained at iteration %i, '
			  'with test performance %f %%' %
			  (best_validation_loss * 100., best_iter + 1, 0.))# test_score * 100.))
		print >> sys.stderr, ('The code for file ' +
							  os.path.split(__file__)[1] +
							  ' ran for %.2fm' % ((end_time - start_time) / 60.))




def train_lenet5_with_batches(useMNIST=False, learning_rate=0.04, weightmomentum=0.75, n_epochs=2000, useDropout=True, batch_size=400, pretrainedWeightsFile=""):
	""" Demonstrates lenet on MNIST dataset
	
	:type learning_rate: float
	:param learning_rate: learning rate used (factor for the stochastic
						  gradient)
	
	:type n_epochs: int
	:param n_epochs: maximal number of epochs to run the optimizer
	
	:type dataset: string
	:param dataset: path to the dataset used for training /testing (MNIST here)
	
	:type nkerns: list of ints
	:param nkerns: number of kernels on each layer
	"""
	
	if useMNIST:
		learning_rate = 0.20
		n_epochs = 2000
		params = myCNNParams()
		params.widthOfImages = 28
		params.filtersizes = [5, 5]
		params.nkerns=[20, 50]
		params.poolsizes = [2, 2]
		numOutClasses = 10
		datasets = load_MNIST('mnist.pkl.gz')
		datasetsOnDevice = True
		pickleNetworkName = "cnn28x28MNISTtheano_paramsWb"
		filterFilesSavedBaseName = ""
	else:
		n_epochs = 15000
		print("learning rate == "+str(learning_rate)+", momentum == "+str(weightmomentum))
		params = myCNNParams()
		datasets = load_chars74k(params.widthOfImages)
		datasetsOnDevice = False
		pickleNetworkName = "/media/ucsdauvsi/442ABBE92ABBD660/OCR_Neural_Network_Backups/weights_saved/cnn40x40theano_paramsWb"
		filterFilesSavedBaseName = "/media/ucsdauvsi/442ABBE92ABBD660/OCR_Neural_Network_Backups/filters_saved/filt"
	
	myCNN = OCR_CNN(batch_size=batch_size, useDropout=useDropout, params=params)
	
	if len(pretrainedWeightsFile) > 1:
		print("loading pretrained weights!")
		# load params layer-by-layer
		for nnlayeridx in range(len(myCNN.allLayers)):
			myCNN.allLayers[nnlayeridx].loadParams(pretrainedWeightsFile+".l"+str(nnlayeridx))
	
	myCNN.Train(datasets, datasetsOnDevice=datasetsOnDevice, learning_rate=learning_rate, weightmomentum=weightmomentum, n_epochs=n_epochs, pickleNetworkName=pickleNetworkName, filterFilesSavedBaseName=filterFilesSavedBaseName)
	


def test_saved_lenet5_on_full_dataset(wasGivenPrebuiltCNN, trainedWeightsFile="", builtCNN="", batchSize=1):
	
	datasets = load_chars74k(myCNNParams().widthOfImages)
	
	if wasGivenPrebuiltCNN == False:
		# construct CNN
		builtCNN = OCR_CNN(batch_size=batchSize, useDropout=False, params=myCNNParams())
		
		# load params layer-by-layer
		for nnlayeridx in range(len(builtCNN.allLayers)):
			builtCNN.allLayers[nnlayeridx].loadParams(trainedWeightsFile+".l"+str(nnlayeridx))
	
	#======================================================================================================================
	train_set_x, train_set_y = datasets[0]
	valid_set_x, valid_set_y = datasets[1]
	test_set_x, test_set_y = datasets[2]
	
	# compute number of minibatches for training, validation and testing
	n_train_batches = train_set_x.get_value(borrow=True).shape[0]
	n_valid_batches = valid_set_x.get_value(borrow=True).shape[0]
	n_test_batches = test_set_x.get_value(borrow=True).shape[0]
	nTrainingImagesTotal = n_train_batches
	nValidationImagesTotal = n_valid_batches
	print("num images in training set: "+str(nTrainingImagesTotal))
	print("num images in validation set: "+str(nValidationImagesTotal))
	n_train_batches /= builtCNN.batch_size
	n_valid_batches /= builtCNN.batch_size
	n_test_batches /= builtCNN.batch_size
	
	print("batch size == "+str(builtCNN.batch_size))
	print("num valid images actually used in training set: "+str(n_train_batches*builtCNN.batch_size))
	print("num valid images actually used in validation set: "+str(n_valid_batches*builtCNN.batch_size))
	print("num images MISSING from training set due to minibatch rounding: "+str(nTrainingImagesTotal - n_train_batches*builtCNN.batch_size))
	print("num images MISSING from validation set due to minibatch rounding: "+str(nValidationImagesTotal - n_valid_batches*builtCNN.batch_size))
	
	y = T.ivector('y')  # training labels are presented as 1D vector of [int] labels
	
	# allocate symbolic variables for the data
	index = T.lscalar()  # index to a [mini]batch
	
	# create a function to compute the mistakes that are made by the model
	traintest_model = theano.function(
		[index],
		builtCNN.allLayers[-1].errors(y),
		givens={
			builtCNN.x: train_set_x[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size],
			y: train_set_y[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size]
		}
	)
	test_model = theano.function(
		[index],
		builtCNN.allLayers[-1].errors(y),
		givens={
			builtCNN.x: test_set_x[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size],
			y: test_set_y[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size]
		}
	)
	validate_model = theano.function(
		[index],
		builtCNN.allLayers[-1].errors(y),
		givens={
			builtCNN.x: valid_set_x[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size],
			y: valid_set_y[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size]
		}
	)
	
	# compute zero-one loss on validation set
	# traintest_losses = [traintest_model(i) for i in xrange(n_train_batches)]
	# this_traintest_loss = numpy.mean(traintest_losses)
	
	# compute zero-one loss on validation set
	# print("computing validation score")
	# validation_losses = [validate_model(i) for i in xrange(n_valid_batches)]
	# this_validation_loss = numpy.mean(validation_losses)
	
	# test it on the test set
	print("computing test score")
	test_losses = [test_model(i) for i in xrange(n_test_batches)]
	test_score = numpy.mean(test_losses)
	
	# print("train set score: "+str(this_traintest_loss)+",  test score: "+str(test_score*100.))
	print("test score: "+str(test_score*100.))
	#======================================================================================================================



def predict_CNN_on_img(givenCNN, img, widthOfImage, batchSize=1, debuggingMode=False):
	
	imdata = img.astype(dtype=theano.config.floatX)
	
	for ij in range(len(imdata.shape)):
		if imdata.shape[ij] < widthOfImage:
			if ij == 0: #pad rows
				neededRows = (widthOfImage-imdata.shape[ij])
				print("padded with "+str(neededRows)+" rows")
				imdata = numpy.pad(imdata, [(neededRows/2,(widthOfImage-imdata.shape[ij])-(neededRows/2)),(0,0)], 'constant', constant_values=(0))
			else: #pad columns
				neededCols = (widthOfImage-imdata.shape[ij])
				print("padded with "+str(neededCols)+" columns")
				imdata = numpy.pad(imdata, [(0,0),(neededCols/2,(widthOfImage-imdata.shape[ij])-(neededCols/2))], 'constant', constant_values=(0))
	
	imdata = numpy.reshape(imdata, (1,widthOfImage*widthOfImage))
	if batchSize > 1:
		imdataCopy = numpy.copy(imdata)
		for i in range(batchSize-1):
			imdata = numpy.concatenate([imdata,imdataCopy])
		print("new numpy array shape: "+str(imdata.shape))
	
	# make prediction
	predim = T.matrix('predim')
	predict = theano.function(inputs=[predim], outputs=givenCNN.allLayers[-1].y_pred, givens={givenCNN.x: predim})
	
	CNNprediction = predict(imdata)
	
	if debuggingMode:
		print("the CNN predicted this: \""+str(CNNprediction)+"\"")
		print("maximum grayscale pixel value in image == "+str(numpy.amax(imdata)))
		import cv2
		cv2.imshow("testimg_processed 222", numpy.reshape(imdata,(widthOfImage,widthOfImage))/255.)
		cv2.waitKey(0)
	
	return CNNprediction[0]



def test_saved_net_on_image_in_memory(img, trainedWeightsFile):
	
	widthOfImages = myCNNParams().widthOfImages
	filter0Size = myCNNParams().filter0Size
	filter1Size = myCNNParams().filter1Size
	nkerns = myCNNParams().nkerns
	numOutClasses = myCNNParams().numOutClasses
	
	# construct CNN
	builtCNN = OCR_CNN(batch_size=1, useDropout=False, widthOfImages=widthOfImages, numOutClasses=numOutClasses, filter0Size=filter0Size, filter1Size=filter1Size, nkerns=nkerns)
	
	# load params layer-by-layer
	for nnlayeridx in range(len(builtCNN.allLayers)):
		builtCNN.allLayers[nnlayeridx].loadParams(trainedWeightsFile+".l"+str(nnlayeridx))
	
	return predict_CNN_on_img(builtCNN, img, widthOfImages, batchSize=1)



def test_saved_lenet5_on_image_file(testImageFile, wasGivenPrebuiltCNN, trainedWeightsFile="", builtCNN="", batchSize=1):
	
	widthOfImages = myCNNParams().widthOfImages
	filter0Size = myCNNParams().filter0Size
	filter1Size = myCNNParams().filter1Size
	nkerns = myCNNParams().nkerns
	numOutClasses = myCNNParams().numOutClasses
	
	# load and convert test image
	from PIL import Image
	im = Image.open(testImageFile)
	print("original size of image: "+str(im.size[0])+" columns, "+str(im.size[1])+" rows")
	im = im.convert("L")
	if im.size[0] != widthOfImages or im.size[1] != widthOfImages:
		im.thumbnail((widthOfImages,widthOfImages), Image.ANTIALIAS)
	
	if wasGivenPrebuiltCNN == False:
		# construct CNN
		builtCNN = OCR_CNN(batch_size=1, useDropout=False, widthOfImages=widthOfImages, numOutClasses=numOutClasses, filter0Size=filter0Size, filter1Size=filter1Size, nkerns=nkerns)
		
		# load params layer-by-layer
		for nnlayeridx in range(len(builtCNN.allLayers)):
			builtCNN.allLayers[nnlayeridx].loadParams(trainedWeightsFile+".l"+str(nnlayeridx))
	
	npim = numpy.asarray(im)
	im.close()
	return predict_CNN_on_img(builtCNN, npim, widthOfImages, batchSize=batchSize, debuggingMode=True)
	


if __name__ == '__main__':
	if len(sys.argv) < 2:
		print("args:  {train|retrain|test|testfull}  {test-savedweights}  {test-imagefile}")
		quit()
	if str(sys.argv[1]) == "train":
		if len(sys.argv) < 4:
			print("args:  {train|retrain|test|testfull}  {learning-rate}  {momentum}")
			quit()
		train_lenet5_with_batches(useDropout=True, learning_rate=float(sys.argv[2]), weightmomentum=float(sys.argv[3]))
	elif str(sys.argv[1]) == "retrain":
		if len(sys.argv) < 5:
			print("args:  {train|retrain|test|testfull}  {test-savedweights}  {learning-rate}  {momentum}")
			quit()
		train_lenet5_with_batches(useDropout=True, pretrainedWeightsFile=str(sys.argv[2]), learning_rate=float(sys.argv[3]), weightmomentum=float(sys.argv[4]))
	elif str(sys.argv[1]) == "testfull":
		if len(sys.argv) < 3:
			print("args:  {train|retrain|test|testfull}  {test-savedweights}")
			quit()
		test_saved_lenet5_on_full_dataset(False, trainedWeightsFile=str(sys.argv[2]), batchSize=499)
	elif str(sys.argv[1]) == "test":
		if len(sys.argv) < 4:
			print("args:  {train|retrain|test|testfull}  {test-savedweights}  {test-imagefile}")
			quit()
		test_saved_lenet5_on_image_file(str(sys.argv[3]), False, trainedWeightsFile=str(sys.argv[2]))
	else:
		print("unknown option \""+str(sys.argv[1])+"\"")


def experiment(state, channel):
	train_lenet5_with_batches(learning_rate=state.learning_rate, dataset=state.dataset)
