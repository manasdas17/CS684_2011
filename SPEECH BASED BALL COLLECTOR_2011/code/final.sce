//function to detect object and return the co-ordinates of the object in the 1st frame it is detected in the vector rectrec
//parameters 
//colour=colour of object to find; notfound=signal to be sent till object is not found; found=signal to be sent when object is found; objct=ball or basket

function rectret=findobject(colour,notfound,found,objct) 

obj_found=0;
	
rect=[0,0,0,0];

//vid=camopen();
fr=avireadframe(vid);
while ~isempty(fr)

		  str=notfound;
		  writeserial(handle,str+ascii(13))              //write to serial port that object is not found
		  //sleep(3000);
		  
	if colour=='red'
		set_val=1;
	elseif colour=='green'             
		set_val=2;
	else
		set_val=3;
	end;
	                                                      
	if colour=='blue'                                   //colour intensity threshold for colour detection
		cthresh=0.1;
	elseif colour=='red'
		cthresh=0.2;
	else 
		cthresh=0.05;
	end;
	
	count=0;
	maxm=0;
	reg=0;
	objx=zeros(info.Height);
	objy=zeros(info.Height);
	objcount=0;

	diff_im = imabsdiff(fr(:,:,set_val),rgb2gray(fr));           //subtract colour component from the image
	diff_im = im2bw(diff_im,cthresh);                            //convert image to binary
	mat=diff_im;
	
	flag1=0;

		xS=190;                                                 //search for the object in the range [190-350] x co-ordinates with 10 pixels gap
		xE=350;													//i.e search the object only when it appears in the center of the screen
		yS=1;
		yE=info.Height;
		
		if objct=='box'
			xS=230;
			xE=400;
		end;
		
		xS
		xE


	if ~obj_found,
		totcount=0;
			
	//seedfill algorith, to identify object cluster with size greater han 1000 pixels....seach co-ordinates in x-range defined with 10 pixels gap
    //as the object size is large we do not need to search each and every pixel
	
		for i=xS:10:xE,
			if flag1==1
					break;
			end;
			for j=yS:50:yE,
				if flag1==1
					break;
				end;
				//disp(i);disp(j);
				if mat(i,j)==%t
					count=0;
					objcount=0;
					savex=zeros(info.Height);
					savey=zeros(info.Height);
					reg=reg+1;
					totcount=0;
					count=count+1;
					stakx(count)=i;
					staky(count)=j;
					while count>0,
						mat(stakx(count),staky(count))=%f;
						objcount=objcount+1;
						savex(objcount)=stakx(count);
						savey(objcount)=staky(count);
						m=stakx(count);
						n=staky(count);
						count=count-1;
						totcount=totcount+1;
					
						if totcount > 1000    //object size
							flag1=1;
							objx=savex;
							objy=savey;
							obj_found=1;
							disp('obj found');
							break;
						end;
						
						if (m-1)>0 & mat(m-1,m)==%t
							count=count+1;
							stakx(count)=m-1;
							staky(count)=n;
						end;
						if (n+1)<info.Width & mat(m,n+1)==%t
							count=count+1;
							stakx(count)=m;
							staky(count)=n+1;
						end;	
						if (n-1)>0 & mat(m,n-1)==%t
							count=count+1;
							stakx(count)=m;
							staky(count)=n-1;
						end;
						if (m+1)<info.Height & mat(m+1,n)==%t
							count=count+1;
							stakx(count)=m+1;
							staky(count)=n;
						end;
						if (m-1)>0 & (n-1)>0 & mat(m-1,n-1)==%t
							count=count+1;
							stakx(count)=m-1;
							staky(count)=n-1;
						end;
						if (m+1)<info.Height & (n+1)<info.Width & mat(m+1,n+1)==%t
							count=count+1;
							stakx(count)=m+1;
							staky(count)=n+1;
						end;
						if (m-1)>0 & (n+1)<info.Width & mat(m-1,n+1)==%t
							count=count+1;
							stakx(count)=m-1;
							staky(count)=n+1;
						end;
						if (n-1)>0 & mat(m+1,n-1)==%t
							count=count+1;
							stakx(count)=m+1;
							staky(count)=n-1;
						end;
					end;
				end;
				if totcount>maxm
					maxm=totcount;
					objx=savex;
					objy=savey;
				end;							
			end;
		end;

		//maxm

		if maxm > 1000                //object size
			minRow=min(objx);
			maxRow=max(objx);
			minCol=min(objy);
			maxCol=max(objy);
			if maxRow>info.Height
				maxRow=info.Height;
			end;
			if maxCol>info.Width
				maxCol=info.Width;
			end;
			rect=[minCol,minRow,maxCol-minCol,maxRow-minRow];
			newObj=rectangle(fr,rect,[255,0,0]);
		else
			newObj=fr;
		end; //end condition for drawing rectangle

	end; // end if for obj found

	imshow(newObj);

	if obj_found==1      //object is found here..guide the bot to reach it
		break;
	end;
	
	fr=avireadframe(vid);
end; //end for loop for reading frames

	rectret=rect;     //return co-ordinates of the rectange where object is first spotted in the frame
endfunction


//use camshift function in scilab to trace the object in each frame
//parameters same as above function; additional parameter : action="pick" or "drop" the object when it is reached 
function traceobject(colour,notfound,found,action,objct)

rect=findobject(colour,notfound,found,objct)

fr=avireadframe(vid);

centrex=info.Width/2;
centrey=info.Height/2;

//avicloseframe(vid);
obj_win = camshift(fr, rect); //initialize tracker

    while ~isempty(fr),
	
          obj_win = camshift(fr); //camshift tracking    
		  disp('obj window');
          //obj_win
          im = rectangle(fr, obj_win, [255,0,0]);
          imshow(im);
		  cx=obj_win(1)+obj_win(3)/2;
		  cy=obj_win(2)+obj_win(4)/2;             //identify approaximate centre of object cluster in the rectangle
		  val=abs(cx-centrex);                    //val - distance between centre of frame and centre of object
		  disp(val);
		  if  val < 270                          //if the distance between centres is less than some threshold object is at centre of screen
			disp('move forward');
			str=found;
			disp('data sent');                   //send signal to firebird that object is found  
			disp(str+ascii(13));
						
			buf='0'
		    //buf=readserial(handle);             //read ack from firebird whether it received the signal or else send it again till ack-ed
			v=str+ascii(13);
		    while(buf ~= v & buf~=action)
				disp(buf);
				sleep(500);
				writeserial(handle,str+ascii(13))
				sleep(500);
				buf=readserial(handle)
				if (buf=='5')
					return;
				end;
		    end
			disp('data received');
			disp(buf);
		    
			if buf==action                        //after ack-ing the signal sent by scilab the firebird sends another signal, which indicates ball has been picked or dropped in the basket
				return                            //the program returns after recieving the signal
			end;
			
			buf='0'
		    buf=readserial(handle);
		    while(buf ~= action)
			  sleep(500);
			  buf=readserial(handle);
		    end
			disp('data received');
			disp(buf);
		    return

		else
		    str=notfound;                          //the object is not found yet...keep on sending this signal to the firebird
			disp('data sent');
			disp(str+ascii(13));
			writeserial(handle,str+ascii(13))
			//sleep(3000);
		  end;
          fr = avireadframe(vid);
    end;
endfunction


function main(objc)
//parameters for frame dimension
info.Height=500;
info.Width=700;


handle=openserial(6,"9600,n,8,1")        //open the serial communication interface with port_no=6
vid=camopen();                           //open the camera
str='1';             
disp('data sent');
disp(str+ascii(13));                     //send 2 dummy signals to firebird to initiate it before picking ball
writeserial(handle,str+ascii(13))
str='1';
disp('data sent');
disp(str+ascii(13));
writeserial(handle,str+ascii(13))

traceobject(objc,'1','2','3','ball');   //trace ball
disp('ball picked up');

obj_found=0;
fr=0;
str='4';
disp('data sent');
disp(str+ascii(13));
writeserial(handle,str+ascii(13))        //send 2 dummy signals to firebird to initiate it before dropping ball
str='4';
vid=camopen();                           //refresh the frames and re-initiate camera
disp('data sent');
disp(str+ascii(13));
writeserial(handle,str+ascii(13))
traceobject('green','4','5','6','box'); //trace basket

closeserial(handle);
endfunction                            //end main function


filename = "C:\Users\int\workspace\speechrecg\color.txt";   // read from this file which colour of ball to pick. basket colour by default is green
                                                            //the speech processing module writes to the above file indicating colour of ball to be picked
fid = mopen(filename, "r");
if (fid == -1)
	error("cannot open file for reading");
end;
[num_read, val(1)] = mfscanf(fid, "%s");
mclose(fid);
col=val(1);
disp(col);
	
main(col);                          // call main function with ball colour