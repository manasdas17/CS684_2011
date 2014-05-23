Group-02 : Movement Control and Obstacle Avoidance of Hexapod 
------------------------------------------------------------------------

Team
------
Meruva Naga Sreenivas     10305079 
Krishna Teja V            10305029 
Nikhil M                  10305032

Prerequisites
--------------------
1. Virtual box with linux os
2. Windows OS
3. AVR Studio
4. ICC AVR
5. cec esterel compiler


Procedure
-------------

1. Open virtual box and start ubuntu
2. Open terminal, go to the src folder and run movement_control.strl for obstacle avoidance functionality or run movement_control_motorwise_main.strl for demonstration of motorwise abstraction with forward and backward motions.
Eg: To run movement_control.strl
		./firebird_gen movement_control.strl
3. This generates a c file movement_control.c
4. Now share this file and firebird_winavr.h to windows. if you cannot share this file you can mail these to your account and download them from windows
5. Now open ICC AVR and create a project and add movement_control.c and firebird_winavr.h files to that project
6. Now go to project-> rebuildall. This generates a hex file with the project name. That is if project name is Main.prj Main.hex is generated.
7. Now open AVR studio and click on connect. Make sure that hexapod is powered on and browse through this hex file and burn it on hexapod.
