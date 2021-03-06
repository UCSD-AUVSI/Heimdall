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
from myworkpath import myworkpath

import numpy

import theano
import theano.tensor as T
from theano.tensor.signal import downsample
from theano.tensor.nnet import conv

import linear_svm_multiclass
from logistic_sgd import LogisticRegression
from mlp import HiddenLayer, DropoutHiddenLayer, _dropout_from_layer, _add_noise_to_input, ReLu

# for AdaDelta
from collections import OrderedDict
def build_shared_zeros(shape, name):
    """ Builds a theano shared variable filled with a zeros numpy array """
    return theano.shared(value=numpy.zeros(shape, dtype=theano.config.floatX), name=name, borrow=True)

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
        :param filter_shape: (number of filters, num input feature channels, filter height, filter width)
        
        :type image_shape: tuple or list of length 4
        :param image_shape: (batch size, num input feature maps, image height, image width)
        
        :type poolsize: tuple or list of length 2
        :param poolsize: the downsampling (pooling) factor (#rows, #cols)
        """
        
        assert image_shape[1] == filter_shape[1]
        self.input = input
        
        outpimsize = (image_shape[3] - filter_shape[3] + 1)
        #print("constructing LeNetConvPoolLayer... image_shape == "+str(image_shape)+", filter_shape == "+str(filter_shape)+", num outputs == "+str(filter_shape[0]*outpimsize*outpimsize/(poolsize[0]*poolsize[1])))
        
        # save a copy of input filter shape
        self.filter_shape = filter_shape
        
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
        
        if numpy.prod(poolsize) > 1:
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
        else:
            self.output = activation(conv_out + self.b.dimshuffle('x', 0, 'x', 'x'))
        
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
    
    def resetParams(self):
        fan_in = numpy.prod(self.filter_shape[1:])
        fan_out = (self.filter_shape[0] * numpy.prod(self.filter_shape[2:]) / numpy.prod(poolsize))
        W_bound = numpy.sqrt(6. / (fan_in + fan_out))
        self.W.set_value(numpy.asarray(rng.uniform(low=-W_bound, high=W_bound, size=self.filter_shape), dtype=theano.config.floatX), borrow=True)
        b_values = numpy.zeros((self.filter_shape[0],), dtype=theano.config.floatX)
        self.b.set_value(b_values, borrow=True)
    
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
        self.lastlayerLogistic = True
        self.numOutClasses = 36
        # misc options due to memory constraints
        self.microbatches = False
        self.batchSize = 400
        self.deviceTrainSetSize=64800
        self.deviceValidSetSize=4800
class myCNNForOrientationParams(myCNNParams):
    def __init__(self):
        myCNNParams.__init__(self)
        self.nkerns=[200, 320, 200]
        self.hiddenlayers = [2400, 1000]
        self.numOutClasses = (36*4)
        # misc options due to memory constraints
        self.deviceTrainSetSize=36000
        self.deviceValidSetSize=3200
        self.batchSize = 100
        self.microbatches = True # use microbatches: only one minibatch i.e. "batchSize" images are on the GPU at any time
        print("initializing myCNNForOrientation with "+str(self.numOutClasses)+" classes!!!!!!!")

class myCNNForOrientationParams222(myCNNParams):
    def __init__(self):
        myCNNParams.__init__(self)
        # conv layers
        self.nkerns=[330, 300, 300, 300]
        self.filtersizes = [5, 3, 3, 3]
        self.poolsizes = [2, 2, 1, 1]
        self.dropoutrates_conv = [0.1, 0.1, 0.1, 0.1]
        self.noiserates_conv = [0.2, 0, 0, 0]
        # hidden layers
        self.dropoutrates_fullyconn = [0.3, 0.3]
        self.hiddenlayers = [1600, 2000]
        # classifier
        self.numOutClasses = (36*4) + 1
        # training
        self.deviceTrainSetSize=48960
        self.deviceValidSetSize=9900
        self.batchSize = 90
        self.microbatches = True # use microbatches: only one minibatch i.e. "batchSize" images are on the GPU at any time
        print("initializing myCNNForOrientation222 with "+str(self.numOutClasses)+" classes!!!!!!!")

class myCNNForOrientationParams555july(myCNNParams):
    def __init__(self):
        myCNNParams.__init__(self)
        # conv layers
        self.nkerns=[330, 300, 300, 300]
        self.filtersizes = [5, 3, 3, 3]
        self.poolsizes = [2, 2, 1, 1]
        self.dropoutrates_conv = [0.1, 0.1, 0.1, 0.1]
        self.noiserates_conv = [0.2, 0, 0, 0]
        # hidden layers
        self.dropoutrates_fullyconn = [0.3, 0.3]
        self.hiddenlayers = [1600, 2000]
        # classifier
        self.numOutClasses = (36*4) + 1
        # training
        self.deviceTrainSetSize=45000
        self.deviceValidSetSize=10000
        self.batchSize = 100
        self.microbatches = False # use microbatches: only one minibatch i.e. "batchSize" images are on the GPU at any time
        print("initializing myCNNForOrientation555july with "+str(self.numOutClasses)+" classes!!!!!!!")

class myCNNForOrientationParamsSTUBTEMPLATE(myCNNParams):
    def __init__(self):
        myCNNParams.__init__(self)
        # conv layers
        self.nkerns=None
        self.filtersizes = None
        self.poolsizes = None
        self.dropoutrates_conv = None
        self.noiserates_conv = None
        # hidden layers
        self.dropoutrates_fullyconn = None
        self.hiddenlayers = None
        # classifier
        self.numOutClasses = (36*4) + 1
        # training
        self.deviceTrainSetSize=45000
        self.deviceValidSetSize=10000
        self.batchSize = 100
        self.microbatches = False # use microbatches: only one minibatch i.e. "batchSize" images are on the GPU at any time
        print("initializing myCNNForOrientationParamsSTUBTEMPLATE with "+str(self.numOutClasses)+" classes!!!!!!!")

class myCNNForOrientationParams333May26(myCNNParams):
    def __init__(self):
        myCNNParams.__init__(self)
        # conv layers
        self.nkerns=[200, 400, 600, 320]
        self.filtersizes = [5, 3, 3, 3]
        self.poolsizes = [2, 2, 1, 1]
        self.dropoutrates_conv = [0.1, 0.1, 0.1, 0.1]
        self.noiserates_conv = [0.2, 0, 0, 0]
        # hidden layers
        self.dropoutrates_fullyconn = [0.3, 0.3]
        self.hiddenlayers = [1100, 2400]
        # classifier
        self.numOutClasses = (36*4)
        # training
        self.deviceTrainSetSize=50000
        self.deviceValidSetSize=10000
        self.batchSize = 125
        self.microbatches = True # use microbatches: only one minibatch i.e. "batchSize" images are on the GPU at any time
        print("initializing myCNNForOrientationParams333May26 with "+str(self.numOutClasses)+" classes!!!!!!!")

class myCNNForMoreOrientationsSVMParams(myCNNParams):
    def __init__(self):
        myCNNParams.__init__(self)
        self.widthOfImages = 40
        self.activation = ReLu
        # convolutional layers
        self.filtersizes = [5, 3, 3, 3]
        self.poolsizes = [2, 2, 1, 1]
        self.nkerns=[1000, 600, 600, 500]
        self.dropoutrates_conv = [0.2, 0.2, 0.2, 0.2]
        self.noiserates_conv = [0.2, 0, 0, 0]
        # connected layers
        self.dropoutrates_fullyconn = [0.4, 0.4]
        self.hiddenlayers = [1440, 2880]
        self.numOutClasses = (36*4)
        # classification layer
        self.lastlayerLogistic = True # according to the name... should be SVM... but use logistic for ease of train
        # misc options due to memory constraints
        self.microbatches = True # use microbatches: only one minibatch i.e. "batchSize" images are on the GPU at any time
        #self.batchSize = 500
        #self.deviceTrainSetSize=50000
        #self.deviceValidSetSize=10000
        self.deviceTrainSetSize=27000
        self.deviceValidSetSize=3600
        self.batchSize = 200
        print("initializing myCNNForMoreOrientationsSVMParams with "+str(self.numOutClasses)+" classes!!!!!!!")

class defaultCNNParams(myCNNForOrientationParams555july):
    def __init__(self):
        myCNNForOrientationParams555july.__init__(self)

class OCR_CNN(object):
    def __init__(self, batch_size, useDropout, params):
        
        self.rng = numpy.random.RandomState(23455)
        self.xx = T.matrix('xx')   # the data are presented as rasterized images
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
            print("~~~~~~~~~~~ will construct layers with dropouts and noise!!!")
        
        # Reshape matrix of rasterized images of shape (batch_size, widthOfImages * widthOfImages)
        # to a 4D tensor, compatible with our LeNetConvPoolLayer
        # (widthOfImages, widthOfImages) is the size of images.
        nextinput = self.xx.reshape((batch_size, 1, params.widthOfImages, params.widthOfImages))
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
        
        if len(params.hiddenlayers) > 0:
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
        
        if params.lastlayerLogistic:
            newlayer = LogisticRegression(input=nextinput, n_in=nextnumin, n_out=params.numOutClasses)
        else:
            newlayer = linear_svm_multiclass.LinearSVMMulticlass(input=nextinput, nfeatures=nextnumin, nclasses=params.numOutClasses, C=1)
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
    
    def TrainHyperoptMicrobatches(self, datasets, datasetsOnDevice, pickleNetworkName, filterFilesSavedBaseName, deviceTrainSetSize, deviceValidSetSize):
        quit()
    
    def Train(self, datasets, datasetsOnDevice, learning_rate, weightmomentum, n_epochs, pickleNetworkName, filterFilesSavedBaseName="", deviceTrainSetSize=36000, deviceValidSetSize=3200):
        
        import DatasetsLoaders
        
        if datasetsOnDevice:
            train_set_x, train_set_y, valid_set_x, valid_set_y, datasets, numTrainPts, numValidationPts = DatasetsLoaders.send_host_datasets_to_device(datasets, deviceTrainSetSize, deviceValidSetSize, False, self.params.numOutClasses)
            # compute number of minibatches for training, validation and testing
            n_train_batches = deviceTrainSetSize
            n_valid_batches = deviceValidSetSize
            #n_test_batches = test_set_x.get_value(borrow=True).shape[0]
            nTrainingImagesTotal = n_train_batches
            nValidationImagesTotal = n_valid_batches
            print("num images in training set: "+str(nTrainingImagesTotal))
            print("num images in validation set: "+str(nValidationImagesTotal))
            n_train_batches /= self.batch_size
            n_valid_batches /= self.batch_size
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
        
        if datasetsOnDevice and not self.params.microbatches:
            # create a function to compute the mistakes that are made by the model
            validate_model = theano.function(
                [index],
                self.allLayers[-1].calculateErrors(y),
                givens={
                    self.xx: valid_set_x[index * self.batch_size: (index + 1) * self.batch_size],
                    y: valid_set_y[index * self.batch_size: (index + 1) * self.batch_size]
                }
            )
        
        # create a list of all model parameters to be fit by gradient descent
        params = self.allLayers[0].params
        for lidx in range(len(self.allLayers)):
            if lidx > 0:
                params = (params + self.allLayers[lidx].params)
        
        # the cost we minimize during training is the NLL of the model
        cost = self.allLayers[-1].CostToMinimize(y)
        
        #-----------------------------------------------------------------------
        # ADADELTA: https://gist.github.com/SnippyHolloW/67effa81dd1cd5a488b4
        _rho = 0.9  # ``momentum'' for adadelta
        _eps = 1.E-6  # epsilon for adadelta
        _accugrads = []  # for adadelta
        _accudeltas = []  # for adadelta
        _accugrads.extend([build_shared_zeros(t.shape.eval(), 'accugrad') for t in params])
        _accudeltas.extend([build_shared_zeros(t.shape.eval(), 'accudelta') for t in params])
        updatesADA = OrderedDict()
        #-----------------------------------------------------------------------
        
        # update function (uses stochastic gradient descent with momentum)
        updates = gradient_updates_momentum(cost, params, learning_rate, momentum=weightmomentum)
        
        if datasetsOnDevice and not self.params.microbatches:
            print("SETTING UP TRAINING FOR DATASETS ON DEVICE AND NOT MICROBATCHES")
            train_model = theano.function(
                [index],
                cost,
                updates=updates,
                givens={
                    self.xx: train_set_x[index * self.batch_size: (index + 1) * self.batch_size],
                    y: train_set_y[index * self.batch_size: (index + 1) * self.batch_size]
                }
            )
            gparams = T.grad(cost, params)
            for accugrad, accudelta, param, gparam in zip(_accugrads, _accudeltas, params, gparams):
                # c.f. Algorithm 1 in the Adadelta paper (Zeiler 2012)
                agrad = _rho * accugrad + (1 - _rho) * gparam * gparam
                dx = - T.sqrt((accudelta + _eps) / (agrad + _eps)) * gparam
                updatesADA[accudelta] = (_rho * accudelta + (1 - _rho) * dx * dx)
                updatesADA[param] = param + dx
                updatesADA[accugrad] = agrad
                train_ADA_model = theano.function(inputs=[index],
                        outputs=cost,
                        updates=updatesADA,
                        givens={
                            self.xx: train_set_x[index * self.batch_size: (index + 1) * self.batch_size],
                            y: train_set_y[index * self.batch_size: (index + 1) * self.batch_size]
                        })
        
        #------------------------------------------------------------------------
        if self.params.microbatches:
            print("SETTING UP TRAINING FOR MICROBATCHES")
            
            train_model = theano.function(
                [self.xx, T.cast(y,'int32')],
                cost,
                updates=updates
            )
            validate_model = theano.function(
                [self.xx, T.cast(y,'int32')],
                self.allLayers[-1].calculateErrors(y)
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
        
        savedSamplesOnceDone = True #if true, never save samples
        best_validation_loss = numpy.inf
        best_iter = 0
        test_score = 0.
        start_time = time.clock()
        
        epoch = 0
        done_looping = False
        
        #print("USING ADADELTA ~~~~~~~~~~~~")
        #print("USING ADADELTA ~~~~~~~~~~~~")
        print("USING ADADELTA ~~~~~~~~~~~~")
        try:
            while epoch < n_epochs:
            
                train_set_x, train_set_y, valid_set_x, valid_set_y, datasets, numTrainPts, numValidationPts = DatasetsLoaders.send_host_datasets_to_device(datasets, deviceTrainSetSize, deviceValidSetSize, self.params.microbatches, self.params.numOutClasses)
                
                if savedSamplesOnceDone == False:
                    savedSamplesOnceDone = True
                    import cv2
                    print("SAVING SAMPLES")
                    samplesfolder = myworkpath+"/samples/"
                    lentrainsety = 0
                    if datasetsOnDevice and not self.params.microbatches:
                        randsamples = numpy.random.randint(0, len(datasets[3][1].get_value(borrow=True)), size=500)
                    else:
                        randsamples = numpy.random.randint(0, train_set_x.get_value(borrow=True).shape[0], size=500)
                        print("train_set_x.get_value(borrow=True).shape[0] === "+str(train_set_x.get_value(borrow=True).shape[0]))
                    #print("indices chosen: "+str(randsamples))
                    numsaveddd = 0
                    print("--------------------------------------------------- "+str(len(randsamples)))
                    if len(randsamples) != randsamples.shape[0]:
                        print("What? len(randsamples) != randsamples.shape[0]: "+str(len(randsamples))+" != "+str(randsamples.shape[0]))
                    for ii in range(randsamples.shape[0]):
                        numsaveddd = (numsaveddd + 1)
                        #print(str(randsamples[ii]))
                        if datasetsOnDevice:# and not self.params.microbatches:
                            nparr = train_set_x.get_value(borrow=True)[randsamples[ii]]
                            nparr = numpy.reshape(nparr, (40,40))
                            samplleimgfilename = samplesfolder+"imgnist_"+str(datasets[3][1].get_value(borrow=True)[randsamples[ii]])+"__idx_"+str(randsamples[ii])+".png"
                        else:
                            nparr = train_set_x[randsamples[ii]]
                            nparr = numpy.reshape(nparr, (40,40))
                            samplleimgfilename = samplesfolder+"imgnist_"+str(train_set_y[randsamples[ii]])+"__idx_"+str(randsamples[ii])+".png"
                        cv2.imwrite(samplleimgfilename, nparr)
                        #print("saved \'"+samplleimgfilename+"\'")
                    print("done saving "+str(numsaveddd)+" samples")
                    #quit()
                
                if datasetsOnDevice == False and not self.params.microbatches:
                    print("SETTING UP TRAINING FOR DATASETS NOT ON DEVICE")
                    # we need to produce a dataset to train this iteration
                    
                    if numTrainPts < deviceTrainSetSize or numValidationPts < deviceValidSetSize:
                        print("warning: there were fewer training points in the dataset than could be requested... num points available: "+str(numTrainPts))
                        n_train_batches = numTrainPts/self.batch_size
                        n_valid_batches = numValidationPts/self.batch_size
                    
                    train_model = theano.function(
                        [index],
                        cost,
                        updates=updates,
                        givens={
                            self.xx: train_set_x[index * self.batch_size: (index + 1) * self.batch_size],
                            y: train_set_y[index * self.batch_size: (index + 1) * self.batch_size]
                        }
                    )
                    validate_model = theano.function(
                        [index],
                        self.allLayers[-1].calculateErrors(y),
                        givens={
                            self.xx: valid_set_x[index * self.batch_size: (index + 1) * self.batch_size],
                            y: valid_set_y[index * self.batch_size: (index + 1) * self.batch_size]
                        }
                    )
                #------------------------------------------------------------------------
                st1epc = time.clock()
                
                epoch = epoch + 1
                for minibatch_index in xrange(n_train_batches):
                    
                    iter = (epoch - 1) * n_train_batches + minibatch_index
                    if iter % 100 == 0:
                        print 'training @ iter = ', iter
                    
                    if self.params.microbatches:
                        print("abort please, microbatches??????????????")
                        cost_ij = train_model(train_set_x[minibatch_index*self.batch_size : (minibatch_index+1)*self.batch_size], train_set_y[minibatch_index*self.batch_size : (minibatch_index+1)*self.batch_size])
                    else:
                        #print("using adadelta")
                        cost_ij = train_ADA_model(minibatch_index)
                        #cost_ij = train_model(minibatch_index)
                
                ##et1epc = time.clock()
                ##print("time for that epoch == "+str(et1epc-st1epc)+" seconds")
                ##quit()
                #------------------------------------------------------------------------
                # compute error on validation set
                
                if self.params.microbatches:
                    validation_losses = [validate_model(train_set_x[minibatch_index*self.batch_size : (minibatch_index+1)*self.batch_size], train_set_y[minibatch_index*self.batch_size : (minibatch_index+1)*self.batch_size]) for minibatch_index in xrange(n_valid_batches)]
                else:
                    validation_losses = [validate_model(i) for i in xrange(n_valid_batches)]
                
                this_validation_loss = numpy.mean(validation_losses)
                print('epoch %i, minibatch %i/%i, validation error %f %%' %
                      (epoch, minibatch_index + 1, n_train_batches,
                       this_validation_loss * 100.))
                
                if (this_validation_loss < best_validation_loss) or (epoch % 4 == 0):
                    if pickleNetworkName is not None:
                        pfname = pickleNetworkName+"_"+str(int(round(self.batch_size*n_train_batches*epoch*0.001)))+"kims_score_"+str(round(this_validation_loss*100.,2))+"_lr"+str(learning_rate)+"_mom"+str(weightmomentum)+"_batch"+str(self.batch_size)+"_time"+str(round((time.clock()-start_time)/60.,2))+".pkl"
                        for nnlayeridx in range(len(self.allLayers)):
                            self.allLayers[nnlayeridx].saveParams(pfname+".l"+str(nnlayeridx))
                        if filterFilesSavedBaseName is not None and len(filterFilesSavedBaseName) > 1:
                            for layeridx in range(len(self.convLayers)):
                                self.convLayers[layeridx].saveFilters(filterFilesSavedBaseName+"_"+str(layeridx)+"_")
                        print("====== saved new weights and filters")
                
                # if we got the best validation score until now; save it to disk
                if this_validation_loss < best_validation_loss:
                
                    #improve patience if loss improvement is good enough
                    if this_validation_loss < best_validation_loss *  \
                       improvement_threshold:
                        patience = max(patience, iter * patience_increase)
                
                    # save best validation scores and iteration number
                    best_validation_loss = this_validation_loss
                    best_iter = iter
            # DONE with all epochs
            print(str(best_validation_loss))
            print(str(best_validation_loss))
            print(str(best_validation_loss))
            print(str(best_validation_loss))
        except (KeyboardInterrupt, SystemExit):
            end_time = time.clock()
            print("Optimization complete.")
            print('Best validation score of %f %% obtained at iteration %i, '
                  'with test performance %f %%' %
                  (best_validation_loss * 100., best_iter + 1, 0.))
            print("Ran for " + str((end_time - start_time) / 60.) + " minutes")




def train_lenet5_with_batches(useMNIST=False, learning_rate=0.04, weightmomentum=0.75, n_epochs=1000, useDropout=True, batch_size=300, pretrainedWeightsFile="", givenparams=None):
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
    import DatasetsLoaders
    
    if useMNIST:
        params = myCNNParams()
        params.widthOfImages = 28
        params.filtersizes = [5, 5]
        params.nkerns=[20, 50]
        params.poolsizes = [2, 2]
        numOutClasses = 10
        datasets = DatasetsLoaders.load_MNIST('mnist.pkl.gz')
        datasetsOnDevice = True
        pickleNetworkName = "cnn28x28MNISTtheano_paramsWb"
        filterFilesSavedBaseName = ""
        batch_size = 400
        deviceTrainSetSize = 50000
        deviceValidSetSize = 10000
    else:
        print("learning rate == "+str(learning_rate)+", momentum == "+str(weightmomentum))
        if givenparams is not None:
            params = givenparams
        else:
            params = defaultCNNParams()
        batch_size = params.batchSize
        deviceTrainSetSize = params.deviceTrainSetSize
        deviceValidSetSize = params.deviceValidSetSize
        datasets = DatasetsLoaders.load_chars74k(params.widthOfImages)
        datasetsOnDevice = True
        pickleNetworkName = myworkpath+"/weights_saved/cnn40x40theano_paramsWb"
        filterFilesSavedBaseName = myworkpath+"/filters_saved/filt"
    
    myCNN = OCR_CNN(batch_size=batch_size, useDropout=useDropout, params=params)
    
    if len(pretrainedWeightsFile) > 1:
        print("loading pretrained weights!")
        # load params layer-by-layer
        for nnlayeridx in range(len(myCNN.allLayers)):
            myCNN.allLayers[nnlayeridx].loadParams(pretrainedWeightsFile+".l"+str(nnlayeridx))
    
    myCNN.Train(datasets, datasetsOnDevice=datasetsOnDevice, learning_rate=learning_rate, weightmomentum=weightmomentum, n_epochs=n_epochs, pickleNetworkName=pickleNetworkName, filterFilesSavedBaseName=filterFilesSavedBaseName, deviceTrainSetSize=deviceTrainSetSize, deviceValidSetSize=deviceValidSetSize)
    


def test_saved_lenet5_on_full_dataset(wasGivenPrebuiltCNN, trainedWeightsFile="", builtCNN="", batchSize=1):
    
    import DatasetsLoaders
    datasets = DatasetsLoaders.load_chars74k(defaultCNNParams().widthOfImages)
    datasetsOnDevice = False
    
    if wasGivenPrebuiltCNN == False:
        # construct CNN
        builtCNN = OCR_CNN(batch_size=batchSize, useDropout=False, params=defaultCNNParams())
        
        # load params layer-by-layer
        for nnlayeridx in range(len(builtCNN.allLayers)):
            builtCNN.allLayers[nnlayeridx].loadParams(trainedWeightsFile+".l"+str(nnlayeridx))
    
    #------------------------------------------------------------------------
    numTrainPts = -1
    if datasetsOnDevice == False:
        # we need to produce a dataset to train this iteration
        deviceTrainSetSize = 70000
        deviceValidSetSize = batchSize
        
        train_set_x, train_set_y, valid_set_x, valid_set_y, datasets, numTrainPts, numValidationPts = DatasetsLoaders.send_host_datasets_to_device(datasets, deviceTrainSetSize, deviceValidSetSize, self.params.microbatches, self.params.numOutClasses)
        if numTrainPts < deviceTrainSetSize or numValidationPts < deviceValidSetSize:
            print("warning: there were fewer training points in the dataset than could be requested... num points available: "+str(numTrainPts))
            n_train_batches = numTrainPts/self.batch_size
            n_valid_batches = numValidationPts/self.batch_size
    #------------------------------------------------------------------------
    
    # compute number of minibatches for training, validation and testing
    n_train_batches = train_set_x.get_value(borrow=True).shape[0]
    n_valid_batches = valid_set_x.get_value(borrow=True).shape[0]
    #n_test_batches = test_set_x.get_value(borrow=True).shape[0]
    nTrainingImagesTotal = n_train_batches
    nValidationImagesTotal = n_valid_batches
    print("num images in training set: "+str(nTrainingImagesTotal))
    print("num images in validation set: "+str(nValidationImagesTotal))
    n_train_batches /= builtCNN.batch_size
    n_valid_batches /= builtCNN.batch_size
    #n_test_batches /= builtCNN.batch_size
    
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
        builtCNN.allLayers[-1].calculateErrors(y),
        givens={
            builtCNN.xx: train_set_x[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size],
            y: train_set_y[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size]
        }
    )
    validate_model = theano.function(
        [index],
        builtCNN.allLayers[-1].calculateErrors(y),
        givens={
            builtCNN.xx: valid_set_x[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size],
            y: valid_set_y[index * builtCNN.batch_size: (index + 1) * builtCNN.batch_size]
        }
    )
    
    # compute zero-one loss on train set
    traintest_losses = [traintest_model(i) for i in xrange(n_train_batches)]
    this_traintest_loss = numpy.mean(traintest_losses)
    print("score on "+str(numTrainPts)+" points from training set: "+str(100.0*this_traintest_loss))
    
    # compute zero-one loss on validation set
    # validation_losses = [validate_model(i) for i in xrange(n_valid_batches)]
    # this_validation_loss = numpy.mean(validation_losses)
    
    # test it on the test set
    #print("computing test score")
    #test_losses = [test_model(i) for i in xrange(n_test_batches)]
    #test_score = numpy.mean(test_losses)
    
    # print("train set score: "+str(this_traintest_loss)+",  test score: "+str(test_score*100.))
    #print("test score: "+str(test_score*100.))
    #======================================================================================================================


'''
    ConvertOrientationClassToDegrees()
        convert to a system where 0 is top-is-up, 90 is top-is-right, 180 is upside down, 270 is top-is-left
        this is clockwise from vertical
'''
def ConvertOrientationClassToDegrees(classOutOf144):
    if classOutOf144 == 144:
        return 0
    anglepred = 0
    if classOutOf144 >= 36:
        anglepred = 270
        classOutOf144 = (classOutOf144 - 36)
        if classOutOf144 >= 36:
            anglepred = 180
            classOutOf144 = (classOutOf144 - 36)
            if classOutOf144 >= 36: 
                anglepred = 90
                classOutOf144 = (classOutOf144 - 36)
    return anglepred


'''
    ConvertClassToCharPlusOrientation()
        this only works when converting from
        1 of 144 classes to orientations at 90 degree offsets
'''
def ConvertClassToCharPlusOrientation(prediction):
    if prediction == 144:
        return ('#','N/A') #if trained with 145 classes
    anglepred = "top-is-up"
    if prediction >= 36:
        anglepred = "top-is-left"
        prediction = (prediction - 36)
        if prediction >= 36:
            anglepred = "top-is-down"
            prediction = (prediction - 36)
            if prediction >= 36: 
                anglepred = "top-is-right"
                prediction = (prediction - 36)
    if prediction < 10:
        predchar = chr(prediction+48)
    else:
        predchar = chr(prediction+55)
    return (predchar, anglepred)


'''
    cv2rotateImg()
        OpenCV doesnt have an imrotate() function, so this is how you do it
'''
def cv2rotateImg(image,angledegrees):
    import cv2
    center = tuple(numpy.array(image.shape)/2)
    shape = tuple(image.shape)
    return cv2.warpAffine(image, cv2.getRotationMatrix2D(center,angledegrees,1.), dsize=shape, flags=cv2.INTER_LINEAR)
                

'''
    predict_CNN_on_img()
        Use a saved CNN to make a prediction given one image
'''
def predict_CNN_on_img(givenCNN, img, widthOfImage, debuggingMode=False, anglesInbetween=0, includesJunkChar=False, junkConfThreshold=0.25, returnDetailedInfo=False, numTopGuessesToReturn=1):
    
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
    
    predim = T.matrix('predim') #symbolic variable representing image array, used to compile prediction function
    
    if anglesInbetween == 0:
        # simple prediction based on argmax of outputs, i.e. what it was trained for
        
        predict = theano.function(inputs=[predim], outputs=givenCNN.allLayers[-1].y_pred, givens={givenCNN.xx: predim})
        CNNprediction = predict(imdata)
        
        if debuggingMode:
            print("the CNN predicted this: \""+str(CNNprediction)+"\"")
            print("maximum grayscale pixel value in image == "+str(numpy.amax(imdata)))
            import cv2
            cv2.imshow("testimg_processed xxx", numpy.reshape(imdata,(widthOfImage,widthOfImage))/255.)
            cv2.waitKey(0)
        #print("numTopGuessesToReturn ignored when no inbetween angles are checked.....")
        return ConvertClassToCharPlusOrientation(CNNprediction[0])
    
    else:
        import cv2
        # consider in-between angles such as 45 degrees, instead of just 90-degree increments
        
        predict = theano.function(inputs=[predim], outputs=givenCNN.allLayers[-1].p_y_given_x, givens={givenCNN.xx: predim})
        allpredictions = []
        deltaAngle = 90.0/float(anglesInbetween+1)
        
        for aidx in range(anglesInbetween+1):
            
            if aidx > 0:
                imrotated = cv2rotateImg(numpy.reshape(imdata, (widthOfImage,widthOfImage)), float(aidx)*deltaAngle)
                imrotated = numpy.reshape(imrotated, (1, widthOfImage*widthOfImage))
            else:
                imrotated = imdata
            
            allpredictions = numpy.append(allpredictions, predict(imrotated))
            
            if debuggingMode:
                cv2.imshow("imrotated"+str(aidx), cv2.resize(numpy.reshape(imrotated, (widthOfImage,widthOfImage))/255., (200,200)))
        
        topidxs = numpy.argpartition(allpredictions, -numTopGuessesToReturn)[-numTopGuessesToReturn:]
        topidxs = topidxs[numpy.argsort(allpredictions[topidxs])]
        #now a list of "numTopGuessesToReturn" indices sorted in ascending order of confidence
        
        if includesJunkChar:
            angoffsets = (topidxs / 145)
            #classes = (topidxs - (angoffsets*145))
            
            chars = []
            orientations = []
            confidences = []
            for cidx in range(len(topidxs)):
                if float(allpredictions[topidxs[cidx]]) >= junkConfThreshold:
                    if (topidxs[cidx]+1) % 145 == 0:
                        chars.append('#')
                        orientations.append(0.0)
                    else:
                        charchar = (topidxs[cidx] % 145)
                        if charchar == 144:
                            #should have been handled by the above if case
                            print("ERROR????????????????????????????????????????????????????????????????? FIXME")
                        characteridx = (charchar%36)
                        if characteridx < 10:
                            chars.append(chr(characteridx+48))
                        else:
                            chars.append(chr(characteridx+55))
                        thisorientation = float(ConvertOrientationClassToDegrees(charchar)) - deltaAngle*float(angoffsets[cidx])
                        if thisorientation < 0.0:
                            orientations.append(thisorientation+360.0)
                        else:
                            orientations.append(thisorientation)
                else:
                    chars.append('#')
                    orientations.append(0.0)
                confidences.append(float(allpredictions[topidxs[cidx]]))
            
            if debuggingMode:
                print("topidxs numeric == "+str(topidxs))
                #print("chars numeric == "+str(classes))
                print("chars == "+str(chars))
                print("orientations == "+str(orientations))
                print("confidences == "+str(confidences))
        
            if returnDetailedInfo:
                return (chars, orientations, confidences)
            else:
                return (chars[-1], orientations[-1])
        else:
            angoffsets = (topidxs / 144)
            #classes = (topidxs - (angoffsets*144))
        
            chars = []
            orientations = []
            confidences = []
            for cidx in range(len(topidxs)):
                if float(allpredictions[topidxs[cidx]]) >= junkConfThreshold:
                    characteridx = (topidxs[cidx]%36)
                    if characteridx < 10:
                        chars.append(chr(characteridx+48))
                    else:
                        chars.append(chr(characteridx+55))
                    thisorientation = float(ConvertOrientationClassToDegrees(topidxs[cidx]%144)) - deltaAngle*float(angoffsets[cidx])
                    if thisorientation < 0.0:
                        orientations.append(thisorientation+360.0)
                    else:
                        orientations.append(thisorientation)
                else:
                    chars.append('#')
                    orientations.append(0.0)
                confidences.append(float(allpredictions[topidxs[cidx]]))
        
            if debuggingMode:
                print("topidxs numeric == "+str(topidxs))
                #print("chars numeric == "+str(classes))
                print("chars == "+str(chars))
                print("orientations == "+str(orientations))
                print("confidences == "+str(confidences))
        
            if returnDetailedInfo:
                return (chars, orientations, confidences)
            else:
                return (chars[-1], orientations[-1])


def test_saved_net_on_image_in_memory(img, trainedWeightsFile, widthOfImages, anglesInbetween, returnDetailedInfo, numTopGuessesToReturn):
    
    # construct CNN
    builtCNN = OCR_CNN(batch_size=1, useDropout=False, params=defaultCNNParams())
    # load params layer-by-layer
    for nnlayeridx in range(len(builtCNN.allLayers)):
        builtCNN.allLayers[nnlayeridx].loadParams(trainedWeightsFile+".l"+str(nnlayeridx))
    
    return predict_CNN_on_img(builtCNN, img, widthOfImages, anglesInbetween=anglesInbetween, includesJunkChar=(builtCNN.params.numOutClasses==145), numTopGuessesToReturn = numTopGuessesToReturn, returnDetailedInfo = (numTopGuessesToReturn > 1))
    #return predict_CNN_on_img(builtCNN, img, widthOfImages, anglesInbetween=anglesInbetween, returnDetailedInfo=returnDetailedInfo, includesJunkChar=(builtCNN.params.numOutClasses==145), numTopGuessesToReturn=numTopGuessesToReturn)


def GetMeanMedianOfList(thelist):
    themean = 0.0
    for fl in thelist:
        themean = themean + float(fl)
    themean = themean / float(len(thelist))
    sortedlist = sorted(thelist)
    themedian = 0.0
    if len(thelist) % 2 == 1:
        themedian = sortedlist[len(sortedlist)/2]
    else:
        themedian = (sortedlist[len(sortedlist)/2] + sortedlist[(len(sortedlist)/2)-1]) * 0.5
    return (themean, themedian)


def test_saved_lenet5_on_image_file_or_folder(testImageFile, fileIsActuallyFolder, wasGivenPrebuiltCNN, trainedWeightsFile="", builtCNN="", truthIsFirstCharacter=False):
    
    if wasGivenPrebuiltCNN == False:
        # construct CNN
        builtCNN = OCR_CNN(batch_size=1, useDropout=False, params=defaultCNNParams())
        # load params layer-by-layer
        for nnlayeridx in range(len(builtCNN.allLayers)):
            builtCNN.allLayers[nnlayeridx].loadParams(trainedWeightsFile+".l"+str(nnlayeridx))
    
    images = []
    if fileIsActuallyFolder == False:
        images = [testImageFile]
    else:
        for (dirpath, dirnames, filenames) in os.walk(testImageFile):
            for filename in filenames:
                #print("found image \""+str(filename)+"\" for testing")
                images.append(filename)
    if len(images) == 0:
        print("Error: no images found in that folder! Quitting...")
        return
    
    topNumNchars = 2
    confsOfTopWrong = []
    confsOfCorrect = []
    numcorrectChars = 0
    numseenChars = 0
    numcorrectJunk = 0
    numseenJunk = 0
    desiredWidth = defaultCNNParams().widthOfImages
    for imagename in images:
        # load and convert test image
        from PIL import Image
        if fileIsActuallyFolder:
            im = Image.open(testImageFile+"/"+imagename)
        else:
            im = Image.open(testImageFile)
        #print("original size of image: "+str(im.size[0])+" columns, "+str(im.size[1])+" rows")
        im = im.convert("L")
        if im.size[0] != desiredWidth or im.size[1] != desiredWidth:
            im.thumbnail((desiredWidth,desiredWidth), Image.ANTIALIAS)
        
        npim = numpy.asarray(im)
        #im.close()
        
        numTopGuessesToReturn = 100
        
        prediction = predict_CNN_on_img(builtCNN, npim, widthOfImage=desiredWidth, debuggingMode=False, anglesInbetween = 5, includesJunkChar=(builtCNN.params.numOutClasses==145), numTopGuessesToReturn = numTopGuessesToReturn, returnDetailedInfo = (numTopGuessesToReturn > 1))
        
        if truthIsFirstCharacter:
            if imagename[0] == '#':
                numseenJunk += 1
            else:
                numseenChars += 1
        
        if numTopGuessesToReturn == 1:
            if truthIsFirstCharacter:
                truthchar = imagename[0]
                if truthchar == prediction[0] or prediction[0] == '0' and truthchar == 'O' or prediction[0] == 'O' and truthchar == '0':
                    print("prediction on "+str(imagename)+" was \'"+str(prediction[0])+"\' with orientation \""+str(prediction[1])+"\"")
                    if truthchar == '#':
                        numcorrectJunk += 1
                    else:
                        numcorrectChars += 1
                else:
                    print("prediction on "+str(imagename)+" was \'"+str(prediction[0])+"\' with orientation \""+str(prediction[1])+"\"     ----- misclassified!")
            else:
                print("prediction on "+str(imagename)+" was \'"+str(prediction[0])+"\' with orientation \""+str(prediction[1])+"\"")
        else:
            thisCharFound = False
            lastUniqueCharGuess = ''
            topNchars = []
            topNconfs = []
            immediatelyStopIfNumberOneIsJunk = True
            for gidx in range(len(prediction[0])):
                trueidx = (len(prediction[0]) - gidx - 1) #confidences are ascending order, so read in reverse
                if truthIsFirstCharacter:
                    truthchar = imagename[0]
                    
                    thisCharIsCorrectRightNow = False
                    if truthchar == prediction[0][trueidx] or prediction[0][trueidx] == '0' and truthchar == 'O' or prediction[0][trueidx] == 'O' and truthchar == '0':
                        thisCharIsCorrectRightNow = True
                    if thisCharIsCorrectRightNow == False:
                        confsOfTopWrong.append(prediction[2][trueidx])
                    
                    if prediction[0][trueidx] != lastUniqueCharGuess:
                        lastUniqueCharGuess = prediction[0][trueidx]
                        topNchars.append(lastUniqueCharGuess)
                        topNconfs.append(prediction[2][trueidx])
                        if immediatelyStopIfNumberOneIsJunk and '#' in topNchars:
                            while len(topNchars) < topNumNchars:
                                topNchars.append('#')
                                topNconfs.append(topNconfs[0])
                    if len(topNchars) <= topNumNchars:
                        if truthchar == lastUniqueCharGuess or lastUniqueCharGuess == '0' and truthchar == 'O' or lastUniqueCharGuess == 'O' and truthchar == '0':
                            if thisCharFound == False:
                                print("prediction on "+str(imagename)+" was \'"+str(lastUniqueCharGuess)+"\' with orientation \""+str(prediction[1][trueidx])+"\"... top N chars == "+str(topNchars[:topNumNchars])+", conf "+str(topNconfs[:topNumNchars]))
                                if truthchar == '#':
                                    numcorrectJunk += 1
                                else:
                                    numcorrectChars += 1
                                thisCharFound = True
                                confsOfCorrect.append(topNconfs[-1])
                    else:
                        if thisCharFound == False:
                            print("top prediction on "+str(imagename)+" was \'"+str(prediction[0][-1])+"\' with orientation \""+str(prediction[1][-1])+"\"... top N chars == "+str(topNchars[:topNumNchars])+", conf "+str(topNconfs[:topNumNchars])+"     ----- but it was misclassified!")                        
                        trueidx = len(prediction[0]) + 1000
                        break
                else:
                    print("TRUTH WASNT FIRST CHAR? prediction on "+str(imagename)+" was \'"+str(prediction[0])+"\' with orientation \""+str(prediction[1])+"\"")
    
    print(" ")
    if numseenChars > 0:
        print("chars top-"+str(topNumNchars)+" results: correct ratio == "+str(numcorrectChars)+"/"+str(numseenChars)+" == "+str((float(numcorrectChars)/float(numseenChars))))
    else:
        print("no seen characters!")
    if numseenJunk > 0:
        print("junk  top-"+str(topNumNchars)+" results: correct ratio == "+str(numcorrectJunk)+"/"+str(numseenJunk)+" == "+str((float(numcorrectJunk)/float(numseenJunk))))
    if numseenJunk > 0 or numseenChars > 0:
        print("total top-"+str(topNumNchars)+" results: correct ratio == "+str(numcorrectJunk+numcorrectChars)+"/"+str(numseenJunk+numseenChars)+" == "+str((float(numcorrectJunk+numcorrectChars)/float(numseenJunk+numseenChars))))
    print(" ")
    print(" ")
    
    (confsofcorrectMean, confsofcorrectMedian) = GetMeanMedianOfList(confsOfCorrect)
    (confsoftopwrongMean, confsoftopwrongMedian) = GetMeanMedianOfList(confsOfTopWrong)
    
    print("confsofcorrectMean == "+str(confsofcorrectMean)+", confsofcorrectMedian == "+str(confsofcorrectMedian))
    print("confsoftopwrongMean == "+str(confsoftopwrongMean)+", confsoftopwrongMedian == "+str(confsoftopwrongMedian))
    print("confsOfCorrect: "+str(sorted(confsOfCorrect)))
    print("confsOfTopWrong: "+str(sorted(confsOfTopWrong)))


def doTrainConvNetGivenSomeParams(learningrate, momentum, l0nfilt, l0fsize, l0fpool, l1nfilt, l1fsize, l1fpool, l2nfilt, l2fsize, l2fpool, l3nfilt, l3fsize, l3fpool, fc1nn, fc2nn):
    import DatasetsLoaders
    numconvlayers = 4
    numfclayers = 2
    if l3nfilt is None or l3fsize is None or l3fpool is None or l3nfilt < 1 or l3fsize < 1 or l3fpool < 1:
        l3fsize = None
        l3nfilt = None
        l3fpool = None
        numconvlayers = 3
    if l2nfilt is None or l2fsize is None or l2fpool is None or l2nfilt < 1 or l2fsize < 1 or l2fpool < 1:
        l2fsize = None
        l2nfilt = None
        l2fpool = None
        numconvlayers = 2
    if l1nfilt is None or l1fsize is None or l1fpool is None or l1nfilt < 1 or l1fsize < 1 or l1fpool < 1:
        l1fsize = None
        l1nfilt = None
        l1fpool = None
        numconvlayers = 1
    if l0nfilt is None or l0fsize is None or l0fpool is None or l0nfilt < 1 or l0fsize < 1 or l0fpool < 1:
        l0fsize = None
        l0nfilt = None
        l0fpool = None
        numconvlayers = 0
    if fc2nn < 1:
        fc2nn = None
        numfclayers = 1
    if fc1nn < 1:
        fc1nn = None
        numfclayers = 0
    
    nkerns = (l0nfilt, l1nfilt, l2nfilt, l3nfilt)
    fsizes = (l0fsize, l1fsize, l2fsize, l3fsize)
    psizes = (l0fpool, l1fpool, l2fpool, l3fpool)
    hiddensizes = (fc1nn, fc2nn)
    
    params = myCNNForOrientationParamsSTUBTEMPLATE()
    
    # build conv layer(s)
    params.nkerns = [nkerns[ii] for ii in range(numconvlayers)]
    params.filtersizes = [fsizes[ii] for ii in range(numconvlayers)]
    params.poolsizes = [psizes[ii] for ii in range(numconvlayers)]
    params.dropoutrates_conv = [0.1 for ii in range(numconvlayers)]
    params.noiserates_conv = [0. for ii in range(numconvlayers)]
    if numconvlayers > 0:
        params.noiserates_conv[0] = 0.2
    
    # build fc layer(s)
    params.hiddenlayers = [hiddensizes[ii] for ii in range(numfclayers)]
    params.dropoutrates_fullyconn = [0.33 for ii in range(numfclayers)]
    
    # setup and start training
    datasets = DatasetsLoaders.load_chars74k(params.widthOfImages)
    datasetsOnDevice = True
    n_epochs = 12
    pickleNetworkName = None
    filterFilesSavedBaseName = None
    
    try:
        myCNN = OCR_CNN(batch_size=params.batchSize, useDropout=True, params=params)
    except:
        print("exception while building OCR_CNN()")
        return
    
    #if False: #len(pretrainedWeightsFile) > 1:
    #    print("loading pretrained weights!")
    #    # load params layer-by-layer
    #    for nnlayeridx in range(len(myCNN.allLayers)):
    #        myCNN.allLayers[nnlayeridx].loadParams(pretrainedWeightsFile+".l"+str(nnlayeridx))
    
    myCNN.Train(datasets, datasetsOnDevice=datasetsOnDevice, learning_rate=learningrate, weightmomentum=momentum, n_epochs=n_epochs, pickleNetworkName=pickleNetworkName, filterFilesSavedBaseName=filterFilesSavedBaseName, deviceTrainSetSize=params.deviceTrainSetSize, deviceValidSetSize=params.deviceValidSetSize)



if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("args:  {train|retrain|test|testfull}  {test-savedweights}  {test-imagefile}")
        quit()
    if str(sys.argv[1]) == "train":
        if len(sys.argv) < 4:
            print("args:  {train|retrain|test|testfull}  {learning-rate}  {momentum}")
            quit()
        train_lenet5_with_batches(useDropout=True, learning_rate=float(sys.argv[2]), weightmomentum=float(sys.argv[3]))
    elif str(sys.argv[1]) == "train2":
        if len(sys.argv) < 18:
            print("args:  {train|retrain|test|testfull}  {learning-rate}  {momentum}  {l0nfilt, l0fsize, l0fpool, l1nfilt, l1fsize, l1fpool, l2nfilt, l2fsize, l2fpool, l3nfilt, l3fsize, l3fpool, fc1nn, fc2nn}")
            quit()
        print("note: if l3nfilt <= 0, then the 4th convolutional layer will not exist")
        doTrainConvNetGivenSomeParams(float(sys.argv[2]), float(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]), int(sys.argv[6]), int(sys.argv[7]), int(sys.argv[8]), int(sys.argv[9]), int(sys.argv[10]), int(sys.argv[11]), int(sys.argv[12]), int(sys.argv[13]), int(sys.argv[14]), int(sys.argv[15]), int(sys.argv[16]), int(sys.argv[17]))
    elif str(sys.argv[1]) == "retrain":
        if len(sys.argv) < 5:
            print("args:  {train|retrain|test|testfull}  {test-savedweights}  {learning-rate}  {momentum}")
            quit()
        train_lenet5_with_batches(useDropout=True, pretrainedWeightsFile=str(sys.argv[2]), learning_rate=float(sys.argv[3]), weightmomentum=float(sys.argv[4]))
    elif str(sys.argv[1]) == "testfull":
        if len(sys.argv) < 3:
            print("args:  {train|retrain|test|testfull}  {test-savedweights}")
            quit()
        test_saved_lenet5_on_full_dataset(False, trainedWeightsFile=str(sys.argv[2]), batchSize=400)
    elif str(sys.argv[1]) == "test":
        if len(sys.argv) < 6:
            print("args:  {train|retrain|test|testfull}  {test-savedweights}  {test-imagefile}  {file-is-actually-folder}  {truth-is-first-char}")
            quit()
        test_saved_lenet5_on_image_file_or_folder(str(sys.argv[3]), int(sys.argv[4])!=0, False, trainedWeightsFile=str(sys.argv[2]), truthIsFirstCharacter=(int(sys.argv[5])!=0))
    else:
        print("unknown option \""+str(sys.argv[1])+"\"")


