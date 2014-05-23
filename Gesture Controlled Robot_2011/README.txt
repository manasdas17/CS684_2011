Group 16: Gesture Controlled Robot Project
----------------------------------------------

Team:
-------
+  Hitesh Kewalramani
+  Subhasish Saha
+  Ashok Rawat
+  Sudheer Katta

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
REQUIREMENTS
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
1. Kinect sensor device
2. Machine with dual core processor
3. The Kinect SDK is restricted to work on MS Windows 7 with DirectX SDK
4. The Kinect Sensor Device requires a good processor and a dedicated VGA card.
5. The distance between the user and the Kinect Interface must be 4 to 11 feet.


++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ON COMPUTER : GESTURE RECOGNITION AND SEND COMMAND THROUGH Zigbee MODULE
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

1. Install Microsoft Visual Studio IDE
2. Install Kinect SDK
3. Folder CS_code contains C# code for gesture recognition
4. Open it with Microsoft Visual Studio IDE
5. CS_code/MainWindow.xaml.cs is the main source file defining the main functionality
	i.   Here on each clap the Bot toggles between start and stop state.
	ii.  To make the Bot right turn, put your right wrist above your head.
	iii. Put the hand as long as you want to make it turn.
	iv.  Smilarly put left hand above head for left turn.
	v.   Take right arm to left of body and then wave it back to right. This
             will be recognnized as right wave. Bot will take a fixed amount
             of right turn.
	vi.  Similar for left wave, Bot will take a fixed amount of left turn.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
ON BOT : RECEIVE COMMANDS THROUGH Zigbee AND EXECUTE
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
1. 13B_Serial_Communication_USB-RS232 contains the C code for receiving
   commands through Zigbee module and run the bot.
2. Open it with AVRStudio IDE and burn it on Bot.
