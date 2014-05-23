%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Authors:   Arjun Joshi 
%            Prashant Bhut
%            Manish Patidar
%
% Version:   1.0
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%zigbee module
clear all;

% Change the COM port name .. different for each PC
s = serial('COM28');
fopen(s);
%video capture
vid=videoinput('winvideo',1);
%start the video aquisition here
start(vid);   
while(1)
    % waits till Firebird sends 'C' command to matlab
    comm = fscanf(s,'%c',1);
    if comm == 'C'
        % Get the snapshot of the current frame
        data = getsnapshot(vid);    
        % Display the image
        imshow(data)
    end
end   


