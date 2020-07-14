import cv2
import sys

imgname = sys.argv[1]
img = cv2.imread(imgname)
ret, img = cv2.threshold(img,127,255,cv2.THRESH_BINARY_INV)
cv2.imwrite(imgname, img)
