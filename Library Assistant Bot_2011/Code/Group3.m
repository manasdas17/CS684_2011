

%********************************************************************************/
% Tracking red , blue , green objects in a video in real time.
%
% This works on the difference between frames concept.
% Every frame in the video is returnd as an rgb image on which we can do
% image processing stuff.
% Capture the video frames using the videoinput function
% You have to replace the resolution & your installed adaptor name.
 vid=videoinput('winvideo',2);

% Set the properties of the video object
set(vid, 'FramesPerTrigger', Inf);
set(vid, 'ReturnedColorspace', 'rgb')
vid.FrameGrabInterval = 5;
s = serial('COM41');
fopen(s);
flag=1;
%start the video aquisition here
start(vid)
% Set a loop that runs infinitely.
while(1)
    %if (flag==1)
        % Get the snapshot of the current frame
        data = getsnapshot(vid);    
        % Now to track red objects in real time
        % we have to subtract the red component 
        % from the grayscale image to extract the red components in the image.
        diff_im = imsubtract(data(:,:,1), rgb2gray(data)); 
        % Convert the resulting grayscale image into a binary image.
        diff_im = im2bw(diff_im,0.18);
        % Remove all those pixels less than 300px
        diff_im = bwareaopen(diff_im,300);
        % Label all the connected components in the image.
        bw = bwlabel(diff_im, 8);    
        % Here we do the image blob analysis.
        % We get a set of properties for each labeled region.
        stats = regionprops(bw, 'BoundingBox');
        % Display the image
        %imshow(data)
        imshow(data)
        %This is a loop to bound the red objects in a rectangular box.
        hold on
    
        for object = 1:length(stats)
            bb = stats(object).BoundingBox;
            c1=bb(1) + bb(3)/2;
            c2=bb(2) + bb(4)/2;
            rectangle('Position',bb,'EdgeColor','r','LineWidth',2)
            plot(c1,c2,'r*')
            plot(380,265,'b*')
            if(c1>=340 && c1<=420)
              fprintf(s,'1');
              display('Red Left')
                %flag=0;
         
                elseif(c1<340)
                fprintf(s,'1');
                display('Red')
           
                elseif(c1>420)
                fprintf(s,'1');
                display('Red Right')
            end
        end
        hold off
       
        %%%%%%%%%%%%%%%%%% Now Detect Green%%%%%%%%%%%%%%%%%%%%%%%%%%%55
        % Now to track Green objects in real time
        % we have to subtract the red component 
        % from the grayscale image to extract the red components in the image.
        diff_im = imsubtract(data(:,:,2), rgb2gray(data)); 
        % Convert the resulting grayscale image into a binary image.
        diff_im = im2bw(diff_im,0.18);
        % Remove all those pixels less than 300px
        diff_im = bwareaopen(diff_im,300);
        % Label all the connected components in the image.
        bw = bwlabel(diff_im, 8);    
        % Here we do the image blob analysis.
        % We get a set of properties for each labeled region.
        stats = regionprops(bw, 'BoundingBox');
        % Display the image
        %imshow(data)
        %imshow(data)
        %This is a loop to bound the red objects in a rectangular box.
        hold on
    
        for object = 1:length(stats)
            bb = stats(object).BoundingBox;
            c1=bb(1) + bb(3)/2;
            c2=bb(2) + bb(4)/2;
            rectangle('Position',bb,'EdgeColor','r','LineWidth',2)
            plot(c1,c2,'r*')
            plot(380,265,'b*')
            if(c1>=340 && c1<=420)
              fprintf(s,'3');
              display('Green Left')
                %flag=0;
         
                elseif(c1<340)
                fprintf(s,'3');
                display('Green')
           
                elseif(c1>420)
                fprintf(s,'3');
                display('Green Right')
            end
        end
        hold off
       
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Now Detect
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%Blue%%%%%%%%%%%%%%%%%%%%%%%%%
        %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        
        % Now to track blue objects in real time
        % we have to subtract the red component 
        % from the grayscale image to extract the red components in the image.
        diff_im = imsubtract(data(:,:,3), rgb2gray(data)); 
        % Convert the resulting grayscale image into a binary image.
        diff_im = im2bw(diff_im,0.18);
        % Remove all those pixels less than 300px
        diff_im = bwareaopen(diff_im,300);
        % Label all the connected components in the image.
        bw = bwlabel(diff_im, 8);    
        % Here we do the image blob analysis.
        % We get a set of properties for each labeled region.
        stats = regionprops(bw, 'BoundingBox');
        % Display the image
        %imshow(data)
        %imshow(data)
        %This is a loop to bound the red objects in a rectangular box.
        hold on
    
        for object = 1:length(stats)
            bb = stats(object).BoundingBox;
            c1=bb(1) + bb(3)/2;
            c2=bb(2) + bb(4)/2;
            rectangle('Position',bb,'EdgeColor','r','LineWidth',2)
            plot(c1,c2,'r*')
            plot(380,265,'b*')
            if(c1>=340 && c1<=420)
              fprintf(s,'2');
              display('Blue Left')
                %flag=0;
         
                elseif(c1<340)
                fprintf(s,'2');
                display('Blue')
           
                elseif(c1>420)
                fprintf(s,'2');
                display('Blue Right')
            end
        end
        hold off
       
        
end
% Both the loops end here.
% Stop the video aquisition.
stop(vid);
% Flush all the image data stored in the memory buffer.
flushdata(vid);
% Clear all variables
clear all