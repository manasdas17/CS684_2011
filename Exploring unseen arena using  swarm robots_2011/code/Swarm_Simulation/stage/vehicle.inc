# Vehicle.inc
# model for the robot "Vehicle"

#Vehicle's linefollowers
define vehicle_linefollowers linefollower
(
	# number of sonars
	scount 3                                # these are the no.s on top of each sensor..!!!! 

	# define the pose of each transducer [xpos ypos zpos]
	spose[0] [ 0.4 0.3 -.2 ]	# left sensor
	spose[1] [ 0.5 0 -0.15 ]	# middle sensor
	spose[2] [ 0.4 -0.3 -.2 ]		# right sensor

	# define width (in meters) which is to be scanned by each sensor
	# along the two axes
	scanWidth .2
	
	# specify if the colour detection algorithm should inverted
	# i.e. whether it is detecting a white(1) / black(0) line.
	# invert 0

	# define the size of each transducer
	# [xsize ysize] in meters
	ssize [0.01 0.05 0.01]
)




#Vehicle's sonars
define vehicle_sonars ranger
(
      # number of sonars
      scount 6                                     # these are the beams with orientation...!!! 

      # define the pose of each transducer [xpos ypos heading]
      spose[0] [ 0.75 -0.1875 0]	#fr left tooth
      spose[1] [ 0.75 0.1875 0]	#fr right tooth
      spose[2] [ 0.25 0.5 20]		# left corner
      spose[3] [ 0.25 -0.5 -20]	# right corner
      spose[4] [ 0.75 -0.5 0]	# left 2nd forward	
      spose[5] [ 0.75 0.5 0]	# right 2nd forward
      # define the field of view of each transducer 
      #[range_min range_max view_angle]
      sview [0.05 2.0 9]

      # define the size of each transducer
      # [xsize ysize] in meters
      ssize [0.01 0.05]
)


# vehicle's body
define vehicle position
(
      # actual size
      size [0.5 0.5 1]

      # Vehicle's centre of rotation is offset from its centre of area
      #origin [0.125 0 0 0]

      # the shape of Vehicle
 
	block
	(
		points 3
		#point[5] [0 0]
		#point[4] [0 1]
		#point[3] [0.75 1]
		point[2] [1 1]
		point[1] [1 0.75]
		point[0] [0.85 0.85]
		z [0 0.5]
	)

  
    # differential steering model
	drive "diff"
	localization "gps"         	  
	localization_origin [0 0 0 0] 	# Start odometry at (0, 0, 0).

	# sensors attached to bigbob
	vehicle_linefollowers()  
	
	# sensors attached to vehicle
	vehicle_sonars()  
	
	obstacle_return 1           	# Can hit things.
	ranger_return 1             	# reflects sonar beams 
	linefollower_return 1             	# reflects sonar beams
)

