#! /bin/ksh

#------------------------------------------------------------------------
#
# Copyright (c) 2010 by Mark Galea, University of Edinburgh
#
# See the file "license.txt" for information on usage and redistribution of this file, 
# and for a DISCLAIMER OF ALL WARRANTIES.
#
# run_face_detect.ksh
#
# Mark Galea mmgalea@gmail.com
#
# Usage : see function "usage" below
#
# Takes a video in any format and determines the frames in which there is a face.  
# #
# Change History:
#------------------------------------------------------------------------

function usage
{
	print "run_face_detect.ksh - a wrapper around MPEG-1/2 and uncompressed video face detectors"
	print "Usage: run_shotdetect.ksh [-o STR] [-v STR] "
	print "  -o   Output directory"
	print "  -v   Full path to the video file"
	print "  -h   Print help and exit"
	print "Example: ./run_face_detect.ksh -o ./results/ -v ./videos/test.mpg "
}

while getopts "o:v:h" arg
do
	case $arg in
	    o)
	    	out_dir=$OPTARG
		;;
	    v)
	    	video_fp=$OPTARG
		;;
	    h|*)
	    	usage
			exit 1
		;;
	esac
done

if [[ -z $out_dir || -z $video_fp ]];
then
	print "*** ERROR: Mandatory parameters cannot be empty! ***"
	usage
	exit 1
fi


echo "Command line parameters are: out_dir = "$out_dir", video_fp = "$video_fp


# Ensures the appropriate environment variables are configured for the script. N.B: This needs editing for your
# particular machine before running the script.
function set_env_vars
{
	FACE_DETECT="./"
	export FACE_DETECT

	HAAR_PROFILE="/opt/local/share/OpenCV/haarcascades/haarcascade_profileface.xml"
	export HAAR_PROFILE

	HAAR_FRONTAL="/opt/local/share/OpenCV/haarcascades/haarcascade_frontalface_alt2.xml"
	export HAAR_FRONTAL

	LD_LIBRARY_PATH="/opt/local/lib"
	export LD_LIBRARY_PATH
}

# Verifies that the required libraries are installed correctly on the machine
function check_install
{
	print "*** Searching Software" $FACE_DETECT"/main"
	if [[ ! -x "$FACE_DETECT/main" ]]; 
	then
		print "*** ERROR: Face Detect software not installed on this machine. Cannot continue. ***"
		print "*** ERROR: Please install the Face Detection module and update script environment variables accordingly. ***"
		usage
		exit 1
	fi
}


function main
{
	set_env_vars
	check_install
	
	if [[ ! -d $out_dir ]]
	then
		mkdir $out_dir 
		chmod -R +rwx $out_dir 
	fi
	
	echo "*** Performing Face Detection ***";
	echo "$FACE_DETECT/main" -f $video_fp -o $out_dir -r $HAAR_FRONTAL -s $HAAR_PROFILE 
	"$FACE_DETECT/main" -f $video_fp -o $out_dir -r $HAAR_FRONTAL -s $HAAR_PROFILE
	echo "*** Finished Face Detection ***"

	exit 0
}

main
