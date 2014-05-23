/* 
*               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                    
*                                  PROJECT TITLE  :     ERROR DETECTION IN REMOTE CONTROL TARGET BOT (RCTB)
*               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
*   @uthors  - Deepak Jayanth
*            - Prathik Patodi
*	     - Firuza Aibara
*	     - Srinivas Alluri	
*	
*	Copyright (c) 2010, ERTS Lab IIT Bombay erts@cse.iitb.ac.in               
*   	All rights reserved.
*
*	Redistribution and use in source and binary forms, with or without
*   	modification, are permitted provided that the following conditions are met:
*
*  	---- Redistributions of source code must retain the above copyright
*     	     notice, this list of conditions and the following disclaimer.
*
*   	---- Redistributions in binary form must reproduce the above copyright
*     	     notice, this list of conditions and the following disclaimer in
*     	     the documentation and/or other materials provided with the
*    	     distribution.
*
*   	---- Neither the name of the copyright holders nor the names of
*    	     contributors may be used to endorse or promote products derived
*   	     from this software without specific prior written permission.
*
*   	---- Source code can be used for academic purpose. 
*	 
*	---- For commercial use permission form the author needs to be taken.
*
*	  	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
*	  	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*	  	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*	  	ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
*	  	LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*	  	CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*	  	SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*	  	INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*	  	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*	  	ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*	  	POSSIBILITY OF SUCH DAMAGE. 
*
*	  	Software released under Creative Commence cc by-nc-sa licence.
*	  	For legal information refer to: 
*	  	http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
*
*
*/

				/********     FUNCTION TO CALCULATE THE ERROR BETWEEN TARGET AND SPLASH     *********/


function err = func(motX,motY,targX,targY,shootX,shootY,the1,the2)
mother=[motX;motY];							         // gives the Mother-bot position (co-ordinates)

target=[targX;targY];						                 // gives the Target-bot position (co-ordinates)

shoting=[shootX;shootY];					                 // gives the Shooting-bot position (co-ordinates)

theta1=the1*%pi/180;						                 // calculates the angle to rotate for Mother-bot. 

theta2=the2*%pi/180;						                 // calculates the angle to rotate for Shooting-bot.

splash1=rotate(target,theta1,mother);                                            // rotates and aligns the mother-bot with target-bot.

splash2=rotate(target,theta2,shoting);                                           //  rotates and aligns the shooting-bot with target-bot.

a11=(mother(2)-splash1(2))/(mother(1)-splash1(1)+1);                             // calculates the slope (y2-y1)/(x2-x1).
a12=-1;
a21=(shoting(2)-splash2(2))/(shoting(1)-splash2(1)+1);				 // calculates the slope (y2-y1)/(x2-x1).
a22=-1;
if (shoting(1) ~= splash2(1) & mother(1) ~= splash1(1)) then,
    
c1=(mother(2)*splash1(1)-splash1(2)*mother(1))/(mother(1)-splash1(1));           //  ((x1*y2)-(y1*x2))/(x2-x1).
c2=(shoting(2)*splash2(1)-splash2(2)*shoting(1))/(shoting(1)-splash2(1));
A=[a11,a12;a21,a22]
C=[c1;c2]
B=A\C;
x=target(1)-B(1);
y=target(2)-B(2);
err=sqrt(abs((x*x)+(y*y)));							// Calculates the error.

else err=0;
    end
endfunction


					/****** FUNCTION FOR DETECTING MOTION USING MOTHER-BOT CAMERA *******/


function [temp2,center1]= motiondetectionCam1(frame2,h)
   center1 = 0;
[height,width] = size(frame2);
for i = 1 :1,
    temp2=zeros(height,width,3);                                   
    negative=zeros(height,width);
    greenIndex = height*width;
    blueIndex = 2*height*width;
    frame2hsv=rgb2hsv(frame2);
    index1=find(frame2hsv(:,:,1)>0.6 & frame2hsv(:,:,1)<0.69 & frame2hsv(:,:,2)>0.69 & frame2hsv(:,:,3)>0.69  );   //blue color detection
    temp2( blueIndex + index1)=250;                                                                                //blue color in filtered image
    negative(index1)=1;
		// GIVES THE MAXIMUM COLUMN NUMBER FOR THE IMAGE
    maxcol=10;
    maxval=0;
    for col = 1 : width,
    	s=sum(negative(:,col));
    	if (s > maxval) then,
    		maxval=s;
    		maxcol=col;
    	end;
    end
		// GIVES THE MAXIMUM ROW NUMBER FOR THE IMAGE
    maxrow=1;
    maxval=0;
    for row = 1 : height,
    	s=sum(negative(row,:));
    	if (s > maxval) then,
    		maxval=s;
    		maxrow=row;
    	end;
    end

    temp2 = rectangle(temp2, [maxcol, maxrow, 1,1],[255,255,255]);    

    /*
     *  divided the frame into 21 columns if the maxcol value is on the 
     *  left side region the mother-bot moves to the right in order to 
     *  align target bot to the center.  
     */

    if(maxcol<10*width/21) then,        
            writeserial(h,ascii(48));
            xpause(10000);
            writeserial(h,ascii(52));
            xpause(5000);
            writeserial(h,ascii(48));
            xpause(10000);
            writeserial(h,ascii(53));

     /*
     *  divided the frame into 21 columns if the maxcol value is in the 
     *  right side region the mother-bot moves to the left in order to 
     *  align target bot to the center.  
     */

    elseif (maxcol>13*width/21) then,            
                writeserial(h,ascii(48));
                xpause(10000);
                writeserial(h,ascii(54));
                xpause(5000);
                writeserial(h,ascii(48));
                xpause(10000);
                writeserial(h,ascii(53));

     /*
     *  divided the frame into 21 columns if the maxcol value is in the 
     *  center region  the mother-bot doesn't move any side. it stops.
     */ 
   
    elseif (maxcol<13*width/21 & maxcol>9*width/21) then,
                writeserial(h,ascii(48));
                xpause(10000);
                writeserial(h,ascii(53));
                xpause(10000);
		center1 = 1;
            end

end
endfunction


					/******   FUNCTION FOR DETECTING MOTION USING SHOOTING-BOT CAMERA  *******/


function [temp3,center2]=motiondetectionCam2(frame3,h)
center2 = 0;   
[height,width] = size(frame3);
for i = 1 :1,
    temp3=zeros(height,width,3);
    negative=zeros(height,width);
    greenIndex = height*width;
    blueIndex = 2*height*width;
    frame3hsv=rgb2hsv(frame3);
    index1=find(frame3hsv(:,:,1)>0.6 & frame3hsv(:,:,1)<0.69 & frame3hsv(:,:,2)>0.69 & frame3hsv(:,:,3)>0.69  ); 
    temp3( blueIndex + index1)=250;                                                                              
    negative(index1)=1;
    // gives the maximum cloumn number of the image
    maxcol=10;
    maxval=0;
    for col = 1 : width,
    	s=sum(negative(:,col));
    	if (s > maxval) then,
    		maxval=s;
    		maxcol=col;
    	end;
    end
    // gives the maximum row number of the image
    maxrow=1;
    maxval=0;
    for row = 1 : height,
    	s=sum(negative(row,:));
    	if (s > maxval) then,
    		maxval=s;
    		maxrow=row;
    	end;
    end
    
    temp3 = rectangle(temp3, [maxcol, maxrow, 1,1],[255,255,255]);    
    /*
     *  divided the frame into 21 columns if the maxcol value is on the 
     *  left side region the shooting-bot moves to the right in order to 
     *  align target bot to the center.  
     */
      if(maxcol<10*width/21) then,
        
            writeserial(h,ascii(49));
            xpause(10000);
            writeserial(h,ascii(52));
            xpause(5000);
            writeserial(h,ascii(49));
            xpause(10000);
            writeserial(h,ascii(53));
        
    /*
     *  divided the frame into 21 columns if the maxcol value is in the 
     *  right side region the shooting-bot moves to the left in order to 
     *  align target bot to the center.  
     */
    elseif (maxcol>13*width/21) then,
            
                writeserial(h,ascii(49));
                xpause(10000);
                writeserial(h,ascii(54));
                xpause(5000);
                writeserial(h,ascii(49));
                xpause(10000);
                writeserial(h,ascii(53));
    /*
     *  divided the frame into 21 columns if the maxcol value is in the 
     *  center region  the shooting-bot doesn't move any side. it stops.
     */ 
            
    elseif (maxcol<13*width/21 & maxcol>9*width/21) then,
                writeserial(h,ascii(49));
                xpause(10000);
                writeserial(h,ascii(53));
                xpause(10000);
		center2 = 1;
            end


end
endfunction


		/******   FUNCTION TO RETURN THE CO-ORDINATES AND DISTANCE BETWEEN MOTHER-BOT,SHOOTING-BOT AND TARGET-BOT    *******/


function [motX,motY,targX,targY,shootX,shootY,actualdistm,actualdists,erro,temp1] = topcam(frame1)
    
        [height,width] = size(frame1);                        			// READS THE HEIGHT AND WIDTH OF THE FRAME
        temp1=zeros(height,width,3);
        negativeRed=zeros(height,width);
        negativeBlue=zeros(height,width);
        negativeGreen=zeros(height,width);
        negativePink=zeros(height,width);

        greenIndex = height*width;
        blueIndex = 2*height*width;
        
        frame1hsv=rgb2hsv(frame1);
        index=find((frame1hsv(:,:,1)<0.2 | frame1hsv(:,:,1)>0.8)  & frame1hsv(:,:,2)>0.6 & frame1hsv(:,:,3)>0.6  );	                // RED COLOR DETECTION
        temp1(index)=250;
        indexBlue=find(frame1hsv(:,:,1)>0.6 & frame1hsv(:,:,1)<0.69 & frame1hsv(:,:,2)>0.69 & frame1hsv(:,:,3)>0.69  ); 		// BLUE COLOR DETECTION
        indexPink=find(frame1hsv(:,:,1)>0.75 & frame1hsv(:,:,1)<0.95 & frame1hsv(:,:,2)>0.50 & frame1hsv(:,:,3)>0.68  );                // PINK COLOR DETECTION 
        //indexYellow=find(frame1hsv(:,:,1)>0.09 & frame1hsv(:,:,1)<0.34 & frame1hsv(:,:,2)>0.65 & frame1hsv(:,:,3)>0.7  );             // YELLOW COLOR DETECTION
        
        indexGreen=find(frame1hsv(:,:,1)>0.25 & frame1hsv(:,:,1)<0.5 & frame1hsv(:,:,2)>0.5 & frame1hsv(:,:,3)>0.5  );                  // GREEN COLOR DETECTION
        
        //BLUE COLOR
        temp1( blueIndex + indexBlue)=250;
        
        // GREEN COLOR
        temp1( greenIndex + indexGreen) = 250;
        
        //RED COLOR
        temp1(index)=250;
        
        //PINK COLOR
        temp1(indexPink)=250;
        temp1(blueIndex+indexPink)=250;
        
	/* 
	 * counts the red-pixels in each column
	 * maintans the column number which has maximum count. 	
	 */
        negativeRed(index)=1;
        maxcolr=1;
        maxvalr=0;
        for col = 1 : width,
        	s=sum(negativeRed(:,col));
        	if (s > maxvalr) then,
        		maxvalr=s;
        		maxcolr=col;
        	end;
        end
        /*
	 * counts the red-pixels in each row
	 * maintans the row number which has maximum count. 	
	 */
        maxrowr=1;
        maxvalr=0;
        for row = 1 : height,
        	s=sum(negativeRed(row,:));
        	if (s > maxvalr) then,
        		maxvalr=s;
        		maxrowr=row;
        	end;
        end
        
        /* 
	 * counts the blue-pixels in each column
	 * maintans the column number which has maximum count. 	
	 */
        negativeBlue(indexBlue)=1;
        maxcolb=1;
        maxvalb=0;
        for col = 1 : width,
        	s=sum(negativeBlue(:,col));
        	if (s > maxvalb) then,
        		maxvalb=s;
        		maxcolb=col;
        	end;
        end
	/*
	 * counts the blue-pixels in each row
	 * maintans the row number which has maximum count. 	
	 */
        maxrowb=1;
        maxvalb=0;
        for row = 1 : height,
        	s=sum(negativeBlue(row,:));
        	if (s > maxvalb) then,
        		maxvalb=s;
        		maxrowb=row;
        	end;
        end

        
        /*
	 * counts the green-pixels in each column
	 * maintans the column number which has maximum count. 	
	 */
        negativeGreen(indexGreen)=1;
        maxcolg=1;
        maxvalg=0;
        for col = 1 : width,
        	s=sum(negativeGreen(:,col));
        	if (s > maxvalg) then,
        		maxvalg=s;
        		maxcolg=col;
        	end;
        end
	/*
	 * counts the green-pixels in each row
	 * maintans the row number which has maximum count. 	
	 */
        maxrowg=1;
        maxvalg=0;
        for row = 1 : height,

        	s=sum(negativeGreen(row,:));
        	if (s > maxvalg) then,
        		maxvalg=s;
        		maxrowg=row;
        	end;
        end
        
        /*
	 * counts the pink-pixels in each column
	 * maintans the column number which has maximum count. 	
	 */
        negativePink(indexPink)=1;
        maxcolp=1;
        maxvalp=0;
        for col = 1 : width,
        	s=sum(negativePink(:,col));
        	if (s > maxvalp) then,
        		maxvalp=s;
        		maxcolp=col;
        	end;
        end
	
	/*
	 * counts the pink-pixels in each row
	 * maintans the row number which has maximum count. 	
	 */
        maxrowp=1;
        maxvalp=0;
        for row = 1 : height,
        	s=sum(negativePink(row,:));
        	if (s > maxvalp) then,
        		maxvalp=s;
        		maxrowp=row;
        	end;
        end
        
        xm= (maxcolr - maxcolb);
        ym= (maxrowr - maxrowb);
        
        distancem = sqrt(abs(xm*xm + ym*ym));
        actualdistm=distancem*58/135;
        //disp(actualdistm);
        
        xs= (maxcolg - maxcolb);
        ys= (maxrowg - maxrowb);
        
        distances = sqrt(abs(xs*xs + ys*ys));
        actualdists=distances*58/135;
        //disp(actualdists);
        motX=maxcolr;
        motY=maxrowr;
        targX=maxcolb;
        targY=maxrowb;
        shootX=maxcolg;
        shootY=maxrowg;
        errox=maxrowb-maxrowp;
        erroy=maxcolb-maxcolp;
        perro=sqrt(abs(errox*errox+erroy*erroy));
        erro=perro*58/135;
        
endfunction

			/*******      FUNCTION TO CALCULATE THE ANGLE BETWEEN MOTHER-BOT AND TARGET-BOT      ********/


function [theta1]=angledetection1(frame2)
        [height,width] = size(frame2);
        negativeBlue=zeros(height,width);
        negativePink=zeros(height,width);
        //stacksize('max');
        greenIndex = height*width;
        blueIndex = 2*height*width;
        
        frame1hsv=rgb2hsv(frame2);
        indexBlue=find(frame1hsv(:,:,1)>0.6 & frame1hsv(:,:,1)<0.69 & frame1hsv(:,:,2)>0.69 & frame1hsv(:,:,3)>0.69  ); //blue detection
        indexPink=find(frame1hsv(:,:,1)>0.75 & frame1hsv(:,:,1)<0.95 & frame1hsv(:,:,2)>0.50 & frame1hsv(:,:,3)>0.68  );// pink detection
        //Blue color
        temp1( blueIndex + indexBlue)=250;
        
        //Pink color
        temp1(indexPink)=250;
        temp1(blueIndex+indexPink)=250;
        
        
        /*
	 * counts the blue-pixels in each column
	 * maintans the column number which has maximum count. 	
	 */
        negativeBlue(indexBlue)=1;
        maxcolb=1;
        maxvalb=0;
        for col = 1 : width,
        	s=sum(negativeBlue(:,col));
        	if (s > maxvalb) then,
        		maxvalb=s;
        		maxcolb=col;
        	end;
        end
        /*
	 * counts the blue-pixels in each row
	 * maintans the row number which has maximum count. 	
	 */
        maxrowb=1;
        maxvalb=0;
        for row = 1 : height,
        	s=sum(negativeBlue(row,:));
        	if (s > maxvalb) then,
        		maxvalb=s;
        		maxrowb=row;
        	end;
        end
        /*
	 * counts the pink-pixels in each column
	 * maintans the column number which has maximum count. 	
	 */
        negativePink(indexPink)=1;
        maxcolp=1;
        maxvalp=0;
        for col = 1 : width,
        	s=sum(negativePink(:,col));
        	if (s > maxvalp) then,
        		maxvalp=s;
        		maxcolp=col;
        	end;
        end
	/*
	 * counts the pink-pixels in each row
	 * maintans the row number which has maximum count. 	
	 */
        maxrowp=1;
        maxvalp=0;
        for row = 1 : height,
        	s=sum(negativePink(row,:));
        	if (s > maxvalp) then,
        		maxvalp=s;
        		maxrowp=row;
        	end;
        end
        
        x= abs((maxcolb - maxcolp));
       /*
	* By running the caliberation.sce code the below given values (58,135) are set. 
	* here 58 is the  actual distance between the two detected object used for caliberating the distance
	* 135 is value returned by the program caliberation.sce.
	* the Caliberation.sce program has to be run everytime the hight of the top cam is changed.
	*/
        perpendecular=x*58/135;
      
        theta1=atan(2*tan(25*%pi/180)*perpendecular/width);

endfunction


  					/*********    FUNCTION TO CALCULATE THE ANGLE BETWEEN SHOOTING-BOT AND TARGET-BOT    ********/


function [theta2]=angledetection2(frame3)
        [height,width] = size(frame3);
        negativeBlue=zeros(height,width);
        negativePink=zeros(height,width);
        greenIndex = height*width;
        blueIndex = 2*height*width;
        
        frame1hsv=rgb2hsv(frame3);
        indexBlue=find(frame1hsv(:,:,1)>0.6 & frame1hsv(:,:,1)<0.69 & frame1hsv(:,:,2)>0.69 & frame1hsv(:,:,3)>0.69  );     // BLUE COLOR DETECTION
        indexPink=find(frame1hsv(:,:,1)>0.75 & frame1hsv(:,:,1)<0.95 & frame1hsv(:,:,2)>0.50 & frame1hsv(:,:,3)>0.68  );    // PINK COLOR DETECTION
        //Blue color
        temp1( blueIndex + indexBlue)=250;
        
        //Pink color
        temp1(indexPink)=250;

        temp1(blueIndex+indexPink)=250;        
        
	/*
	 * counts the blue-pixels in each column
	 * maintans the column number which has maximum count. 	
	 */
        negativeBlue(indexBlue)=1;
        maxcolb=1;
        maxvalb=0;
        for col = 1 : width,
        	s=sum(negativeBlue(:,col));
        	if (s > maxvalb) then,
        		maxvalb=s;
        		maxcolb=col;
        	end;
        end

	/*
	 * counts the blue-pixels in each row
	 * maintans the row number which has maximum count. 	
	 */
        maxrowb=1;
        maxvalb=0;
        for row = 1 : height,
        	s=sum(negativeBlue(row,:));
        	if (s > maxvalb) then,
        		maxvalb=s;
        		maxrowb=row;
        	end;
        end

	/*
	 * counts the pink-pixels in each column
	 * maintans the column number which has maximum count. 	
	 */
        negativePink(indexPink)=1;
        maxcolp=1;
        maxvalp=0;
        for col = 1 : width,
        	s=sum(negativePink(:,col));
        	if (s > maxvalp) then,
        		maxvalp=s;
        		maxcolp=col;
        	end;
        end

	/*
	 * counts the pink-pixels in each row
	 * maintans the row number which has maximum count. 	
	 */
        maxrowp=1;
        maxvalp=0;
        for row = 1 : height,
        	s=sum(negativePink(row,:));
        	if (s > maxvalp) then,
        		maxvalp=s;
        		maxrowp=row;
        	end;
        end
        
        x= abs((maxcolb - maxcolp));
   
        perpendecular=x*58/135;

        theta2=atan(2*tan(25*%pi/180)*perpendecular/width);

endfunction

                                                                   
			/***********************************************************************************   
					                MAIN OR DRIVER FUNCTION   
			************************************************************************************/


    avicloseall;
    stacksize('max');				// ALLOCATES THE STACK OF SIZE 'MAX' 

    n1 = camopen(1);                            // OPENS THE TOP CAMERA
 
    n2 = camopen(2);				// OPENS THE CAMERA ON MOTHER-BOT

    n3 = camopen(3);				// OPENS THE CAMERA ON SHOOTING-BOT

    h=openserial(6,"9600,n,8,1");		// OPENS THE SERIAL COMMUNICATION WITH PORT NUMBER '6'
    
    for i = 1:300,
    frame1 = avireadframe(n1); 			// READS THE FRAME FROM TOP CAMERA
        
        frame2 = avireadframe(n2); 		// READS THE FRAME FROM MOTHER-BOT CAMERA

        frame3 = avireadframe(n3); 		// READS THE FRAME FROM SHOOTING-BOT CAMERA
        
        [motX,motY,targX,targY,shootX,shootY,actualdistm,actualdists,erro,temp1] = topcam(frame1);     
        [temp2,center1]=motiondetectionCam1(frame2,h);
        [temp3,center2]=motiondetectionCam2(frame3,h);
        
        theta1=angledetection1(frame2);             //calls the angle detection function

        angle1=theta1*180/%pi                       // converts the radians into angle

        theta2=angledetection2(frame3);            // calls the angle detection function

        angle2=theta2*180/%pi			   // converts the radians into angle
        
	if (center1 == 1 & center2 == 1) then,

	    	err = func(motX,motY,targX,targY,shootX,shootY,angle1,angle2)

		disp("--")
		disp(err);                                // displays the  calculated error using mother-bot, shooting-bot cameras
		disp(erro);                               // dispalys the error caluculated using top camera(radar) 
		disp("--")
	end;
        canvas=[uint8(frame1),uint8(frame2),uint8(frame3);uint8(temp1),uint8(temp2),uint8(temp3)];  // TAKES ALL THE 3 CAMERA IMAGES AND FILTERED IMAGES

        resized = imresize(canvas, 0.85);    // RESIZES THE CANVAS IMAGES

        imshow(resized);                    // DISPALYS THE ALL THE 3 IMAGES AND FILTERED IMAGES
    end
    avicloseall;                            // CLOSES ALL THE CAMERAS.
