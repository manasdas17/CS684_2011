% check for previously opened serial port
% we can't open already open port
% we need to close if it is open already
if(exist( 's', 'var')) % check if variable s exist
    % close it first
    fclose(s) 

    % delete it  
    delete(s)

    % remove it from workspace
    clear s
end

% clear all previous variable
clear all 

% To close all previous windows
close all

% size of the arena in picture (not the actual one)
arena_size=3000;

% initialize arena with all white color
arena(1:arena_size,1:arena_size)=255;

% size of the block to be printed on screen 
robo_size=120;
point_size=30;


% default screen scale
scaling_factor=1;


% Default location of the robot on the screen
x0(1:1000)=1000+robo_size/2;
y0(1:1000)=1000+robo_size/2;
theeta0(1:1000)=0;
       
% Display the message        
disp('Going to open COM30 port');

%initialize the Serial Port
s=serial('COM30', 'BaudRate',9600, 'DataBits',8, 'Terminator','');

% data will come as sxxxxe with no null character transmission while transmitting from MCU
% Initialize the Input buffer size it should be equal to packet size
% our packet size is 15, which is a fixed number
s.InputBufferSize=15;

% Finally open the serial port
% this may take 10-15 seconds
fopen(s);

% Display message
disp('Port succefully Opened');

% packet format $id,r,theeta#

% start infinite loop
while true

% index for recieved buffer
i=2;

% recieve the buffer in inbuff array
inbuff=fscanf(s);
    
% if something is recieved or packet found in buffer
% start parsing it
if(not(isempty(inbuff)))
 j=1;

% keep transfering characters till ',' found
 while(inbuff(i)~=',')
     str_id(j)=inbuff(i);
     i=i+1;
     j=j+1;
 end

 i=i+1;
 j=1;

% keep transfering characters till another ',' found
  while(inbuff(i)~=',')
     str_r(j)=inbuff(i);
     i=i+1;
     j=j+1;
 end

 i=i+1;
 j=1;

% keep transfering characters till '#' end of packet found
 while(inbuff(i)~='#')
     str_theeta(j)=inbuff(i);
     i=i+1;
     j=j+1;
 end

% if recieved id is empty or no packet recieved
if(isempty(str_id))
   % assign null id if no packet recieved
   tmp_id=0;
else
   % convert string id to number
   tmp_id=str2num(str_id);
end

% if recieved r is empty or no packet recieved
if(isempty(str_r))
   % assign null value    
   tmp_r=0;
else
    % else convert string r to number
    tmp_r=str2num(str_r);        
end


% if recieved theeta is empty or no packet recieved
if(isempty(str_theeta))
    % assign null value    
    tmp_theeta=0;
else
    % else convert string theeta to number
    tmp_theeta=str2num(str_theeta);        
end
       
    % store all values for processing
    num_id=tmp_id;
    num_theeta(num_id)=tmp_theeta;
    num_r(num_id)=tmp_r;
       

       % update theeta of the recieved robot id & convert it in degree
       theeta(num_id)=theeta0(num_id)+num_theeta(num_id)*12.85;
       
       % now theeta is in degree
       theeta0(num_id)=theeta(num_id);
       % theeta0 stores the current value for future use 
       
       % if the angle is more than 360 degree correct it
       if (theeta(num_id) >= 360)
           theeta(num_id)=mod(theeta(num_id),360);
       else if (theeta(num_id)<=-360)
               theeta(num_id)=mod(theeta(num_id),360); 
           end
       end

       % convert r into real distance
       r(num_id)=num_r(num_id)*12.92;
       
       % calculate r using polar coordinates
       x(num_id)=x0(num_id)+r(num_id)*cos(theeta(num_id)*pi/180);
       y(num_id)=y0(num_id)+r(num_id)*sin(theeta(num_id)*pi/180);
       
       %print recieved packet and robot coordinates with other information
       disp(sprintf('packet recieved=$%02d,%03d,%03d# and latest coordinate of robot %03d are (%03d,%03d) THETA= %d', num_id,num_r(num_id),num_theeta(num_id),num_id,x(num_id),y(num_id),theeta(num_id)));
      
       %convert num id into 32 bit integer
       x_int=int32(x(num_id));
       y_int=int32(y(num_id));
       
       % plot on the arena
       for ii=-point_size:point_size
           for jj=-point_size:point_size
               if not(((x_int+ii)>arena_size) || ((x_int+ii)<=0) ||((y_int+jj)>arena_size) || ((y_int+jj)<=0))             
               arena(x_int+ii,y_int+jj)=0;
               end
           end
       end
       
       % make current coordinates as previous coordinates
       x0(num_id)=x(num_id);
       y0(num_id)=y(num_id);
       
       % update the arena
       imshow(arena)

       % pause for next packet
       % this pause is required for matlab processing       
       pause(.05);
       
end
end



%imshow(Arena)
