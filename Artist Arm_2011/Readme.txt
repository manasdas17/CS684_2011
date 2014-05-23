Group 7 : Vector Image extraction and Autonomous Duplication by Artist Arm 
--------------------------------------------------------------------------------

Team 7--------
+  Nimit Kalaria  (10305904)
+  Mayur Mahrotri (10305088)
+ Shripad Sarade (10305051)

Hardware Requirements:
----------------------
1. Dexter ER-2 Heavy Duty Robotics Arm
2. PC

Software Requirements:
----------------------
1. Visual Studio 2008
2. OpenCV2.1

The detailed instructions of how to integrate OpenCV2.1 with Visual Studio 2008 is given at:

http://opencv.willowgarage.com/wiki/VisualC%2B%2B_VS2008

1. The folder Vector contains the Visual C++ project (.sln will run in Visual Studio). The project was made in Visual Studio 2008 (.Net Framework 9).
2. The project needs to be given the command line argument, the image, which the robotic arm will sketch on paper. How the command line argument is given
is explained in the screencast (Screen recording).
3. Given this image as input, the program produces a text file (Armprogram.txt) which contains strings. These strings when given as input to the
Robotics arm GUI will draw the outline of image as points. How the input is burnt onto the Robotic arms is also explained in the screencast.
