Group 10: RCTB Error Detection Project
-----------------------------------------

Team 10:
---------
* Deepak Jayanth P M (10305913) 
* Pratik Patodi      (10305917) 
* Firuza Aibara      (p11119) 
* Alluri Srinivas    (10305076)

Hardware Requirements
-----------------------
• One Windows based system with 4 USB ports
• Three USB Camera 
• Three Zigbee Modules
• Two Spark V robots

Software Requirements
-----------------------
• Scilab 5.3.3 (Windows)
• Scilab Image and Video Processing Toolbox(SIVP) 
• Serial communication 
• AVR Studio 4 
• X-CTU software to con?gure ZigBee modules

Follow these steps in sequence to succesfully execute the project
----------------------------------------------------------------------


1. Prepare  the mother bot and shooting bot.



2. Open AVR studio4 and burn a code for serial communication in both the spark V bots with which
   it will move.



3. Then configure three zigbees so that they can communicate with each other(we are broadcastiong
   the commands,
and in serial communication code we have already specify the address of the bots,
   so they will recive the commands and execute it if it was intended for it).



4. Fix two zigbees, one on target bot and the other no shooting bot and connect one zigbee to the
   laptop using the USB cable.



5. Chekc whether the both the bots are moving correctly using the X-CTU software. If yes then
   proceed ahead else check your zigbee.



6. Connect two USB cameras to the laptop and mount it, one on mother bot and the other on shooting
   bot.



7. Now palce the mother bot with a red colored object place on top of it and shooting bot with a
   green colored object on top of it, on one side on field 
and the blue color object(which we are
   using as target bot) & pink color object (which we are using as splash) on the other side of
   the field.



8. Now fix the top camera so that it can see all the three colored object that is the red colored
   oject, the green colored object as wel as the blue color object.



9. Check whether camera is working. If yes proceed ahead else check your camera connection.



10. Now switch on both the bots.



11. Now start the scilab on the laptop.



12. Load the SIVP module from the Toolboxes menu, if not automatically loaded while starting.



13. Using the file menu of scilab open the file rctb_team10.sce.



14. Now execute  this file in scilab using the play button on the toolbar.



15. if you get an error for cannot open camera, check your camera connection.



16. If you get an error for serial port then check that you are connecting to the same port the
    zigbee is connected.



17. or else you will see that both the bots are moving so that they can alline themselfs to the
    target bot and you can see the combine frame of all the three camera perspetiove on you
    laptop screen.
