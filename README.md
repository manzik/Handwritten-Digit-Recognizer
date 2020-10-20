# Persian Handwritten Digit Recognizer

A feedforward fully-connected neural network (multilayer perceptron) project trained to detect English and Persian handwritten digits from an image. 

 __Online demo for the neural network's demonstration:__


[http://github.manzik.com/Handwritten-Digit-Recognizer/js-interactive/](http://github.manzik.com/Handwritten-Digit-Recognizer/js-interactive/)

# Method
This repository contains code for computing and optimizing a neural network's output for digits using backpropagation in both C++ and Javascript.   

- Datasets used:
  - [MNIST](http://yann.lecun.com/exdb/mnist/) (English): Training and testing data provided are mixed. The digits are also scaled and moved randomly as MNIST normalizes and centers all of the images.
  - [Hoda](http://farsiocr.ir/%D9%85%D8%AC%D9%85%D9%88%D8%B9%D9%87-%D8%AF%D8%A7%D8%AF%D9%87/%D9%85%D8%AC%D9%85%D9%88%D8%B9%D9%87-%D8%A7%D8%B1%D9%82%D8%A7%D9%85-%D8%AF%D8%B3%D8%AA%D9%86%D9%88%DB%8C%D8%B3-%D9%87%D8%AF%DB%8C/) (Persian): Only testing data is used. The images are variable in size, so they are moved to a random location in a 28*28 grid. 

We then split the data into 90% for the train and 10% for test data.

- Workflow:
 1. Training
    - We load the dataset for the desired language and train a neural network using the C++ part of the project.
    - The C++ program teaches the NN and saves the optimized parameters in a lzstring compressed file containing a JSON array.  
 2. Demonstration:
    - The file for pretrained parameters is then downloaded, decompressed and loaded to the Javascript version of the neural network library.
    - Now we can make predictions using the pretrained parameters based on user's handwritten images drawn in a canvas inside their browser.

# Results
We achieve __~96%__ and __~92%__ accuracy on test data for English and Persian handwritten digit detection respectively.
```bash
Initializing
Making a neural network with 1818 connections
Initialized
Training using the dataset for the English language
Reading the dataset..
Done reading the dataset (loaded 70000 samples)..
Training..
Learning rate formula (with domain 0 to 2100000): 0.3*1.00001^(-0.27161*x)
=============== EPOCH 1/30 ===============

------------ DEBUG ------------
Data type: Testing Data
Input Neurons:
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . @ @ @ @ @ @ @ @ @ . . . . . . . .
. . . . . . . . . . @ @ @ @ @ @ @ @ @ @ @ @ . . . . . .
. . . . . . . . . . @ @ @ @ @ @ @ @ @ @ @ @ . . . . . .
. . . . . . . . . . @ @ @ @ @ @ @ @ @ @ @ @ . . . . . .
. . . . . . . . . . @ @ @ @ . . . . . . . . . . . . . .
. . . . . . . . . @ @ @ @ @ . . . . . . . . . . . . . .
. . . . . . . . @ @ @ @ @ @ . . . . . . . . . . . . . .
. . . . . . . . @ @ @ @ @ . . . . . . . . . . . . . . .
. . . . . . . @ @ @ @ @ . . . . . . . . . . . . . . . .
. . . . . . . @ @ @ @ @ . . . . . . . . . . . . . . . .
. . . . . . . @ @ @ @ @ . . . . . . . . . . . . . . . .
. . . . . . . . @ @ @ @ @ @ @ @ @ @ @ . . . . . . . . .
. . . . . . . . @ @ @ @ @ @ @ @ @ @ @ @ . . . . . . . .
. . . . . . . . @ @ @ @ @ @ @ @ @ @ @ @ @ @ . . . . . .
. . . . . . . . . @ @ @ @ @ @ @ @ @ @ @ @ @ . . . . . .
. . . . . . . . . . . . @ @ . . . . @ @ @ @ . . . . . .
. . . . . . . . . . . . . . . . . . @ @ @ @ . . . . . .
. . . . . . . . . . . . . . . . @ @ @ @ @ @ . . . . . .
. . . . . . . . . . . . . . @ @ @ @ @ @ . . . . . . . .
. . . . . . . . . . . . . . @ @ @ @ @ . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
Output Neurons:
0.0203967 6.18765e-06 0.0142853 9.40693e-05 0.0725114 0.00455414 0.065087 8.93848e-06 0.00106714 0.000182227
Ground Truth, NN Answer (confidence):   5, 4 (0.0725114)
------------ TRAIN ------------
Used: 1800
Loss (MSE): 0.0930594
Accuracy: 31.7778%
------------ TEST ------------
Used: 201
Loss (MSE): 0.122383
Accuracy: 27.8607%
------------------------------
Total samples Seen/Remaining: 2000/2100000
Learning Rate: 0.298375
------------------------------

<...more iterations>

------------ DEBUG ------------
Data type: Testing Data
Input Neurons:
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . . . . . . . . . . . . . . . . . . . .
. . . . . . . . . @ @ @ @ @ @ @ @ @ @ . . . . . . . . .
. . . . . . . . @ @ @ @ @ @ @ @ @ @ @ . . . . . . . . .
. . . . . . . . @ @ @ @ @ @ @ @ @ @ @ @ . . . . . . . .
. . . . . . . . @ @ @ @ @ @ @ @ @ @ @ @ . . . . . . . .
. . . . . . . . @ @ @ . . . . @ @ @ @ @ . . . . . . . .
. . . . . . . . @ @ @ @ @ @ . @ @ @ @ @ . . . . . . . .
. . . . . . . . @ @ @ @ @ @ @ @ @ @ @ @ . . . . . . . .
. . . . . . . . @ @ @ @ @ @ @ @ @ @ @ . . . . . . . . .
. . . . . . . . . . @ @ @ @ @ @ @ @ @ . . . . . . . . .
. . . . . . . . . . . . . . . . @ @ @ . . . . . . . . .
. . . . . . . . . . . . . . . . @ @ @ . . . . . . . . .
. . . . . . . . . . . . . . . . @ @ @ . . . . . . . . .
. . . . . . . . . . . . . . . . @ @ @ . . . . . . . . .
. . . . . . . . . . . . . . . . @ @ @ . . . . . . . . .
. . . . . . . . . . . . . . . . @ @ @ . . . . . . . . .
. . . . . . . . . . . . . . . . @ @ @ @ . . . . . . . .
. . . . . . . . . . . . . . . . @ @ @ @ . . . . . . . .
. . . . . . . . . . . . . . . . @ @ @ @ . . . . . . . .
. . . . . . . . . . . . . . . . @ @ @ @ . . . . . . . .
. . . . . . . . . . . . . . . . . @ @ @ . . . . . . . .
Output Neurons:
1.79785e-07 0.000163668 0.000321924 0.000577478 0.0715933 0.000590554 9.86376e-06 0.0169101 9.24018e-05 0.960976
Ground Truth, NN Answer (confidence):   9, 9 (0.960976)
------------ TRAIN ------------
Used: 1800
Loss (MSE): 0.00471855
Accuracy: 97.2778%
------------ TEST ------------
Used: 200
Loss (MSE): 0.00675072
Accuracy: 96%
------------------------------
Total samples Seen/Remaining: 2096000/2100000
Learning Rate: 0.00101092
------------------------------

Done training. Elapsed training seconds: 22190.9
Testing..
Training data MSE: 0.00464235
Training data accuracy: 97.6381%

Testing data MSE: 0.00707453
Testing data accuracy: 96.0571%
Enter a character to exit.

```
