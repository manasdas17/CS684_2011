/////////////////////////////////////////////////////////////////////////
//   Project : Gesture Controlled Robot
//   File  : MainWindow.xaml.cs
//   Class : MainWindow
//   Description :
//              1. Processes Depth-data and Sekeltal-data read by 
//                 Kinect sensor
//              2. Updates Main window of the application
//              3. Recognizes gestures
//              4. Sends command to bot using Zigbee module through 
//                 serial port
//                
//   Date of Creation :     NA
//   Created by :           Microsoft
//   Date of Modification : September-November, 2011
//   Created by :           CS-684 Project Group-16
//   NOTE       :           For embedded comment look for "TEAM 16_2011"
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
//
// This module contains code to do Kinect NUI initialization and
// processing and also to display NUI streams on screen.
//
// Copyright © Microsoft Corporation.  All rights reserved.  
// This code is licensed under the terms of the 
// Microsoft Kinect for Windows SDK (Beta) from Microsoft Research 
// License Agreement: http://research.microsoft.com/KinectSDK-ToU
//
/////////////////////////////////////////////////////////////////////////
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Research.Kinect.Nui;
using System.IO.Ports;

namespace SkeletalViewer
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        #region Constructors
        public MainWindow()
        {
            InitializeComponent();
            // TEAM 16_2011
            // Initialize the serial port and open it for communication.
            // Check the device manager setting to get the correct COM
            // port number for your Zigbee module
            this.serialPort = new SerialPort("COM12", 9600, Parity.None, 8, StopBits.One);
            try
            {
                this.serialPort.Open();
            }
            catch (Exception)
            {
                MessageBox.Show("Error opening serial port for communication.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }

            this.state = false;
            this.clapStarted = false;
        }
        #endregion

        #region Variables for initialization of Kinect
        Runtime nui;
        int totalFrames = 0;
        int lastFrames = 0;
        DateTime lastTime = DateTime.MaxValue;

        // We want to control how depth data gets converted into false-color data
        // for more intuitive visualization, so we keep 32-bit color frame buffer versions of
        // these, to be updated whenever we receive and process a 16-bit frame.
        const int RED_IDX = 2;
        const int GREEN_IDX = 1;
        const int BLUE_IDX = 0;
        byte[] depthFrame32 = new byte[320 * 240 * 4];

        Dictionary<JointID, Brush> jointColors = new Dictionary<JointID, Brush>() { 
            {JointID.HipCenter, new SolidColorBrush(Color.FromRgb(169, 176, 155))},
            {JointID.Spine, new SolidColorBrush(Color.FromRgb(169, 176, 155))},
            {JointID.ShoulderCenter, new SolidColorBrush(Color.FromRgb(168, 230, 29))},
            {JointID.Head, new SolidColorBrush(Color.FromRgb(200, 0,   0))},
            {JointID.ShoulderLeft, new SolidColorBrush(Color.FromRgb(79,  84,  33))},
            {JointID.ElbowLeft, new SolidColorBrush(Color.FromRgb(84,  33,  42))},
            {JointID.WristLeft, new SolidColorBrush(Color.FromRgb(255, 126, 0))},
            {JointID.HandLeft, new SolidColorBrush(Color.FromRgb(215,  86, 0))},
            {JointID.ShoulderRight, new SolidColorBrush(Color.FromRgb(33,  79,  84))},
            {JointID.ElbowRight, new SolidColorBrush(Color.FromRgb(33,  33,  84))},
            {JointID.WristRight, new SolidColorBrush(Color.FromRgb(77,  109, 243))},
            {JointID.HandRight, new SolidColorBrush(Color.FromRgb(37,   69, 243))},
            {JointID.HipLeft, new SolidColorBrush(Color.FromRgb(77,  109, 243))},
            {JointID.KneeLeft, new SolidColorBrush(Color.FromRgb(69,  33,  84))},
            {JointID.AnkleLeft, new SolidColorBrush(Color.FromRgb(229, 170, 122))},
            {JointID.FootLeft, new SolidColorBrush(Color.FromRgb(255, 126, 0))},
            {JointID.HipRight, new SolidColorBrush(Color.FromRgb(181, 165, 213))},
            {JointID.KneeRight, new SolidColorBrush(Color.FromRgb(71, 222,  76))},
            {JointID.AnkleRight, new SolidColorBrush(Color.FromRgb(245, 228, 156))},
            {JointID.FootRight, new SolidColorBrush(Color.FromRgb(77,  109, 243))}
        };
        #endregion

        #region Our Variables
        // TEAM 16_2011
        /// <summary>
        /// Variable to track if the robot should be moving or should be in a stop state.
        /// </summary>
        private bool state;

        // TEAM 16_2011
        /// <summary>
        /// Flag to detect if the user is in the middle of a clap.
        /// </summary>
        private bool clapStarted;

        // TEAM 16_2011
        /// <summary>
        /// Flag to detect if the user is in the middle of a wave motion.
        /// </summary>
        private bool leftWaveStarted;
        private bool rightWaveStarted;

        // TEAM 16_2011
        // Variables for serial port communication.
        SerialPort serialPort;

        // TEAM 16_2011
        // Variables to track the position of various joints and co-ordinates.
        double r_wrist_y = 0;
        double l_wrist_y = 0;
        double r_wrist_x = 0;
        double l_wrist_x = 0;
        double r_wrist_z = 0;
        double l_wrist_z = 0;

        double head_y = 0;
        double head_z = 0;

        private double centreX;
        private double centreY;

        private double r_shoulder_x = 0;
        private double r_shoulder_y = 0;
        private double l_shoulder_x = 0;
        private double l_shoulder_y = 0;

        private double leftWaveStarty;
        private double leftWaveStartx;

        private double rightWaveStartx;
        private double rightWaveStarty;
        #endregion

        private void Window_Loaded(object sender, EventArgs e)
        {
            // TEAM 16_2011
            // Initialization
            nui = new Runtime();

            try
            {
                nui.Initialize(RuntimeOptions.UseDepthAndPlayerIndex | RuntimeOptions.UseSkeletalTracking | RuntimeOptions.UseColor);
            }
            catch (InvalidOperationException)
            {
                System.Windows.MessageBox.Show("Runtime initialization failed. Please make sure Kinect device is plugged in.");
                return;
            }


            try
            {
                nui.VideoStream.Open(ImageStreamType.Video, 2, ImageResolution.Resolution640x480, ImageType.Color);
                nui.DepthStream.Open(ImageStreamType.Depth, 2, ImageResolution.Resolution320x240, ImageType.DepthAndPlayerIndex);
            }
            catch (InvalidOperationException)
            {
                System.Windows.MessageBox.Show("Failed to open stream. Please make sure to specify a supported image type and resolution.");
                return;
            }

            lastTime = DateTime.Now;

            // TEAM 16_2011
            // Setting function 'nui_DepthFrameReady' as event handler of depth frame. 
            // As a depth frames comes from Kinect to be processed, this function will be called.
            // Similarly setting function 'nui_SkeletonFrameReady' and
            // 'nui_ColorFrameReady' as event handlers.
            nui.DepthFrameReady += new EventHandler<ImageFrameReadyEventArgs>(nui_DepthFrameReady);
            nui.SkeletonFrameReady += new EventHandler<SkeletonFrameReadyEventArgs>(nui_SkeletonFrameReady);
            nui.VideoFrameReady += new EventHandler<ImageFrameReadyEventArgs>(nui_ColorFrameReady);
        }

        // Converts a 16-bit grayscale depth frame which includes player indexes into a 32-bit frame
        // that displays different players in different colors
        byte[] convertDepthFrame(byte[] depthFrame16)
        {
            for (int i16 = 0, i32 = 0; i16 < depthFrame16.Length && i32 < depthFrame32.Length; i16 += 2, i32 += 4)
            {
                int player = depthFrame16[i16] & 0x07;
                int realDepth = (depthFrame16[i16 + 1] << 5) | (depthFrame16[i16] >> 3);
                // transform 13-bit depth information into an 8-bit intensity appropriate
                // for display (we disregard information in most significant bit)
                byte intensity = (byte)(255 - (255 * realDepth / 0x0fff));

                depthFrame32[i32 + RED_IDX] = 0;
                depthFrame32[i32 + GREEN_IDX] = 0;
                depthFrame32[i32 + BLUE_IDX] = 0;

                // choose different display colors based on player
                switch (player)
                {
                    case 0:
                        depthFrame32[i32 + RED_IDX] = (byte)(intensity / 2);
                        depthFrame32[i32 + GREEN_IDX] = (byte)(intensity / 2);
                        depthFrame32[i32 + BLUE_IDX] = (byte)(intensity / 2);
                        break;
                    case 1:
                        depthFrame32[i32 + RED_IDX] = intensity;
                        break;
                    case 2:
                        depthFrame32[i32 + GREEN_IDX] = intensity;
                        break;
                    case 3:
                        depthFrame32[i32 + RED_IDX] = (byte)(intensity / 4);
                        depthFrame32[i32 + GREEN_IDX] = (byte)(intensity);
                        depthFrame32[i32 + BLUE_IDX] = (byte)(intensity);
                        break;
                    case 4:
                        depthFrame32[i32 + RED_IDX] = (byte)(intensity);
                        depthFrame32[i32 + GREEN_IDX] = (byte)(intensity);
                        depthFrame32[i32 + BLUE_IDX] = (byte)(intensity / 4);
                        break;
                    case 5:
                        depthFrame32[i32 + RED_IDX] = (byte)(intensity);
                        depthFrame32[i32 + GREEN_IDX] = (byte)(intensity / 4);
                        depthFrame32[i32 + BLUE_IDX] = (byte)(intensity);
                        break;
                    case 6:
                        depthFrame32[i32 + RED_IDX] = (byte)(intensity / 2);
                        depthFrame32[i32 + GREEN_IDX] = (byte)(intensity / 2);
                        depthFrame32[i32 + BLUE_IDX] = (byte)(intensity);
                        break;
                    case 7:
                        depthFrame32[i32 + RED_IDX] = (byte)(255 - intensity);
                        depthFrame32[i32 + GREEN_IDX] = (byte)(255 - intensity);
                        depthFrame32[i32 + BLUE_IDX] = (byte)(255 - intensity);
                        break;
                }
            }
            return depthFrame32;
        }

        void nui_DepthFrameReady(object sender, ImageFrameReadyEventArgs e)
        {
            PlanarImage Image = e.ImageFrame.Image;
            byte[] convertedDepthFrame = convertDepthFrame(Image.Bits);

            depth.Source = BitmapSource.Create(
                Image.Width, Image.Height, 96, 96, PixelFormats.Bgr32, null, convertedDepthFrame, Image.Width * 4);

            ++totalFrames;

            DateTime cur = DateTime.Now;
            if (cur.Subtract(lastTime) > TimeSpan.FromSeconds(1))
            {
                int frameDiff = totalFrames - lastFrames;
                lastFrames = totalFrames;
                lastTime = cur;
                // frameRate.Text = frameDiff.ToString() + " fps by team 16";
            }

            #region Standard movement
            // TEAM 16_2011
            // DETECTING CLAP:
            // Checking whether both x-coordinate and y-coordinate of left wrist and
            // right wrist has come very close. 
            // Here we are checking whether their range is within 20 pixel.
            if ((r_wrist_y < l_wrist_y + 20 && r_wrist_y > l_wrist_y - 20) && (r_wrist_x < l_wrist_x + 20 && r_wrist_x > l_wrist_x - 20))
            {
                this.clapStarted = true;
            }
            // TEAM 16_2011
            // TOGGLE between START and STOP
            // Bot will move and stop depending on state which
            // will toggle when each clap ENDS.
            else if (this.clapStarted)
            {
                this.clapStarted = false;
                this.state = !this.state;
            }


            // TEAM 16_2011
            // RIGHT TURN :
            // If right wrist is above head, turn right.
            // Sending command through serial port
            if (r_wrist_y < head_y)
            {
                gexturePrint.Text = "Command : Right";
                this.serialPort.Write("6");
                this.semaphore.Text = "";
            }
            // LEFT TURN :
            // If left wrist is above head, turn left.
            // Sending command through serial port
            else if (l_wrist_y < head_y)
            {
                gexturePrint.Text = "Command : Left";
                this.serialPort.Write("4");
                this.semaphore.Text = "";
            }
            else
            {
                this.semaphore.Text = "";
                if (this.state)
                {
                    gexturePrint.Text = "Command : Move";
                    this.serialPort.Write("8");
                }
                else
                {
                    gexturePrint.Text = "Command : Stop";
                    this.serialPort.Write("5");
                }
            }
            #endregion

            #region Flag Semaphores
            // Detecting various semaphores.
            double left_hand_slope = this.calculateSlope(this.l_shoulder_x, this.l_shoulder_y, this.l_wrist_x, this.l_wrist_y);
            double right_hand_slope = this.calculateSlope(this.r_shoulder_x, this.r_shoulder_y, this.r_wrist_x, this.r_wrist_y);

            //this.semaphore.Text = "L: " + left_hand_slope + " R: " + right_hand_slope;
            #endregion

            #region Motion gestures

            // TEAM 16_2011
            // MOTION GESTURE
            if (!this.clapStarted)
            {
                // TEAM 16_2011
                // DETECTING RIGHT and LEFT WAVE
                // If right arm goes to left side of body and then waves back to right,
                // the motion is detected as 'right wave'.
                // Similar for 'left wave'.
                if (this.l_wrist_x > this.centreX)
                {
                    this.leftWaveStarted = true;
                    this.leftWaveStartx = this.l_wrist_x;
                    this.leftWaveStarty = this.l_wrist_y;
                }

                if (this.r_wrist_x < this.centreX)
                {
                    this.rightWaveStarted = true;
                    this.rightWaveStartx = this.r_wrist_x;
                    this.rightWaveStarty = this.r_wrist_y;
                }

                if (this.rightWaveStarted)
                {
                    if ((Math.Abs(this.rightWaveStartx - this.r_wrist_x) > 100) && (Math.Abs(this.rightWaveStarty - this.r_wrist_y) < 40))
                    {
                        this.semaphore.Text = "Right Wave";
                        // TEAM 16_2011
                        // On right wave moving to a fixed amount of right direction.
                        for (int i = 0; i < 10; i++)
                        {
                            this.serialPort.Write("6");
                        }
                        //this.rightWaveStarted = false;
                    }
                    else if (Math.Abs(this.rightWaveStarty - this.r_wrist_y) > 40)
                    {
                        this.rightWaveStarted = false;
                    }
                }

                if (this.leftWaveStarted)
                {
                    if ((Math.Abs(this.leftWaveStartx - this.l_wrist_x) > 100) && (Math.Abs(this.leftWaveStarty - this.l_wrist_y) < 40))
                    {
                        this.semaphore.Text = "Left Wave";
                        // TEAM 16_2011
                        // On left wave moving to a fixed amount of left direction.
                        for (int i = 0; i < 10; i++)
                        {
                            this.serialPort.Write("4");
                        }
                        //this.leftWaveStarted = false;
                    }
                    else if (Math.Abs(this.leftWaveStarty - this.l_wrist_y) > 40)
                    {
                        this.leftWaveStarted = false;
                    }
                }
            }
            #endregion
        }

        private double calculateSlope(double x1, double y1, double x2, double y2)
        {
            double slope = 0;

            if (Math.Abs(x2 - x1) != 0)
            {
                slope = Math.Atan2((y2 - y1), (x2 - x1));
            }
            else
            {
                slope = (y1 < y2) ? 0 : Math.PI;
            }

            return Math.Round((slope * 180) / Math.PI, 0);
        }

        private Point getDisplayPosition(Joint joint)
        {
            float depthX, depthY;
            nui.SkeletonEngine.SkeletonToDepthImage(joint.Position, out depthX, out depthY);
            depthX = depthX * 320; //convert to 320, 240 space
            depthY = depthY * 240; //convert to 320, 240 space
            int colorX, colorY;
            ImageViewArea iv = new ImageViewArea();
            // only ImageResolution.Resolution640x480 is supported at this point
            nui.NuiCamera.GetColorPixelCoordinatesFromDepthPixel(ImageResolution.Resolution640x480, iv, (int)depthX, (int)depthY, (short)0, out colorX, out colorY);

            // map back to skeleton.Width & skeleton.Height
            return new Point((int)(skeleton.Width * colorX / 640.0), (int)(skeleton.Height * colorY / 480));
        }

        Polyline getBodySegment(Microsoft.Research.Kinect.Nui.JointsCollection joints, Brush brush, params JointID[] ids)
        {
            PointCollection points = new PointCollection(ids.Length);
            for (int i = 0; i < ids.Length; ++i)
            {
                points.Add(getDisplayPosition(joints[ids[i]]));
            }

            Polyline polyline = new Polyline();
            polyline.Points = points;
            polyline.Stroke = brush;
            polyline.StrokeThickness = 5;
            return polyline;
        }

        void nui_SkeletonFrameReady(object sender, SkeletonFrameReadyEventArgs e)
        {
            SkeletonFrame skeletonFrame = e.SkeletonFrame;
            int iSkeleton = 0;
            Brush[] brushes = new Brush[6];
            brushes[0] = new SolidColorBrush(Color.FromRgb(255, 0, 0));
            brushes[1] = new SolidColorBrush(Color.FromRgb(0, 255, 0));
            brushes[2] = new SolidColorBrush(Color.FromRgb(64, 255, 255));
            brushes[3] = new SolidColorBrush(Color.FromRgb(255, 255, 64));
            brushes[4] = new SolidColorBrush(Color.FromRgb(255, 64, 255));
            brushes[5] = new SolidColorBrush(Color.FromRgb(128, 128, 255));

            skeleton.Children.Clear();
            foreach (SkeletonData data in skeletonFrame.Skeletons)
            {
                if (SkeletonTrackingState.Tracked == data.TrackingState)
                {
                    // Draw bones
                    Brush brush = brushes[iSkeleton % brushes.Length];
                    skeleton.Children.Add(getBodySegment(data.Joints, brush, JointID.HipCenter, JointID.Spine, JointID.ShoulderCenter, JointID.Head));
                    skeleton.Children.Add(getBodySegment(data.Joints, brush, JointID.ShoulderCenter, JointID.ShoulderLeft, JointID.ElbowLeft, JointID.WristLeft, JointID.HandLeft));
                    skeleton.Children.Add(getBodySegment(data.Joints, brush, JointID.ShoulderCenter, JointID.ShoulderRight, JointID.ElbowRight, JointID.WristRight, JointID.HandRight));
                    skeleton.Children.Add(getBodySegment(data.Joints, brush, JointID.HipCenter, JointID.HipLeft, JointID.KneeLeft, JointID.AnkleLeft, JointID.FootLeft));
                    skeleton.Children.Add(getBodySegment(data.Joints, brush, JointID.HipCenter, JointID.HipRight, JointID.KneeRight, JointID.AnkleRight, JointID.FootRight));

                    // Draw joints
                    //motion = 0;
                    foreach (Joint joint in data.Joints)
                    {
                        Point jointPos = getDisplayPosition(joint);
                        Line jointLine = new Line();
                        jointLine.X1 = jointPos.X - 3;
                        jointLine.X2 = jointLine.X1 + 6;
                        jointLine.Y1 = jointLine.Y2 = jointPos.Y;
                        jointLine.Stroke = jointColors[joint.ID];
                        jointLine.StrokeThickness = 6;
                        skeleton.Children.Add(jointLine);

                        // TEAM 16_2011
                        // POPULATE DATA
                        // Here populate programming variables with the data processed 
                        if (joint.ID == JointID.WristRight)
                        {
                            r_wrist_y = jointPos.Y;
                            r_wrist_x = jointPos.X;
                        }
                        if (joint.ID == JointID.WristLeft)
                        {
                            l_wrist_y = jointPos.Y;
                            l_wrist_x = jointPos.X;
                        }
                        if (joint.ID == JointID.Head)
                        {
                            head_y = jointPos.Y;
                        }

                        if (joint.ID == JointID.HipRight)
                        {
                            this.r_shoulder_x = joint.Position.X;
                            this.r_shoulder_y = joint.Position.Y;
                        }

                        if (joint.ID == JointID.HipLeft)
                        {
                            this.l_shoulder_x = joint.Position.X;
                            this.l_shoulder_y = joint.Position.Y;
                        }

                        if (Math.Abs(l_wrist_x - r_wrist_x) > 100)
                        {
                            this.centreX = (this.l_wrist_x + this.r_wrist_x) / 2;
                        }
                    }
                }
                iSkeleton++;
            } // for each skeleton
        }

        void nui_ColorFrameReady(object sender, ImageFrameReadyEventArgs e)
        {
            // 32-bit per pixel, RGBA image
            PlanarImage Image = e.ImageFrame.Image;
            video.Source = BitmapSource.Create(
                Image.Width, Image.Height, 96, 96, PixelFormats.Bgr32, null, Image.Bits, Image.Width * Image.BytesPerPixel);
        }

        private void Window_Closed(object sender, EventArgs e)
        {
            nui.Uninitialize();
            // Close the serial port.
            try
            {
                this.serialPort.Close();
            }
            catch (InvalidOperationException)
            {
                MessageBox.Show("An error occurred while closing the port.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
            Environment.Exit(0);
        }
    }
}
