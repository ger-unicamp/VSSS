import numpy as np
import cv2

def transform(img, p0, p1, p2, p3):
    inputQuad = np.array([p0, p1, p2, p3],  dtype = "float32")  # array containing the four corners of the field
    outputQuad = np.array([( 0,0 ), # array containing the four corners of the image
                  ( img.shape[1]-1,0),
                  ( img.shape[1]-1,img.shape[0]-1),
                  ( 0,img.shape[0]-1  )], dtype = "float32")
# Get the Perspective Transform Matrix i.e. lambda
    lbd = cv2.getPerspectiveTransform(inputQuad, outputQuad)

    #Apply the Perspective Transform just found to the src image
    output = cv2.warpPerspective(img,lbd,(dWidth, dHeight))

    return output


argv1 = input()

cap = cv2.VideoCapture(int(argv1))

if not cap.isOpened():
    print("Can't open the video cam")
    quit()

dWidth = int(cap.get(3)) #get the width of frames of the video
dHeight = int(cap.get(4)) #get the height of frames of the video
print("Frame size:", dWidth, "x", dHeight) #print image size

cv2.namedWindow("Control", cv2.WINDOW_AUTOSIZE)

p = []
for i in range(4):
    ponto = ()
    p.append(ponto)

p[0] = (89, 2)
p[1] = (597, 30)
p[2] = (567, 480)
p[3] = (55, 455)

#P0 is the superior left corner
#P1, P2 and P3 continue clockwise


def callback0(val):
    p[0] = (val, p[0][1])
def callback1(val):
    p[0] = (p[0][0], val)
def callback2(val):
    p[1] = (val, p[1][1])
def callback3(val):
    p[1] = (p[1][0], val)
def callback4(val):
    p[2] = (val, p[2][1])
def callback5(val):
    p[2] = (p[2][0], val)
def callback6(val):
    p[3] = (val, p[3][1])
def callback7(val):
    p[3] = (p[3][0], val)

cv2.createTrackbar("P0-X", "Control", p[0][0], dWidth, callback0)
cv2.createTrackbar("P0-Y", "Control", p[0][1], dHeight, callback1)
cv2.createTrackbar("P1-X", "Control", p[1][0], dWidth, callback2)
cv2.createTrackbar("P1-Y", "Control", p[1][1], dHeight, callback3)
cv2.createTrackbar("P2-X", "Control", p[2][0], dWidth, callback4)
cv2.createTrackbar("P2-Y", "Control", p[2][1], dHeight, callback5)
cv2.createTrackbar("P3-X", "Control", p[3][0], dWidth, callback6)
cv2.createTrackbar("P3-Y", "Control", p[3][1], dHeight, callback7)

#Optimal values
#P0 : (111,47) P1 : (566,10) P2 : (579, 415) P3 : (135, 421)


while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()
    transformed_frame = transform(frame, p[0], p[1], p[2], p[3])

    cv2.circle(frame, p[0], 5, (255, 0, 0), -1);
    cv2.circle(frame, p[1], 5, (0, 255, 0), -1);
    cv2.circle(frame, p[2], 5, (0, 0, 255), -1);
    cv2.circle(frame, p[3], 5, (255, 255, 255), -1);

    # Our operations on the frame come here
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Display the resulting frame
    cv2.imshow('MyVideo_Original', frame)
    cv2.imshow("MyVideo_Transformed", transformed_frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()
