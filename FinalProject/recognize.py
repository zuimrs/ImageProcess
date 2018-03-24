import cv2
import numpy as np
import svm
import ann
import pickle

def inside(r1, r2):
  x1,y1,w1,h1 = r1
  x2,y2,w2,h2 = r2
  if (x1 > x2) and (y1 > y2) and (x1+w1 < x2+w2) and (y1+h1 < y2 + h2):
    return True
  else:
    return False

def wrap_digit(rect):
  h = len(rect)
  w = len(rect[0])
  res = None
  if(h > 3*w):
    rect = cv2.resize(rect, (8*10, 20*10), interpolation = cv2.INTER_LINEAR)
    # w:20 h:28
    res = np.zeros((28*10,20*10))
    res[10*10:18*10] = rect.transpose()
    # w:28 h:20
    tmp = res.transpose()
    res = np.zeros((28*10,28*10))
    res[4*10:24*10] = tmp
  elif(h > w):
    rect = cv2.resize(rect, (14*10, 20*10), interpolation = cv2.INTER_LINEAR)
    # w:20 h:28
    res = np.zeros((28*10,20*10))
    res[7*10:21*10] = rect.transpose()
    # w:28 h:20
    tmp = res.transpose()
    res = np.zeros((28*10,28*10))
    res[4*10:24*10] = tmp
  return res

def load_model():
  print "load model..."
  with open('svm.model', 'rb') as f:
    clf = pickle.load(f)
  return clf


# clf = cv2.ml.ANN_MLP_load('net.model')
clf = load_model()

# print "score:%lf" %(svm.test(clf,200))

font = cv2.FONT_HERSHEY_SIMPLEX

for i in range(1,6):
  path = "./result/"+str(i)+".jpg"
  img = cv2.imread(path, cv2.IMREAD_UNCHANGED)
  # img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
  bw = cv2.GaussianBlur(img,(5,5),0)
  thbw = cv2.adaptiveThreshold(im_gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV, 5, 10)
  thbw = cv2.enrode(thbw,np.ones((2,2)),iterations=2)
  for i in range(len(bw)):
    for j in range(len(bw[0])):
      thbw[i,j] = 255- bw[i,j]
  thbw = cv2.erode(thbw,np.ones((2,2),np.uint8),iterations = 2)
  image, cntrs, hier = cv2.findContours(thbw.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

  rectangles = []

  for c in cntrs:
    r = x,y,w,h = cv2.boundingRect(c)
    a = cv2.contourArea(c)
    b = (img.shape[0]-3) * (img.shape[1] - 3)
    is_inside = False
    for q in rectangles:
      if inside(r, q):
        is_inside = True
        break
    if not is_inside:
      if not a >= b:
        rectangles.append(r)
  # dc = 0
  for r in rectangles:
    x,y,w,h = r
    if w < 10:
      continue
    cv2.rectangle(img, (x,y), (x+w, y+h), (0, 0, 0), 2)
    roi = thbw[y:y+h, x:x+w]
    res = -1
    try:
      roi = wrap_digit(roi)
      cv2.imshow('b',roi)
      cv2.waitKey()
      res = int(svm.predict(clf, roi.copy()))
      #res = int(ann.predict(clf, roi.copy()))
      #cv2.imshow('digit',roi)
      cv2.putText(img, "%d" % res, (x, y+ h +30), font, 1, (0, 0, 0))
    except:
      continue
  cv2.imshow("thbw", thbw)
  cv2.imshow("contours", img)
  cv2.waitKey()
