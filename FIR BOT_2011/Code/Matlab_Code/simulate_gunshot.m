%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Authors:   Arjun Joshi 
%            Prashant Bhut
%            Manish Patidar
%
% Version:  1.0
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
clc;
clear;
% Code to sense gunshot
% y=audiorecorder(44000,8,1);
% recordblocking(y,2);
% data = getaudiodata(y);     % Received signal stored in data   
% wavwrite(data,44000,'gunshot');

fs= 30000;              % sampling frequency

% Bandwidth
Bl=3000;
Bh=Bl+3000;

%Analog frequencies
original_fs1=Bl-500     %stopband freq
original_fp1=Bl
original_fp2=Bh
original_fs2=Bh+500     %stopband freq
original_fc1=(original_fs1+original_fp1)/2
original_fc2=(original_fs2+original_fp2)/2


%Digital frequcy = Analog Frequency*2*pi/fs
Digital_Ws1=2*pi*original_fs1/fs
Digital_Wp1=2*pi*original_fp1/fs
Digital_Wp2=2*pi*original_fp2/fs
Digital_Ws2=2*pi*original_fs2/fs

% Cutoff frequencies for transition band
wc1=2*original_fc1/fs
wc2=2*original_fc2/fs

A=-20*log10(0.1)
deltaw=Digital_Ws2-Digital_Wp2;
%N=ceil((A-8)/(2.285*deltaw*2))+5

% Implemented a 15 order filter since limited by hardware on dsPIC
N=15
n=-N:N;

filter_ir=(wc2*sinc(wc2*n)-wc1*sinc(wc1*n));
% filter_ir=wc2*sinc(wc2*n);
plot(n,filter_ir);
title('Impulse response of the filter');

H_fr=abs(fft(filter_ir,30000));
H_fr1=zeros(30000,1);
H_fr1(:,1)=H_fr(1,:);
figure;
plot(H_fr1);
title('Frequency response of the filter');

[data fs] = wavread('gunshot');
figure;
plot(data);
title('Raw Data');
data1= fft(data,30000);          % using fft to simulate the required frequency of the filter
data1=abs(data1);
figure;
plot(data1);
title('data frequ.');
grid on;

filtered=data1.*H_fr1;
figure;
plot(filtered);
title('Output of the filter');

data_filtered=ifft(filtered);
figure;
plot(data_filtered);
title('IFFT of the filter output');

