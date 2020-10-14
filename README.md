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
We achieve __~95%__ and __~92%__ accuracy on test data for English and Persian handwritten digit detection respectively.
```bash
Initializing
Making a neural network with 1818 connections
Initialized
Training using the dataset for the English language
Reading the dataset..
Done reading the dataset (loaded 70000 samples)..
Training..
Learning rate formula (with domain 0 to 1750000): 0.2*1.00001^(-0.0792172*x)
=============== EPOCH 1/25 ===============

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
0.0117885 0.000424332 0.0826226 0.000127434 0.0420924 0.0827613 0.127265 9.65009e-05 0.0104568 0.00392281
Ground Truth, NN Answer (confidence):   5, 6 (0.127265)
------------ TRAIN ------------
Used: 1800
Loss (MSE): 0.0792671
Accuracy: 40.5556%
------------ TEST ------------
Used: 201
Loss (MSE): 0.0946065
Accuracy: 41.791%
------------------------------
Total samples Seen/Remaining: 2000/1750000
Learning Rate: 0.199683
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
5.55072e-05 2.14186e-05 0.000221989 0.000548984 0.0161675 0.000568514 2.04476e-05 0.0459686 2.8843e-05 0.897582
Ground Truth, NN Answer (confidence):   9, 9 (0.897582)
------------ TRAIN ------------
Used: 1800
Loss (MSE): 0.00508778
Accuracy: 97.4444%
------------ TEST ------------
Used: 200
Loss (MSE): 0.00825342
Accuracy: 96%
------------------------------
Total samples Seen/Remaining: 1746000/1750000
Learning Rate: 0.00504234
------------------------------

Done training. Elapsed training seconds: 18142.4
Testing..
Training data MSE: 0.00527851
Training data accuracy: 97.3254%

Testing data MSE: 0.0073592
Testing data accuracy: 95.7857%
Enter a character to exit.


```
# Notes
This project was originally developed in January 2018 for Persian digits and English digits was added later on in October 2020 along with lzstring compression of the pretrained paramters, significant code structure and naming improvements and more comments.
