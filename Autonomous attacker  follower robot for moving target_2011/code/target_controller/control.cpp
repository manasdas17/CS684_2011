#include <cmath>
#include <cstdlib>
#include <iostream>
#include <GL/glut.h>

#include <stdio.h>

#include <string.h>

#include <fcntl.h>

#include <errno.h>

#include <termios.h>

#include <unistd.h>


int delay = 0;

int fd1;

int fd2;

char *buff, *buffer, *bufptr;

int wr, rd, nbytes, tries;

using namespace std;
//Implement this function

void keyboard(unsigned char key, int x, int y) {
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
        //   printf("Port 1 has been sucessfully opened and %d is the file description\n", fd1);
    }
    switch (key) {
        case 'A': //left
        case 'a':
            wr = write(fd1, "2", 1);
            cerr << "left" << endl;
            break;
        case 'D': //right
        case 'd':
            wr = write(fd1, "3", 1);
            cerr << "right" << endl;
            break;
        case 'W': //forward
        case 'w':
            wr = write(fd1, "1", 1);
            cerr << "forward" << endl;
            //usleep(delay);
            //wr = write(fd1, "5", 1);
            break;
        case 's': //back
        case 'S':
            wr = write(fd1, "4", 1);
            cerr << "back" << endl;
            //usleep(delay);
            //wr = write(fd1, "5", 1);
            break;

        case 'P': //stop
        case 'p':
            wr = write(fd1, "5", 1);
            break;
    }

    close(fd1);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glutSwapBuffers();
}

int main(int argc, char *argv[]) {

    glutInit(&argc, argv);

    glutCreateWindow("Robot control");

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    //  glutMouseFunc(mouse);

    glutMainLoop();
}
