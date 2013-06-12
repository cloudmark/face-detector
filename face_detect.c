#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <highgui.h>
#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <time.h>
#include <string.h>



// ========== SIGNATURES ==========
int load_movie_and_detect(char *video_fp, char *out_dir);
void detect_and_draw_face( IplImage* img , int mode); 

// ========== DEFINITIONS ==========
#define PACKAGE "face_detect"
#define VERSION "0.0.1"


// ========== VARIABLES ========== 
CvCapture* _g_capture = NULL; 
CvHaarClassifierCascade *_cascade_frontal = NULL; 
CvHaarClassifierCascade *_cascade_side = NULL; 
CvMemStorage* _storage = NULL;
IplImage *face1 = NULL; 
IplImage *face2 = NULL; 
IplImage *face3 = NULL; 



// ========== VARIABLES ==========
void print_help(int exval)
{
	printf("%s,%s Finds the faces in a video. \n", PACKAGE, VERSION);
	printf("Usage: %s [-o STR] [-f STR]\n\n", PACKAGE);
	
	printf(" -h      print this help and exit\n");
	printf(" -v      print version info and exit\n\n");
	
	printf(" -f STR  Path to the video\n");
	printf(" -r STR  Path to the cascade front profile\n");
	printf(" -s STR  Path to the cascade side profile\n");
	printf(" -o STR  Output directory d\n");
	
	exit(exval);
}


int main (int argc, char **argv)
{
	int c;
	opterr = 0;
	
	char *out_dir=NULL;
	char *video_fp=NULL;
	char *cascade_front=NULL; 
	char *cascade_side=NULL; 
	
	while ((c = getopt (argc, argv, "h::v::f:r:s:o:")) != -1)
	{
		switch (c)
		{
			case 'h': 
				print_help(0);
				break;
			case 'v':
				fprintf(stdout, "%s %s\n", PACKAGE, VERSION);
				exit(0);
			case 'f':
				video_fp=optarg;
				break;
			case 'o':
				out_dir=optarg;
				break;
			case 'r':
				cascade_front=optarg;
				break; 
			case 's':
				cascade_side=optarg;
				break;
			case '?':
				if (optopt == 'f')
					fprintf (stderr, "Option -%c requires an argument.\n", optopt);
				if (optopt == 'd')
					fprintf (stderr, "Option -%d requires an argument.\n", optopt);
				if (optopt == 'r')
					fprintf (stderr, "Option -%d requires an argument.\n", optopt);
				if (optopt == 's')
					fprintf (stderr, "Option -%d requires an argument.\n", optopt);
				else if (isprint (optopt))
					fprintf (stderr, "Unknown option `-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character `\\x%x'.\n",optopt);
				return 1;
			default:
				abort ();
		}
	}
	
	printf("= Starting %s (%s) ====================================\n", PACKAGE, VERSION); 
	printf("\t Video: %s\n", video_fp); 
	printf("\t Ouput Dir: %s\n", out_dir); 
	printf("\t Frontal Cascade: %s\n", cascade_front);
	printf("\t Side Cascade: %s\n", cascade_side);
	printf("======================================================================\n"); 
	
	
	_storage =  cvCreateMemStorage(0);
	printf("Creating Frontal Cascade Profile\n"); 
	_cascade_frontal = (CvHaarClassifierCascade*)cvLoad(cascade_front, 0, 0, 0);
	if( !_cascade_frontal )
    {
        fprintf( stderr, "ERROR: Could not load classifier front cascade\n" );
        return -1;
    }
  
	printf("Creating Side Cascade Profile\n"); 
  _cascade_side  = (CvHaarClassifierCascade*)cvLoad(cascade_front, 0, 0, 0);
  if( !_cascade_side )
    {
        fprintf( stderr, "ERROR: Could not load classifier front cascade\n" );
        return -1;
    }
	
	load_movie_and_detect(video_fp, out_dir);
}




/*
 * Loads a movie into a window.  
 */
int load_movie_and_detect(char *video_fp, char *out_dir){
	IplImage* frame = NULL;
	
	// Create a named window.  
	cvNamedWindow( "Normal", CV_WINDOW_AUTOSIZE );
	cvNamedWindow("Face1", CV_WINDOW_AUTOSIZE); 
	cvNamedWindow("Face2", CV_WINDOW_AUTOSIZE); 
	cvNamedWindow("Face3", CV_WINDOW_AUTOSIZE); 

	// Loads the capture file.  Initialize to the begining of the AVI file.  
	printf("Loading Movie [%s] \n", video_fp); 
	if(strcmp(video_fp, "0") == 0){
		_g_capture = cvCreateCameraCapture(0);
	}else{
		_g_capture = cvCreateFileCapture( video_fp );
	}
	
	int frames = (int) cvGetCaptureProperty(_g_capture, CV_CAP_PROP_FRAME_COUNT); 
	int frame_rate = (int) cvGetCaptureProperty(_g_capture, CV_CAP_PROP_FPS); 
	
	printf("\t Statistics\n");
	printf("\t\tFrames: %d\n", frames);
	printf("\t\tFrame Rate: %d\n", frame_rate);
	
	while(1) {
		// Start reading the AVI file.  
		// Takes as argument a pointer to the capture data structure.
		// Grabs the next video frame into memory
		frame = cvQueryFrame( _g_capture );
		
		if( !frame ) break;
		// Show the image.  
		detect_and_draw_face(frame, 0); 
		detect_and_draw_face(frame, 1); 
		
		// We want to wait for the framerate.  
		int frame_per_second = 0;
		if (frame_rate == 0){
			frame_per_second = 24;
		}else{
			 frame_per_second = (int) (1000 / frame_rate);
		}
		
		char c = cvWaitKey(frame_per_second);
		// Exit if use hit the escape key.  
		if( c == 27 ) break;
		
	}
	cvReleaseCapture( &_g_capture );
	cvDestroyWindow("Normal");
	cvDestroyWindow("Face1");
	cvDestroyWindow("Face2");
	cvDestroyWindow("Face3");
	return 0; 
}


/*
 * Detect and draw an image.  
 */
// Function to detect and draw any faces that is present in an image
void detect_and_draw_face( IplImage* img, int mode )
{
	
    
    // Create two points to represent the face locations
    CvPoint pt1, pt2;
    int i;
	

	// There can be more than one face in an image. So create a growable sequence of faces.
	// Detect the objects and store them in the sequence
	
	CvSeq* faces = NULL;  
  if (mode == 0){
    faces = cvHaarDetectObjects(img, _cascade_frontal, _storage, 1.1, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(10, 10),  cvSize(80, 80) );
  } else{
	  faces = cvHaarDetectObjects(img, _cascade_side, _storage, 1.1, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(10, 10),  cvSize(80, 80) );
  }
	
	// Loop the number of faces found.
	for( i = 0; i < (faces ? faces->total : 0); i++ )
	{
		if (i > 2) break; 
		// Create a new rectangle for drawing the face
		CvRect* r = (CvRect*)cvGetSeqElem( faces, i );
		
		// Draw the rectangle in the input image
		cvSetImageROI(img, *r); 
		IplImage *temp = cvCreateImage(cvGetSize(img), img->depth, img->nChannels); 
		// Copy the area of interest
		cvCopy(img, temp, NULL); 
		
		switch (i) {
			case 0: 
				if(face1 != NULL) cvReleaseImage(&face1); 
				face1 = temp; 
				break;
			case 1: 
				if(face2 != NULL) cvReleaseImage(&face2); 
				face2 = temp; 
				break;
			case 2: 
				if(face3 != NULL) cvReleaseImage(&face3); 
				face3 = temp; 
				break;
				
			default:
				break;
		}
		cvResetImageROI(img); 
		
		
	
	}

	
    // Show the image in the window named "result"
    cvShowImage("Normal", img );
	if (face1 != NULL) cvShowImage("Face1", face1); 
	if (face2 != NULL) cvShowImage("Face2", face2); 
	if (face3 != NULL) cvShowImage("Face3", face3); 
}


