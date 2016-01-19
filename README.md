# Multi-touch-cloud
Multi-touch application using Kinect

To run this code you need :
1) Kinect
2) the Openframeworks library (http://openframeworks.cc/) to handle the Kinect
3)the OpenCV library (http://opencv.org/) to handle the video (blob detection, ...) 


What is a blob? It is a group of points which has the same properties. 
In this code the property we used to create blobs is the depth. 
In fact, as you can see in the Youtube demonstration video (https://www.youtube.com/watch?v=-FTTZp7G9fs) , 
we applied a filter on the video recorded by the Kinect to keep the points 
which are about 2m away from the camera. Actually, you can adjust (thanks to directional keys (up and down) ) this distance (that is what we do in the video, we adjust the distance to only keep the points we need) 
Nevertheless, the Kinect is not very accurate when the distance is too high. That's why a distance of 
2,5-3m is the maximum if you don't want to have problems in the detection of your finger


You also have to be careful : the Kinect must be in front of the screen you use. If you want
to move the Kinect, you'll have to calculate a matrix that will allow you to process the video 
as if the camera was facing the screen

Finally, you can also use the PCL library to run such an application (http://pointclouds.org/). That is what we first try to use but this library requires a very powerful computer : we determined the equation of the plan in which the screen was on each frame.
Because of the weight of this operation, we had to give up this solution. 

Don't hesitate to contact me if you want more informations : sebastien.demarbre@gmail.com
