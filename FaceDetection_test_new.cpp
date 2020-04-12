/* program to detect facial features using opencv*/

/* detectFacialfeatures() is the main function that detects the face followed by Eyes,nose and mouth detection.*/


//#include "stdafx.h"


/**
 * Loads static images from database and detect faces
 */
#include <stdio.h>
#include <conio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <cxcore.h>
#include "cvaux.h"

#define width_3D_head 140
#define frame_width 150
#define frame_height 125

CvHaarClassifierCascade *cascade;//*cascade_e,*cascade_nose,*cascade_mouth;
CvMemStorage            *storage;
char *face_cascade = "haarcascade_frontalface_alt2.xml";
float tz = 700;
float pose[6];
     pose[0] = 0;
     pose[1] = 0;
     pose[2] = 0;
     
float Rot[3][3], Rot_inv[3][3];
const int MAX_CORNERS = 500;  
//char *eye_cascade="haarcascade_mcs_eyepair_big.xml";
//char *nose_cascade="haarcascade_mcs_nose.xml";
//char *mouth_cascade="haarcascade_mcs_mouth.xml";

void inverse(float *Rot, float *Rot_inv)
{
     float det = 0;
     float temp;
     
     for(int i=0; i<3; i++)
     {
             det = det + (Rot[0][i]*(Rot[1][(i+1)%3]*Rot[2][(i+2)%3] - Rot[1][(i+2)%3]*Rot[2][(i+1)%3]));
     }
     
     for(int i=0;i<3;i++)
     {
          for(int j=0;j<3;j++)
          {
                Rot_inv[i][j] = ((Rot[(i+1)%3][(j+1)%3] * Rot[(i+2)%3][(j+2)%3]) - (Rot[(i+1)%3][(j+2)%3]*Rot[(i+2)%3][(j+1)%3]))/ determinant);
          }
     }
     
     for(int i=0; i<3; i++)
     {
          for(int j=0;j<3;j++)
          {
                  temp = Rot_inv[i][j];
                  Rot_inv[i][j] = Rot_inv[j][i];
                  Rot_inv[j][i] = temp;
          }
     }     
}

/*Cylindrical Model*/
void init_cylindrical_model(IplImage *img, CvRect* r, CvPoint2D32f* cornersA)
{
     float tx,ty;
     int u1, v1, u2, v2, un1, un2, vn1, vn2;
     int num_points;
     num_points = (int)(r->width/10)*(r->height/10);
     float un_x[num_points];
     float un_y[num_points];
     float xd[num_points], yd[num_points], zd[num_points];
     float xo[num_points], yo[num_points], zo[num_points];
     float rx, ry, fl;
     float height_3D_head;
     float height_2D_head, width_2D_head;
     int ppx, ppy;
     u1 = (frame_width/img->width)*r->x;
     v1 = (frame_height/img->height)*r->y;
     //u1 = r->x;
     //v1 = r->y;
     width_2D_head = (frame_width/img->width)*r->width;
     height_2D_head = (frame_height/img->height)*r->height;
     u2 = u1 + width_2D_head;
     v2 = v1 + height_2D_head;
     //u2 = r->x + r->width;
     //v2 = r->y + r-> height;
     rx = width_3D_head/2;
     ry = rx*1.2;
     fl = (tz/width_3D_head)*width_2D_head;
     height_3D_head = (1/fl)*height_2D_head*tz;
     ppx = frame_width/2;
     ppy = frame_height/2;
     //ppx = img->width/2;
     //ppy = img->height/2;
     
     tx = ((u1 - ppx)/fl)*tz + rx;
     //tx = (((frame_width/img->width)*r->x - (frame_width/2))/fl)*tz + rx;
     ty = ((v1 - ppy)/fl)*tz + height_3D_head/2;
     //ty = (((frame_height/img->height)*r->y - (frame_height/2))/fl)*tz + height_3D_head/2;
     
     pose[3] = tx;
     pose[4] = ty;
     pose[5] = tz;
     
     //un1 = (u1 - ppx)/fl;
     //un1 = u1 - ppx;
     //un2 = (u2 - ppx)/fl;
     //un2 = u2 - ppx;
     //vn1 = (v1 - ppy)/fl;
     //vn1 = v1 - ppy;
     //vn2 = (v2 - ppy)/fl;
     //vn2 = v2 - ppy;     
     
     for(int j = 0; j < num_points; j++)
     {
             un_x[j] = (cornersA[j].x - ppx)/fl;
             un_y[j] = (cornersA[j].y - ppx)/fl;
     }
     
     Rot[0][0] = 1;
     Rot[0][1] = -pose[2];
     Rot[0][2] = pose[1];
     Rot[1][0] = pose[2];
     Rot[1][1] = 1;
     Rot[1][2] = -pose[0];
     Rot[2][0] = -pose[1];
     Rot[2][1] = pose[0];
     Rot[2][2] = 1;
     
     inverse(Rot, Rot_inv);
     
     for(int j=0; j < num_points; j++)
     {
           xd[j] = Rot_inv[0][0]*un_x[j] + Rot_inv[0][1]*un_y[j] + Rot_inv[0][2];
           yd[j] = Rot_inv[1][0]*un_x[j] + Rot_inv[1][1]*un_y[j] + Rot_inv[1][2];
           zd[j] = Rot_inv[2][0]*un_x[j] + Rot_inv[2][1]*un_y[j] + Rot_inv[2][2];
     }
}

int main(int argc, char** argv)
{
    CvCapture* capture = 0;
    
    IplImage*  img;
    IplImage* img2;
    IplImage*  img_gray;
    IplImage*  temp_img;
    IplImage*  temp_img1;
    IplImage* img_lk;
    IplImage* imgC;
    IplImage* eig_image;
    IplImage* tmp_image;
    IplImage* test;
    IplImage* pyrA;
    IplImage* pyrB;
    int key = 0;
    int count = 0;
    int win_size = 10;
    int corner_count = MAX_CORNERS;
    int w;
    int h;
    int k;
    float feature_errors[ MAX_CORNERS ];  
    float m[6];
    double factor;
    char features_found[ MAX_CORNERS ]; 
    char image[100],temp_image[100];
    CvSize img_sz;
    CvSize pyr_sz;
    CvMat M;
    CvSeq* faces;
    CvRect *r;
    CvPoint2D32f* cornersA;// = new CvPoint2D32f[ MAX_CORNERS ];
    CvPoint2D32f* cornersB;// = new CvPoint2D32f[ MAX_CORNERS ];
    CvMemStorage* storage=cvCreateMemStorage(0);
    
    capture = cvCaptureFromCAM(0);
    //cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_WIDTH, 500 );
    //cvSetCaptureProperty( capture, CV_CAP_PROP_FRAME_HEIGHT, 300 );
    // always check 
    if ( !capture ) {
        fprintf( stderr, "Cannot open initialize webcam!\n" );
        return 1;
    }

    /* load the classifier 
       note that I put the file in the same directory with
       this code */
    storage = cvCreateMemStorage( 0 );
    cascade = ( CvHaarClassifierCascade* )cvLoad( face_cascade, 0, 0, 0 );
    //cascade_e = ( CvHaarClassifierCascade* )cvLoad( eye_cascade, 0, 0, 0 );
    //cascade_nose = ( CvHaarClassifierCascade* )cvLoad( nose_cascade, 0, 0, 0 );
    //cascade_mouth = ( CvHaarClassifierCascade* )cvLoad( mouth_cascade, 0, 0, 0 );


    
        //if( !(cascade)) //|| cascade_e ||cascade_nose||cascade_mouth) )
        //{
         //   fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
          //  //getchar();
         //   return -1;
        //}
    
    //cvNamedWindow( "image", CV_WINDOW_AUTOSIZE );
    //cvNamedWindow( "image_lk", 1 );
    //cvNamedWindow("image",0);
    //cvNamedWindow( "image1", CV_WINDOW_AUTOSIZE );
    //img1 = cvLoadImage("my_T.jpg");
    //cvShowImage("image1", img1);
    cvNamedWindow("LKpyr_OpticalFlow",1);  
    //cvNamedWindow("test1",1);  
    //cvNamedWindow("test2",1);  
    //cvNamedWindow("test3",1);  
    //cvNamedWindow("test4",1);  
    cvNamedWindow("result",CV_WINDOW_AUTOSIZE);
    
//    temp_img1 = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
    /*r->x = 100;
    r->y = 100;
    r->width = 100;
    r->height = 100;*/
    
    while(key != 'q')
    {
    //for(int j=20;j<27;j++)
    //{
        // sprintf(image,"D:\\ptz images\\%d.jpg",j);
        img = cvQueryFrame(capture);
        img_gray = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
        cvCvtColor(img,img_gray,CV_BGR2GRAY);
        //test = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
        //cvShowImage("test1",img_gray);
        ///////////////////
        img2 = cvQueryFrame(capture);
        temp_img1 = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
        cvCvtColor(img2,temp_img1,CV_BGR2GRAY);
        
        //temp_img1 = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
        //temp_img1 = cvLoadImage("previous.jpg", CV_LOAD_IMAGE_GRAYSCALE);
        
       /*  if(count==0)
         {
                     //cvCvtColor(img,temp_img1,CV_BGR2GRAY);
                     //temp_img1 = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
                     temp_img1 = img_gray;
                     count = 1;
         }*/
        /* always check */
        //if(!img) break;
        //lk(temp_img1, img_gray);
        //cvShowImage("test2",temp_img1);
        //printf("AB\n");
        
////////////////////////////////////////////////////////////////////////////////////Flow Start -> img_gray->first image, temp_img1->second image
        //CvSize      img_sz    = cvGetSize( temp_img1 );
        img_sz = cvGetSize( temp_img1 );
        //int         win_size = 10;
        //IplImage* imgC;
        imgC = temp_img1;
        
        //cvShowImage("test3",imgC);
        
        // The first thing we need to do is get the features  
        // we want to track.  
        //  
        //IplImage* eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
        eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
        //IplImage* tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
        tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
        //int              corner_count = MAX_CORNERS;  
        //CvPoint2D32f* cornersA = new CvPoint2D32f[ MAX_CORNERS ];  
        cornersA = new CvPoint2D32f[ MAX_CORNERS ];
        printf("Check1\n");
/////////////////////////////////////////////////
      if(faces->total == 1)
      {
        k = 0;
        
        for(int j = r->y; j < (r->y + r->height); j = j + 10)
        {
                for(int i = r->x; i < (r->x + r->width); i = i + 10)
                {
                        cornersA[k].x = (float)i;
                        cornersA[k].y = (float)j;
                        k++;
                }
        }
        corner_count = k-1;
        
        cvFindCornerSubPix(  
                             temp_img1,  
                             cornersA,  
                             corner_count,  
                             cvSize(win_size,win_size),  
                             cvSize(-1,-1),
                             cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03)  
                             );
      }
        //corner_count = k;
/////////////////////////////////////////////////
        
/////////////////////////////////////
        else
        {
            corner_count = MAX_CORNERS; 
            
             cvGoodFeaturesToTrack(  
                                temp_img1,
                                eig_image,  
                                tmp_image,  
                                cornersA,  
                                &corner_count,  
                                0.01,  
                                5.0,  
                                0,  
                                3,  
                                0,  
                                0.04  
                                );
                                
             cvFindCornerSubPix(  
                             temp_img1,  
                             cornersA,  
                             corner_count,  
                             cvSize(win_size,win_size),  
                             cvSize(-1,-1),
                             cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03)  
                             );
        }
////////////////////////////////////

        printf("%f\n", corner_count);

        printf("Check2\n");
        /*cvFindCornerSubPix(  
                             temp_img1,  
                             cornersA,  
                             corner_count,  
                             cvSize(win_size,win_size),  
                             cvSize(-1,-1),
                             cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03)  
                             );*/
        
        // Call the Lucas Kanade algorithm  
        //  
        //char features_found[ MAX_CORNERS ];  
        //float feature_errors[ MAX_CORNERS ];  
        //CvSize pyr_sz = cvSize( imgA->width+8, imgB->height/3 );
        pyr_sz = cvSize( temp_img1->width+8, round(img_gray->height/3) );
        printf("Check2.25\n");
        //IplImage* pyrA = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
        pyrA = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
        printf("Check2.35\n");
        //IplImage* pyrB = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );  
        pyrB = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );
        printf("Check2.45\n");
        //CvPoint2D32f* cornersB = new CvPoint2D32f[ MAX_CORNERS ];  
        cornersB = new CvPoint2D32f[ MAX_CORNERS ];
        
        printf("Check2.5\n");
        
        cvCalcOpticalFlowPyrLK(  
                                 temp_img1,  
                                 img_gray,  
                                 pyrA,  
                                 pyrB,  
                                 cornersA,  
                                 cornersB,  
                                 corner_count,  
                                 cvSize( win_size,win_size ),  
                                 5,  
                                 features_found,  
                                 feature_errors,  
                                 cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 ),  
                                 0  
                                 );  
       
        printf("Check3\n");
        
        // Now make some image of what we are looking at:  
        //  
        for( int i=0; i<corner_count; i++ ) {
        if( features_found[i]==0|| feature_errors[i]>550 ) {  
        //       printf("Error is %f/n",feature_errors[i]);  
                 continue;  
        }  
        //    printf("Got it/n");  
        CvPoint p0 = cvPoint(  
                cvRound( cornersA[i].x ),  
                cvRound( cornersA[i].y )  
                );  
                
        CvPoint p1 = cvPoint(  
                cvRound( cornersB[i].x ),  
                cvRound( cornersB[i].y )  
                );  
        cvLine( imgC, p0, p1, CV_RGB(255,0,0),2 );  
    }  
    
    printf("check4\n");
    //cvNamedWindow("ImageA",0);  
    //cvNamedWindow("ImageB",0);  
    //cvNamedWindow("LKpyr_OpticalFlow",1);  
    //cvShowImage("ImageA",imgA);  
    //cvShowImage("ImageB",imgB);  
    /////cvShowImage("LKpyr_OpticalFlow",imgC);  
    cvShowImage("LKpyr_OpticalFlow",imgC);
  
///////////////////////////////////////////////////////////////////////////////Flow End

///////////////////////////////////////////////////////////////////////////////////////////Face Detect Start
        printf("check5\n");
        factor = 1;
        
        //printf("%d\n",img->height);
        w = temp_img1->width;
        h = temp_img1->height;
        
        printf("check6\n");

        m[0] = (float)(factor*cos(0.0));
        m[1] = (float)(factor*sin(0.0));
        m[2] = w*0.5f;
        m[3] = -1*m[1];
        m[4] = m[0];
        m[5] = h*0.5f;
     
        M = cvMat( 2, 3, CV_32F, m );
        
        temp_img = img2;

        cvGetQuadrangleSubPix(img2, temp_img, &M);
        
        cvClearMemStorage( storage );
        
    
        if( cascade )
            faces = cvHaarDetectObjects(img2,cascade, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(20, 20));
        else
            printf("\nFrontal face cascade not loaded\n");

        printf("\n no of faces detected are %d\n",faces->total);
        //printf("Checkpoint 2\n");
    
        // for each face found, draw a red box 
   
        for(int i = 0 ; i < ( faces ? faces->total : 0 ) ; i++ )
        {        
                 r = ( CvRect* )cvGetSeqElem( faces, i );
                 cvRectangle(img2,cvPoint( r->x, r->y ),cvPoint( r->x + r->width, r->y + r->height ), CV_RGB(255, 0, 0), 1, 8, 0 );
                 printf("\n face_x=%d face_y=%d wd=%d ht=%d",r->x,r->y,r->width,r->height);
        
                 /*CvPoint center;
                 int radius;
                 center.x = cvRound(r->x + r->width*0.5);
                 center.y = cvRound(r->y + r->height*0.5);
                 radius = cvRound((r->width + r->height)*0.25);*/
                 
                 //radius = cvRound(r->width*0.5);
                 //detectEyes(img,r);
                 // reset region of interest 
                 //cvResetImageROI(img);
                 //detectNose(img,r);
                 //cvResetImageROI(img);
                 //detectMouth(img,r);
                 //cvResetImageROI(img);
        }
        
        cvShowImage("result",img2);
///////////////////////////////////////////////////////////////////////////////////////////Face Detect End

        //img=cvLoadImage(image);
        //cvShowImage("image", img);
        //temp_img=cvLoadImage(image);
        //temp_img = img;
        
        //cvCvtColor(img,temp_img1,CV_BGR2GRAY);
        
        //cvShowImage("test4",temp_img1);
        
        //temp_img1 = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,1);
        //cvSaveImage("previous.jpg", img_gray);
        //temp_img=cvLoadImage(img);
        
        //if(!img)
        //{
        //printf("Could not load image file and trying once again: %s\n",image);
        //}
        //printf("\n curr_image = %s",image);
        //printf("Checkpoint 1\n");
        ////detectFacialFeatures(img,temp_img/*,j*/);
        //printf("Checkpoint 5\n");
        

        key = cvWaitKey(100);
        //cvReleaseImage(&img);
        //cvReleaseImage(&temp_img);
        //printf("Checkpoint 6\n");
     }
    //}
  
    cvReleaseHaarClassifierCascade( &cascade );
    //cvReleaseHaarClassifierCascade( &cascade_e );
    //cvReleaseHaarClassifierCascade( &cascade_nose );
    //cvReleaseHaarClassifierCascade( &cascade_mouth );
    cvReleaseMemStorage( &storage );
    
     cvReleaseImage(&img);
     cvReleaseImage(&img2);
     cvReleaseImage(&temp_img);
     cvReleaseImage(&temp_img1);
     cvReleaseImage(&img_gray);
     cvReleaseImage(&imgC);
     cvReleaseImage(&eig_image);
     cvReleaseImage(&tmp_image);
     cvReleaseImage(&img_lk);
     cvReleaseImage(&test);
     cvReleaseImage(&pyrA);
     cvReleaseImage(&pyrB);
     cvReleaseCapture( &capture );
     free(cornersA);
     free(cornersB);
     
     //cvDestroyWindow( "image" );
     //cvDestroyWindow( "image_lk" );
     cvDestroyWindow( "LKpyr_OpticalFlow" );
     //cvDestroyWindow( "test1" );
     //cvDestroyWindow( "test2" );
     //cvDestroyWindow( "test3" );
     //cvDestroyWindow( "test4" );
     cvDestroyWindow( "result" );

    return 0;
}

/*void lk(IplImage *imgA, IplImage *imgB)
{
    CvSize      img_sz    = cvGetSize( imgA );
    int         win_size = 10;
    IplImage* imgC;
    imgC = imgA;
    // The first thing we need to do is get the features  
    // we want to track.  
    //  
    IplImage* eig_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
    IplImage* tmp_image = cvCreateImage( img_sz, IPL_DEPTH_32F, 1 );
    int              corner_count = MAX_CORNERS;  
    CvPoint2D32f* cornersA        = new CvPoint2D32f[ MAX_CORNERS ];  
    cvGoodFeaturesToTrack(  
        imgA,  
        eig_image,  
        tmp_image,  
        cornersA,  
        &corner_count,  
        0.01,  
        5.0,  
        0,  
        3,  
        0,  
        0.04  
    );  
    
    cvFindCornerSubPix(  
        imgA,  
        cornersA,  
        corner_count,  
        cvSize(win_size,win_size),  
        cvSize(-1,-1),  
        cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03)  
    );  
    // Call the Lucas Kanade algorithm  
    //  
    char features_found[ MAX_CORNERS ];  
    float feature_errors[ MAX_CORNERS ];  
    CvSize pyr_sz = cvSize( imgA->width+8, imgB->height/3 );  
    IplImage* pyrA = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );  
    IplImage* pyrB = cvCreateImage( pyr_sz, IPL_DEPTH_32F, 1 );  
    CvPoint2D32f* cornersB        = new CvPoint2D32f[ MAX_CORNERS ];  
    cvCalcOpticalFlowPyrLK(  
        imgA,  
        imgB,  
        pyrA,  
        pyrB,  
        cornersA,  
        cornersB,  
        corner_count,  
        cvSize( win_size,win_size ),  
        5,  
        features_found,  
        feature_errors,  
        cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 20, .3 ),  
        0  
    );  
  // Now make some image of what we are looking at:  
  //  
  for( int i=0; i<corner_count; i++ ) {
     if( features_found[i]==0|| feature_errors[i]>550 ) {  
 //       printf("Error is %f/n",feature_errors[i]);  
        continue;  
     }  
 //    printf("Got it/n");  
     CvPoint p0 = cvPoint(  
        cvRound( cornersA[i].x ),  
        cvRound( cornersA[i].y )  
     );  
     CvPoint p1 = cvPoint(  
        cvRound( cornersB[i].x ),  
        cvRound( cornersB[i].y )  
     );  
     cvLine( imgC, p0, p1, CV_RGB(255,0,0),2 );  
  }  
  //cvNamedWindow("ImageA",0);  
  //cvNamedWindow("ImageB",0);  
  cvNamedWindow("LKpyr_OpticalFlow",1);  
  //cvShowImage("ImageA",imgA);  
  //cvShowImage("ImageB",imgB);  
  cvShowImage("LKpyr_OpticalFlow",imgC);  
//  return imgC;
  //cvWaitKey(0);  
}*/

/*Eyes detection*/
/*void detectEyes( IplImage *img,CvRect *r)
{
    char *eyecascade;
    CvSeq *eyes;
    int eye_detect=0;
    

   //eye detection starts
   //Set the Region of Interest: estimate the eyes' position 
    
    cvSetImageROI(img,                    // the source image  
          cvRect
          (
              r->x,            // x = start from leftmost 
              r->y + (r->height/5.5), // y = a few pixels from the top 
              r->width,        // width = same width with the face 
              r->height/3.0    // height = 1/3 of face height 
          )
      );

      // detect the eyes 
      eyes = cvHaarDetectObjects( img,            // the source image, with the estimated location defined 
                                  cascade_e,      // the eye classifier 
                                  storage,        // memory buffer 
                                  1.15, 3, 0,     // tune for your app 
                                  cvSize(25, 15)  // minimum detection scale 
                                );
    
      printf("\no of eyes detected are %d",eyes->total);
    
      
        // draw a rectangle for each detected eye 
        for( int i = 0; i < (eyes ? eyes->total : 0); i++ )
          {
              eye_detect++;
              // get one eye 
              CvRect *eye = (CvRect*)cvGetSeqElem(eyes, i);
              // draw a red rectangle 
                        cvRectangle(img, 
                                    cvPoint(eye->x, eye->y), 
                                    cvPoint(eye->x + eye->width, eye->y + eye->height),
                                    CV_RGB(0, 0, 255), 
                                    1, 8, 0
                                   );
           }            
}*/

//void detectFacialFeatures( IplImage *img,IplImage *temp_img/*,int img_no*/)
/*{
    
    //char image[100],msg[100],temp_image[100];
    float m[6];
    double factor = 1;
    CvMat M;
    int w = (img)->width;
    int h = (img)->height;
    CvSeq* faces;
    CvRect *r;

    m[0] = (float)(factor*cos(0.0));
    m[1] = (float)(factor*sin(0.0));
    m[2] = w*0.5f;
    m[3] = -m[1];
    m[4] = m[0];
    m[5] = h*0.5f;
     
    M = cvMat( 2, 3, CV_32F, m );

    cvGetQuadrangleSubPix(img, temp_img, &M);
    CvMemStorage* storage=cvCreateMemStorage(0);
    cvClearMemStorage( storage );
    
    if( cascade )
        faces = cvHaarDetectObjects(img,cascade, storage, 1.2, 2, CV_HAAR_DO_CANNY_PRUNING, cvSize(20, 20));
    else
        printf("\nFrontal face cascade not loaded\n");

    printf("\n no of faces detected are %d\n",faces->total);
    //printf("Checkpoint 2\n");
    
    cvNamedWindow("result",CV_WINDOW_AUTOSIZE);
   // for each face found, draw a red box 
   
    for(int i = 0 ; i < ( faces ? faces->total : 0 ) ; i++ )
    {        
        r = ( CvRect* )cvGetSeqElem( faces, i );
        cvRectangle(img,cvPoint( r->x, r->y ),cvPoint( r->x + r->width, r->y + r->height ), CV_RGB(255, 0, 0), 1, 8, 0 );
        printf("\n face_x=%d face_y=%d wd=%d ht=%d",r->x,r->y,r->width,r->height);
        
            CvPoint center;
            int radius;
            center.x = cvRound(r->x + r->width*0.5);
            center.y = cvRound(r->y + r->height*0.5);
            radius = cvRound((r->width + r->height)*0.25);
            //radius = cvRound(r->width*0.5);
        //detectEyes(img,r);
        // reset region of interest 
        //cvResetImageROI(img);
        //detectNose(img,r);
        //cvResetImageROI(img);
        //detectMouth(img,r);
        //cvResetImageROI(img);
    }
    cvShowImage("result",img);
    //printf("Checkpoint 3\n");
    // reset region of interest 
      //cvResetImageROI(img);

      //if(faces->total>0)
      //  {
            //sprintf(image,"D:\\face_output\\%d.jpg",img_no);
       //     cvSaveImage( image, img );
       // }
      //printf("Checkpoint 4\n");
}*/
