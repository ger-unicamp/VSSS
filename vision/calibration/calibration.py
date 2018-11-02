import numpy as np
import cv2
import json

#       --> read JSON file "data" <--
try:
    with open('data.json') as f:
        data = json.load(f)
    color_list = []
    p = data['points']
    for i in range(4):
        p[i] = (p[i]['x'], p[i]['y'])
    for color in (data['colors']):
        bgr = (data['colors'][color]['B'], data['colors'][color]['G'], data['colors'][color]['R'])
        color_list.append([color, bgr])
    K = data['K']

except:
    print("JSON file doesn't exist yet")
    color_list = []
    p = []
    for i in range(4):
        ponto = ()
        p.append(ponto)
    #set points as corners of the cam img
    p[0] = (89, 2)
    p[1] = (597, 30)
    p[2] = (567, 480)
    p[3] = (55, 455)
    K = 1

#       --> border calibration <--
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

argv1 = input() #camera number

cap = cv2.VideoCapture(int(argv1))

if not cap.isOpened():
    print("Can't open the video cam")
    quit()

dWidth = int(cap.get(3)) #get the width of frames of the video
dHeight = int(cap.get(4)) #get the height of frames of the video
print("Frame size:", dWidth, "x", dHeight) #print image size

cv2.namedWindow("Control", cv2.WINDOW_AUTOSIZE)

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

cv2.destroyAllWindows()


#       --> color calibration <--
img = transformed_frame
Z = img.reshape((-1,3))

# convert to np.float32
Z = np.float32(Z)

# define criteria, number of clusters(K) and apply kmeans()
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 1000, 1e-4)

#Window of the trackbars "K" and "switch"
cv2.namedWindow("Number of clusters", cv2.WINDOW_AUTOSIZE)

#callback function for the trackbars
def nothing(x):
    pass

#trackbar to set the number of clusters
cv2.createTrackbar("K", "Number of clusters", 1, 30, nothing)
cv2.setTrackbarPos("K", "Number of clusters", K)

# create switch for ON/OFF functionality
cv2.createTrackbar("switch", 'Number of clusters', 0, 1, nothing)

while(True):
    K = cv2.getTrackbarPos('K','Number of clusters')
    s = cv2.getTrackbarPos('switch','Number of clusters')
    if s == 1:
        ret, label, center = cv2.kmeans(Z, K, None, criteria, 10, cv2.KMEANS_RANDOM_CENTERS)
        # Now convert back into uint8, and make original image
        center = np.uint8(center)
        res = center[label.flatten()]
        res2 = res.reshape((img.shape))

        #create numbered color rectangles of each cluster
        retangulos = np.zeros((200, 1200, 3), np.uint8)
        rect_size = 1200 // K
        for i in range(K):
            color_rect = tuple([int(x) for x in center[i]])
            cv2.rectangle(retangulos, (i*rect_size, 0), ((i+1)*rect_size, 150), color_rect, thickness=-1)
            cv2.putText(retangulos, str(i), (i*rect_size + rect_size//2 - 15, 185), cv2.FONT_HERSHEY_SIMPLEX, 1, (255, 255, 255), thickness=3)

        #display original image, clustered image and color rectangles
        cv2.imshow('img',img)
        cv2.imshow('img_clusterizada',res2)
        cv2.imshow('cores', retangulos)
        cv2.waitKey(0)

    elif s == 0:
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

recognizable_color_names = ['blue', 'yellow', 'orange', 'pink', 'green', 'purple', 'red', 'brown', '']
for i in range(K):
    color_name = input('Cluster {}: '.format(i))
    while color_name not in recognizable_color_names:
        print("Unrecognizable color.")
        color_name = input('Cluster {}: '.format(i))
    color_list.append([color_name, tuple([int(x) for x in center[i]]), i])
    print(color_list)
label = list(label.flatten())

max_color_values = []
min_color_values = []
for i in range(K):
    x = [Z[j] for j in range(len(label)) if label[j] == i]
    x = np.array(x)
    max_color_values.append((np.max(x[:,0]), np.max(x[:,1]), np.max(x[:,2])))
    min_color_values.append((np.min(x[:,0]), np.min(x[:,1]), np.min(x[:,2])))


#       --> write to JSON file "data" <--
data = {}
color_dict = {}
for color in color_list:
    ind = color[2]
    color[1] = {'B_max': int(max_color_values[ind][0]), 'B_min': int(min_color_values[ind][0]),
                'G_max': int(max_color_values[ind][1]), 'G_min': int(min_color_values[ind][1]),
                'R_max': int(max_color_values[ind][2]), 'R_min': int(min_color_values[ind][2])}
    color_dict[color[0]] = color[1]
for i in range(4):
    p[i] = {'x': p[i][0], 'y': p[i][1]}
data['colors'] = color_dict
data['points'] = p
data['K'] = K
if '' in data['colors']:
    data['colors'].pop('')

with open('data.json', 'w') as colors_file:
        json.dump(data, colors_file, indent=True, ensure_ascii=False)
