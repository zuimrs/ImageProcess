import cPickle
import numpy as np
import gzip
from sklearn import svm
import pickle
import cv2

def load_data():
  mnist = gzip.open('./dataset/mnist.pkl.gz', 'rb')
  training_data, classification_data, test_data = cPickle.load(mnist)
  mnist.close()
  return (training_data, classification_data, test_data)

def wrap_data():
  tr_d, va_d, te_d = load_data()
  training_inputs = [np.reshape(x, (784, 1)) for x in tr_d[0]]
  training_data = zip(training_inputs, tr_d[1])
  validation_inputs = [np.reshape(x, (784, 1)) for x in va_d[0]]
  validation_data = zip(validation_inputs, va_d[1])
  test_inputs = [np.reshape(x, (784, 1)) for x in te_d[0]]
  test_data = zip(test_inputs, te_d[1])
  return (training_data, validation_data, test_data)


def create_svm():
  return svm.SVC(gamma=0.001)

def train(clf, samples = 50000):
  tr, _, _ = wrap_data()
  X = []
  y  =[]
  counter = 0
  for img in tr:
    if (counter > samples):
      break
    counter += 1
    data, label = img
    X.append(data.ravel().tolist())
    y.append(label)
  print "train set: %d " %(len(X))
  clf.fit(X,y)
  return clf

def validate(clf,samples = 10000):
  _,test,_ = wrap_data()
  X = []
  y  =[]
  counter = 0
  for img in test:
    if (counter > samples):
      break
    counter += 1
    data, label = img
    X.append(data.ravel().tolist())
    y.append(label)
  print "validate set: %d " %(len(X))
  return clf.score(X,y)

def test(clf,samples = 10000):
  _,_,test = wrap_data()
  X = []
  y  =[]
  counter = 0
  for img in test:
    if (counter > samples):
      break
    counter += 1
    data, label = img
    X.append(data.ravel().tolist())
    y.append(label)
  return clf.score(X,y)
def predict(clf, sample):
  resized = sample.copy()
  rows, cols = resized.shape
  if (rows != 28 or cols != 28) and rows * cols > 0:
    resized = cv2.resize(resized, (28, 28), interpolation = cv2.INTER_LINEAR)
  X = []
  X.append(resized.ravel().tolist())
  X = [[each*1.0/255 for each in X[0]]]
  return clf.predict(X)

def save():
  print "create svm clf..."
  clf = create_svm()
  print "train svm clf..."
  clf= train(create_svm(),50000)
  print "test svm clf..."
  print "score:%lf" %(validate(clf))
  print "save svm clf..."
  with open('svm.model', 'wb') as f:
    pickle.dump(clf, f)
if __name__ == '__main__':
	save()
