/* 
*               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                    
*                                  SUPPLEMENTARY TO THE PROJECT CODE
*               ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*               RUN THIS CODE ONLY when the topcam is changed to a new height. update the values obtained to the main program
*               The main logic behind this code is to place two objects(say green and red ) apart by a distance, caliberation1 (cm)
*               And then mount the topcamera to find the a distance value,caliberation 2(cm) each pixel contributes.
*               In our program we have set the values of caliberation1 to 58cm and caliberation2 to 135cm. and kept running 
*               the main program with this value. So whenever the top camera is changed, no need to run the program everytime.
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


n1 = camopen(1);  			// OPENS THE TOP CAMERA
frame1 = avireadframe(n1);		// READS THE FRAME FROM TOP
[height,width] = size(frame1);
for i = 1 : 100,
frame1 = avireadframe(n1);
temp1=zeros(height,width,3);
negativeRed=zeros(height,width);
negativeGreen=zeros(height,width);
stacksize('max');

frame1hsv=rgb2hsv(frame1);		// convert to HSV SPACE
index=find((frame1hsv(:,:,1)<0.1 | frame1hsv(:,:,1)>0.893)  & frame1hsv(:,:,2)>0.7 & frame1hsv(:,:,3)>0.8  )///                  red color

greenIndex = height*width;
blueIndex = 2*height*width;
index1=find(frame1hsv(:,:,1)>0.6 & frame1hsv(:,:,1)<0.69 & frame1hsv(:,:,2)>0.69 & frame1hsv(:,:,3)>0.69  ); //blue detection
index2=find(frame1hsv(:,:,1)>0.75 & frame1hsv(:,:,1)<0.95 & frame1hsv(:,:,2)>0.50 & frame1hsv(:,:,3)>0.68  );// pink detection
indexYellow=find(frame1hsv(:,:,1)>0.09 & frame1hsv(:,:,1)<0.34 & frame1hsv(:,:,2)>0.65 & frame1hsv(:,:,3)>0.7  ); // Yellow detection
indexGreen=find(frame1hsv(:,:,1)>0.25 & frame1hsv(:,:,1)<0.5 & frame1hsv(:,:,2)>0.5 & frame1hsv(:,:,3)>0.5  ); // green detection

//temp1(greenIndex + index)=80;
temp1( blueIndex + index1)=250;

// Yellow Color
temp1( greenIndex + indexYellow) = 250;
temp1( blueIndex + indexYellow) = 250;



// Yellow Color
temp1( greenIndex + indexGreen) = 250;


//REd color
temp1(index)=250;

//Pink color
temp1(index2)=250;
temp1(blueIndex+index2)=250;

negativeRed(index)=1;

maxcolR=1;
maxvalR=0;
for col = 1 : width,
	s=sum(negativeRed(:,col));
	if (s > maxvalR) then,
		maxvalR=s;
		maxcolR=col;
	end;
end
maxrowR=1;
maxvalR=0;
for row = 1 : height,
	s=sum(negativeRed(row,:));
	if (s > maxvalR) then,
		maxvalR=s;
		maxrowR=row;
	end;
end

negativeGreen(indexGreen)=1;

maxcolG=1;
maxvalG=0;
for col = 1 : width,
	s=sum(negativeGreen(:,col));
	if (s > maxvalG) then,
		maxvalG=s;
		maxcolG=col;
	end;
end
maxrowG=1;
maxvalG=0;
for row = 1 : height,
	s=sum(negativeGreen(row,:));
	if (s > maxvalG) then,
		maxvalG=s;
		maxrowG=row;
	end;
end


x= (maxcolR - maxcolG);
y= (maxrowR - maxrowG);

// The following is the distance measured which gives the distance
// between the top camera and the ground level.
distance = sqrt(abs(x*x - y*y));
disp(distance);


// uncomment the following two lines and update the values 58 and 135 
// to cross check the values.
//actualdist=distance*58/135;
//disp(actualdist);


temp1 = rectangle(temp1, [maxcolR, maxrowR, 1, 1],[255,255,255]);
canvas=[uint8(frame1),uint8(temp1)];
imshow(canvas);
end
aviclose(n1);

