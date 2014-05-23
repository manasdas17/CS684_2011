Group 5 :Autonomous attacker / follower robot for moving target
-------------------------------------------------------------------------

TEAM
-------
Viraj Churi        10305072 
Ketan Mav          10305030 
Niket Bagwe        10305086 
Linga Venkatesh    10305085 

Hardware Requirements
-----------------------

1. FireBird : Requires 2 bots (1 Attacker + 1 Enemy) 
2. Camera : To view position of enemy in battlefield
3. Marker : For uniquely identify enemy in battlefield
4. Lazer : Used to fire the enemy
5. Zigbee : To maintain communication between attacker and system
6. Proximity Sensor : To measure the distance between attacker and enemy

Software Requirements
--------------------------

1. OpenCV : For processing images sent by camera
2. Color Segmentation : An algorithm which is both efficient and fast enough to maintain real time segmentation of images sent by camera
3. AVR studio : To program instruction onto a given bot

Instruction
--------------

A. attacker
---------------

	1. Run make file to compile the program.
	2. Make sure that OpenCV 2.0 is installed. (can be installed from package manager)

B. target_controller
-----------------------

	1. Run make file to compile the program.
	2. Install GLUT libraries from package manager before using the program.
	3. Keys A,W,S,D is used to move the robot left, forward, backwards and right respectively. Key P is used to stop the robot. 

