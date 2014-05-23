#include <stdio.h>
#include <iostream>
#include "cv.h"
#include "highgui.h"


#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>


using namespace std;

int fd1;
int fd2;
char *buff, *buffer, *bufptr;
int wr, rd, nbytes, tries;

int h = 480;
int w = 640;
int pixel_diff = 15;
long delay = 50000;

typedef struct {

    struct {
        int x;
        int y;
    } cen;
    int no_pixels;
} pos;

uchar& pixel(IplImage* img, int row, int col, int channel) {
    return ((uchar*) (img->imageData + img->widthStep * row))[col * img->nChannels + channel];
}

void move_robot(char dir) {
    fd1 = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
    struct termios options;

    tcgetattr(fd1, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    //options.c_cflag &= ~CNEW_RTSCTS;

    tcsetattr(fd1, TCSANOW, &options);

    if (fd1 == -1) {
        perror("pen_port: Unable to open /dev/ttyS0 � ");
    } else {
        fcntl(fd1, F_SETFL, 0);
        //printf("Port 1 has been sucessfully opened and %d is the file description\n",fd1);
    }
    switch (dir) {
        case 'L':
        case 'l':
            wr = write(fd1, "3", 1);
            //usleep(delay);
            //wr=write(fd1,"5",1);
            break;

        case 'R':
        case 'r':
            wr = write(fd1, "4", 1);
            //usleep(delay);
            //wr=write(fd1,"5",1);
            break;
        case 'F':
        case 'f':
            wr = write(fd1, "0", 1);
            //usleep(delay);
            //wr=write(fd1,"5",1);
            break;
        case 'p':
        case 'P':
            wr = write(fd1, "1", 1);
            //usleep(delay);
            //wr=write(fd1,"5",1);
            break;

        case 'q':
        case 'Q':
            wr = write(fd1, "2", 1);
            //usleep(delay);
            //wr=write(fd1,"5",1);
            break;

        case 'S':
        case 's':
            wr = write(fd1, "6", 1);
            break;
    }

    close(fd1);
}

pos find_centroid(IplImage *frame) {

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            //if(pixel(frame, row, col, 2)>=200&&pixel(frame, row, col, 0)<=100&&pixel(frame, row, col, 1)<=100)
            if ((pixel(frame, row, col, 2) - pixel(frame, row, col, 0)) >= 100 && (pixel(frame, row, col, 2) - pixel(frame, row, col, 1)) >= 100) {
                pixel(frame, row, col, 0) = 255;
                pixel(frame, row, col, 1) = 255;
                pixel(frame, row, col, 2) = 255;
            } else {
                pixel(frame, row, col, 0) = 0;
                pixel(frame, row, col, 1) = 0;
                pixel(frame, row, col, 2) = 0;
            }
        }
    }

    cvErode(frame, frame, 0, 5);
    cvDilate(frame, frame, 0, 5);
    ////// find centroid
    cv::Point2f cen(0.0, 0.0);

    int tot_points = 0;
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            if (pixel(frame, row, col, 0) != 0) {
                cen.x += float(col);
                cen.y += float(row);
                tot_points++;
            }
        }

    }

    cen.x = cen.x / float(tot_points);
    cen.y = cen.y / float(tot_points);

    int x = int(cen.x);
    int y = int(cen.y);

    for (int row = y - 5; row < y + 5; row++) {
        for (int col = x - 5; col < x + 5; col++) {
            if (row > 0 && row < 480 && col > 0 && col < 640) {

                pixel(frame, row, col, 0) = 255;
                pixel(frame, row, col, 1) = 0;
                pixel(frame, row, col, 2) = 255;

            }
        }

    }
    int width = 0, height = 0;


    pos position;
    position.cen.x = int(cen.x);
    position.cen.y = int(cen.y);
    position.no_pixels = tot_points;

    return position;

}

int main(int argc, char **argv) {
    char *fdir = NULL;
    if (optind < argc)
        fdir = argv[optind];
    CvCapture *capture = 0;
    IplImage *frame = 0, *output = 0;
    int key = 0;

    /* initialize camera */
    capture = cvCaptureFromCAM(0);

    /* always check */
    if (!capture) {
        fprintf(stderr, "Cannot open initialize webcam!\n");
        return 1;
    }

    /* create a window for the video */
    cvNamedWindow("result", CV_WINDOW_AUTOSIZE);
    int fnum = 0;

    pos prev_pos, curr_pos;

    prev_pos.cen.x = 320;
    prev_pos.cen.y = 240;
    char prev_action = 'l';
    while (key != 'q') {
        /* get a frame */
        frame = cvQueryFrame(capture);

        if (!frame) break;
        /* always check */
        // cvShowImage("result", frame);
        pos curr_pos = find_centroid(frame);

        //     cerr<<"centroid"<<curr_pos.cen.x<<" "<<curr_pos.cen.y<<"no of points "<<curr_pos.no_pixels<<endl;

        cvShowImage("result", frame);

        // robot control




        // cout<<"no pixels "<<curr_pos.no_pixels<<endl;
        if (curr_pos.no_pixels < 300) {
            //left
            cerr << "rotate left ";
            move_robot(prev_action);

        } else if (curr_pos.cen.x < (w / 2 + pixel_diff) && curr_pos.cen.x > (w / 2 - pixel_diff)) {
            //forward
            cerr << "forward ";
            move_robot('f');
        } else if (curr_pos.cen.x < (w / 2 - pixel_diff) && (curr_pos.cen.x - prev_pos.cen.x) > pixel_diff) {
            //left forward
            cerr << "left forward ";
            move_robot('p');
        } else if (curr_pos.cen.x > (w / 2 + pixel_diff) && (prev_pos.cen.x - curr_pos.cen.x) > pixel_diff) {
            //right forward
            cerr << "right forward ";
            move_robot('q');

        } else if (curr_pos.cen.x < (w / 2 - pixel_diff) /*&& (prev_pos.cen.x - curr_pos.cen.x) > pixel_diff*/) {
            //left
            cerr << "left ";
            move_robot('l');
            move_robot('f');
            prev_action = 'l';
        } else if (curr_pos.cen.x > (w / 2 + pixel_diff) /*&& (curr_pos.cen.x - prev_pos.cen.x) > pixel_diff)*/) {
            //right
            cerr << "right";
            move_robot('r');
            move_robot('f');
            prev_action = 'r';
        }
        //  cout<<"previous action"<<prev_action<<endl;
        prev_pos = curr_pos;



        key = cvWaitKey(2);
    }

    /* free memory */
    cvDestroyWindow("result");
    cvReleaseCapture(&capture);

    return 0;
}
