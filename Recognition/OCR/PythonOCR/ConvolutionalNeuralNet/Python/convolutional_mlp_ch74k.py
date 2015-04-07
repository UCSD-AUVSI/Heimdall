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
from DatasetsLoaders import load_chars74k
from DatasetsLoaders import load_MNIST
from mlp import HiddenLayer, DropoutHiddenLayer, _dropout_from_layer, _add_noise_to_input, ReLu


class LeNetConvPoolLayer(object):
	"""Pool Layer of a convolutional network """

	def __init__(self, rng, input, filter_shape, image_shape, poolsize=(2, 2)):
		"""
		Allocate a LeNetConvPoolLayer with shared variable internal parameters.

		:type rng: numpy.random.RandomState
		:param rng: a random number generator used to initialize weights

		:type input: theano.tensor.dtensor4
		:param input: symbolic image tensor, of shape image_shape

		:type filter_shape: tuple or list of length 4
		:param filter_shape: (number of filters, num input feature maps,
							  filter height, filter width)

		:type image_shape: tuple or list of length 4
		:param image_shape: (batch size, num input feature maps,
							 image height, image width)

		:type poolsize: tuple or list of length 2
		:param poolsize: the downsampling (pooling) factor (#rows, #cols)
		"""
		
		assert image_shape[1] == filter_shape[1]
		self.input = input
		
		# there are "num input feature maps * filter height * filter width"
		# inputs to each hidden unit
		fan_in = numpy.prod(filter_shape[1:])
		# each unit in the lower layer receives a gradient from:
		# "num output feature maps * filter height * filter width" /
		#   pooling size
		fan_out = (filter_shape[0] * numpy.prod(filter_shape[2:]) /
				   numpy.prod(poolsize))
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
		#self.output = T.tanh(pooled_out + self.b.dimshuffle('x', 0, 'x', 'x'))
		self.output = ReLu(pooled_out + self.b.dimshuffle('x', 0, 'x', 'x'))
		
		# store parameters of this layer
		self.params = [self.W, self.b]
	
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
	def __init__(self, rng, input, filter_shape, image_shape, dropout_rate, dropout_noise_rate, poolsize=(2, 2)):
		super(DropoutLeNetConvPoolLayer, self).__init__(
				rng=rng, input=input, filter_shape=filter_shape, image_shape=image_shape, poolsize=poolsize)
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


class OCR_CNN(object):
	def __init__(self, batch_size, useDropout, widthOfImages, numOutClasses, filter0Size, filter1Size, nkerns):
		
		self.rng = numpy.random.RandomState(23455)
		self.x = T.matrix('x')   # the data are presented as rasterized images
		self.batch_size = batch_size
		
		######################
		# BUILD ACTUAL MODEL #
		######################
		print '... building OCR_CNN model'
		print("nkerns == "+str(nkerns))
		print("filter sizes:  filter0 == "+str(filter0Size)+"x"+str(filter0Size)+",  filter1 == "+str(filter1Size)+"x"+str(filter1Size))
		
		# Reshape matrix of rasterized images of shape (batch_size, widthOfImages * widthOfImages)
		# to a 4D tensor, compatible with our LeNetConvPoolLayer
		# (widthOfImages, widthOfImages) is the size of images.
		layer0_input = self.x.reshape((batch_size, 1, widthOfImages, widthOfImages))
		firstConvLayerInputWidth = widthOfImages
		print("firstConvLayerInputWidth == "+str(firstConvLayerInputWidth))
		secondConvLayerInputWidth = (firstConvLayerInputWidth-filter0Size+1)/2
		print("secondConvLayerInputWidth == "+str(secondConvLayerInputWidth))
		thirdLayerHiddenInputWidth = (secondConvLayerInputWidth-filter1Size+1)/2
		print("thirdLayerHiddenInputWidth == "+str(thirdLayerHiddenInputWidth))
		
		if useDropout == False:
			# Construct the first convolutional pooling layer:
			# filtering reduces the image size to (widthOfImages-filter0Size+1 , widthOfImages-filter0Size+1) = (24, 24) for MNIST
			# maxpooling reduces this further to (24/2, 24/2) = (12, 12) for MNIST
			# 4D output tensor is thus of shape (batch_size, nkerns[0], 12, 12) for MNIST
			self.layer0 = LeNetConvPoolLayer(
				self.rng,
				input=layer0_input,
				image_shape=(batch_size, 1, firstConvLayerInputWidth, firstConvLayerInputWidth),
				filter_shape=(nkerns[0], 1, filter0Size, filter0Size),
				poolsize=(2, 2)
			)
			
			# Construct the second convolutional pooling layer
			# filtering reduces the image size to (secondConvLayerInputWidth-5+1, secondConvLayerInputWidth-5+1) = (8, 8) for MNIST
			# maxpooling reduces this further to (8/2, 8/2) = (4, 4) for MNIST
			# 4D output tensor is thus of shape (batch_size, nkerns[1], 4, 4) for MNIST
			self.layer1 = LeNetConvPoolLayer(
				self.rng,
				input=self.layer0.output,
				image_shape=(batch_size, nkerns[0], secondConvLayerInputWidth, secondConvLayerInputWidth),
				filter_shape=(nkerns[1], nkerns[0], filter1Size, filter1Size),
				poolsize=(2, 2)
			)
		
			# the HiddenLayer being fully-connected, it operates on 2D matrices of
			# shape (batch_size, num_pixels) (i.e matrix of rasterized images).
			# This will generate a matrix of shape (batch_size, nkerns[1] * 4 * 4),
			# or (batch_size, 50 * 4 * 4) = (batch_size, 800) with the default values.
			layer2_input = self.layer1.output.flatten(2)
		
		else:
			print("constructing dropout convolutional layers")
		
			# Construct the first convolutional pooling layer:
			# filtering reduces the image size to (widthOfImages-filter0Size+1 , widthOfImages-filter0Size+1) = (24, 24) for MNIST
			# maxpooling reduces this further to (24/2, 24/2) = (12, 12) for MNIST
			# 4D output tensor is thus of shape (batch_size, nkerns[0], 12, 12) for MNIST
			self.layer0 = DropoutLeNetConvPoolLayer(
				self.rng,
				input=layer0_input,
				image_shape=(batch_size, 1, firstConvLayerInputWidth, firstConvLayerInputWidth),
				filter_shape=(nkerns[0], 1, filter0Size, filter0Size),
				dropout_rate=0.2,
				dropout_noise_rate=0.2,
				poolsize=(2, 2)
			)
			
			# Construct the second convolutional pooling layer
			# filtering reduces the image size to (secondConvLayerInputWidth-5+1, secondConvLayerInputWidth-5+1) = (8, 8) for MNIST
			# maxpooling reduces this further to (8/2, 8/2) = (4, 4) for MNIST
			# 4D output tensor is thus of shape (batch_size, nkerns[1], 4, 4) for MNIST
			self.layer1 = DropoutLeNetConvPoolLayer(
				self.rng,
				input=self.layer0.output,
				image_shape=(batch_size, nkerns[0], secondConvLayerInputWidth, secondConvLayerInputWidth),
				filter_shape=(nkerns[1], nkerns[0], filter1Size, filter1Size),
				dropout_rate=0.4,
				dropout_noise_rate=0.1,
				poolsize=(2, 2)
			)
			
			# the HiddenLayer being fully-connected, it operates on 2D matrices of
			# shape (batch_size, num_pixels) (i.e matrix of rasterized images).
			# This will generate a matrix of shape (batch_size, nkerns[1] * 4 * 4),
			# or (batch_size, 50 * 4 * 4) = (batch_size, 800) with the default values.
			layer2_input = self.layer1.output.flatten(2)
		
		
		if useDropout == False:
			# construct a fully-connected sigmoidal layer
			self.layer2 = HiddenLayer(
				self.rng,
				input=layer2_input,
				n_in=nkerns[1] * thirdLayerHiddenInputWidth * thirdLayerHiddenInputWidth,
				n_out=batch_size,
				activation=ReLu #T.tanh
			)
			
			# classify the values of the fully-connected sigmoidal layer
			self.layer3 = LogisticRegression(input=self.layer2.output, n_in=batch_size, n_out=numOutClasses)
		
		else:
			dropout_rate = 0.45
			print("constructing dropout hidden layer")
			
			self.layer2 = DropoutHiddenLayer(rng=self.rng,
							input=_dropout_from_layer(self.rng, layer2_input, p=dropout_rate),
							n_in=nkerns[1] * thirdLayerHiddenInputWidth * thirdLayerHiddenInputWidth,
							n_out=batch_size,
							activation=ReLu, #T.tanh
							dropout_rate=dropout_rate,
							dropout_noise_rate=0)
			
			# classify the values of the fully-connected sigmoidal layer
			# Set up the output layer
			self.layer3 = LogisticRegression(
					input=self.layer2.output,  #layer2drop
					n_in=batch_size,
					n_out=numOutClasses)
		
		self.allLayers = [self.layer0, self.layer1, self.layer2, self.layer3]
		
	
	def Train(self, datasets, learning_rate, weightmomentum, n_epochs, pickleNetworkName):
		
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
		n_train_batches /= self.batch_size
		n_valid_batches /= self.batch_size
		n_test_batches /= self.batch_size
		
		print("batch size == "+str(self.batch_size))
		print("num valid images actually used in training set: "+str(n_train_batches*self.batch_size))
		print("num valid images actually used in validation set: "+str(n_valid_batches*self.batch_size))
		print("num images MISSING from training set due to minibatch rounding: "+str(nTrainingImagesTotal - n_train_batches*self.batch_size))
		print("num images MISSING from validation set due to minibatch rounding: "+str(nValidationImagesTotal - n_valid_batches*self.batch_size))
		
		y = T.ivector('y')  # training labels are presented as 1D vector of [int] labels
		
		# allocate symbolic variables for the data
		index = T.lscalar()  # index to a [mini]batch
		
		# create a function to compute the mistakes that are made by the model
		test_model = theano.function(
			[index],
			self.layer3.errors(y),
			givens={
				self.x: test_set_x[index * self.batch_size: (index + 1) * self.batch_size],
				y: test_set_y[index * self.batch_size: (index + 1) * self.batch_size]
			}
		)
		validate_model = theano.function(
			[index],
			self.layer3.errors(y),
			givens={
				self.x: valid_set_x[index * self.batch_size: (index + 1) * self.batch_size],
				y: valid_set_y[index * self.batch_size: (index + 1) * self.batch_size]
			}
		)
		
		# create a list of all model parameters to be fit by gradient descent
		params = self.layer3.params + self.layer2.params + self.layer1.params + self.layer0.params
		
		# the cost we minimize during training is the NLL of the model
		cost = self.layer3.negative_log_likelihood(y)
		
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
			epoch = epoch + 1
			for minibatch_index in xrange(n_train_batches):
			
				iter = (epoch - 1) * n_train_batches + minibatch_index
			
				if iter % 100 == 0:
					print 'training @ iter = ', iter
				cost_ij = train_model(minibatch_index)
			
				if (iter + 1) % validation_frequency == 0:
				
					# compute zero-one loss on validation set
					validation_losses = [validate_model(i) for i
										 in xrange(n_valid_batches)]
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
						
						# test it on the test set
						test_losses = [
							test_model(i)
							for i in xrange(n_test_batches)
						]
						test_score = numpy.mean(test_losses)
						print(('   epoch %i, minibatch %i/%i, test error of '
							   'best model %f %%') %
							  (epoch, minibatch_index + 1, n_train_batches,
							   test_score * 100.))
						
						pfname = pickleNetworkName+"_"+str(iter)+"_score_"+str(test_score*100.)+".pkl"
						for nnlayeridx in range(len(self.allLayers)):
							self.allLayers[nnlayeridx].saveParams(pfname+".l"+str(nnlayeridx))

				
				if patience <= iter:
					done_looping = True
					break
	
		end_time = time.clock()
		print('Optimization complete.')
		print('Best validation score of %f %% obtained at iteration %i, '
			  'with test performance %f %%' %
			  (best_validation_loss * 100., best_iter + 1, test_score * 100.))
		print >> sys.stderr, ('The code for file ' +
							  os.path.split(__file__)[1] +
							  ' ran for %.2fm' % ((end_time - start_time) / 60.))


#=========================================================================================================================
#
class myCNNParams(object):
	def __init__(self):
		self.widthOfImages = 40
		self.filter0Size = 7
		self.filter1Size = 5
		self.nkerns=[24, 54]
		self.numOutClasses = 36



def train_lenet5_with_batches(useMNIST=False, learning_rate=0.07, n_epochs=2000, useDropout=True, batch_size=499):
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
	weightmomentum = 0.75
	
	if useMNIST:
		widthOfImages = 28
		filter0Size = 5
		filter1Size = 5
		nkerns=[20, 50]
		numOutClasses = 10
		datasets = load_MNIST('mnist.pkl.gz')
		pickleNetworkName = "cnn28x28MNISTtheano_paramsWb"
	else:
		widthOfImages = myCNNParams().widthOfImages
		filter0Size = myCNNParams().filter0Size
		filter1Size = myCNNParams().filter1Size
		nkerns = myCNNParams().nkerns
		numOutClasses = myCNNParams().numOutClasses
		datasets = load_chars74k(widthOfImages)
		pickleNetworkName = "/media/ucsdauvsi/442ABBE92ABBD660/OCR_Neural_Network_Backups/weights_saved/cnn40x40theano_paramsWb"
	
	
	myCNN = OCR_CNN(batch_size=batch_size, useDropout=useDropout, widthOfImages=widthOfImages, numOutClasses=numOutClasses, filter0Size=filter0Size, filter1Size=filter1Size, nkerns=nkerns)
	
	myCNN.Train(datasets, learning_rate=learning_rate, weightmomentum=weightmomentum, n_epochs=n_epochs, pickleNetworkName=pickleNetworkName)
	


def test_saved_lenet5_on_full_dataset(wasGivenPrebuiltCNN, trainedWeightsFile="", builtCNN="", batchSize=1):
	
	widthOfImages = myCNNParams().widthOfImages
	filter0Size = myCNNParams().filter0Size
	filter1Size = myCNNParams().filter1Size
	nkerns = myCNNParams().nkerns
	numOutClasses = myCNNParams().numOutClasses
	datasets = load_chars74k(widthOfImages)
	
	if wasGivenPrebuiltCNN == False:
		# construct CNN
		builtCNN = OCR_CNN(batch_size=batchSize, useDropout=False, widthOfImages=widthOfImages, numOutClasses=numOutClasses, filter0Size=filter0Size, filter1Size=filter1Size, nkerns=nkerns)
		
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
		builtCNN.layer3.errors(y),
		givens={
			builtCNN.x: train_set_x[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size],
			y: train_set_y[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size]
		}
	)
	test_model = theano.function(
		[index],
		builtCNN.layer3.errors(y),
		givens={
			builtCNN.x: test_set_x[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size],
			y: test_set_y[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size]
		}
	)
	validate_model = theano.function(
		[index],
		builtCNN.layer3.errors(y),
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
	predict = theano.function(inputs=[predim], outputs=givenCNN.layer3.y_pred, givens={givenCNN.x: predim})
	
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
		print("args:  {train|test|testfull}  {test-savedweights}  {test-imagefile}")
		quit()
	if str(sys.argv[1]) == "train":
		train_lenet5_with_batches(useDropout=True)
	elif str(sys.argv[1]) == "testfull":
		if len(sys.argv) < 3:
			print("args:  {train|test|testfull}  {test-savedweights}")
			quit()
		test_saved_lenet5_on_full_dataset(False, trainedWeightsFile=str(sys.argv[2]), batchSize=499)
	elif str(sys.argv[1]) == "test":
		if len(sys.argv) < 4:
			print("args:  {train|test|testfull}  {test-savedweights}  {test-imagefile}")
			quit()
		test_saved_lenet5_on_image_file(str(sys.argv[3]), False, trainedWeightsFile=str(sys.argv[2]))
	else:
		print("unknown option \""+str(sys.argv[1])+"\"")


def experiment(state, channel):
	train_lenet5_with_batches(learning_rate=state.learning_rate, dataset=state.dataset)
