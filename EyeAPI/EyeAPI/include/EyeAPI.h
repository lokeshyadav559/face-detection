/*
Copyright University of Amsterdam, 2008. All rights reserved.

Contact person:
Roberto Valenti (rvalenti@science.uva.nl, http://www.science.uva.nl/~rvalenti)
Intelligent Systems Lab Amsterdam
Informatics Institute, Faculty of Science, University of Amsterdam
Kruislaan 403, 1098 SJ Amsterdam, The Netherlands.

This software is being made available for individual research use only.
Any commercial use or redistribution of this software requires a license from
the University of Amsterdam.

You may use this work subject to the following conditions:

1. This work is provided "as is" by the copyright holder, with
absolutely no warranties of correctness, fitness, intellectual property
ownership, or anything else whatsoever.  You use the work
entirely at your own risk.  The copyright holder will not be liable for
any legal damages whatsoever connected with the use of this work.

2. The copyright holder retain all copyright to the work. All copies of
the work and all works derived from it must contain (1) this copyright
notice, and (2) additional notices describing the content, dates and
copyright holder of modifications or additions made to the work, if
any, including distribution and use conditions and intellectual property
claims.  Derived works must be clearly distinguished from the original
work, both by name and by the prominent inclusion of explicit
descriptions of overlaps and differences.

3. The names and trademarks of the copyright holder may not be used in
advertising or publicity related to this work without specific prior
written permission. 

4. In return for the free use of this work, you are requested, but not
legally required, to do the following:

- If you become aware of factors that may significantly affect other
  users of the work, for example major bugs or
  deficiencies or possible intellectual property issues, you are
  requested to report them to the copyright holder, if possible
  including redistributable fixes or workarounds.

- If you use the work in scientific research or as part of a larger
  software system, you are requested to cite the use in any related
  publications or technical documentation. The work is based upon:

    R. Valenti and T. Gevers. Accurate eye center location and tracking using isophote curvature.
	In IEEE Conference on Computer Vision and Pattern Recognition, June 2008.
	For additional information refer to http://staff.science.uva.nl/~rvalenti/index.php?content=publications
*/

#pragma once
#include "cv.h"
#include "highgui.h"

#include <vector>

using namespace std;

#ifndef MAXWIDTH
	enum {MEMORY=-1,CAMERA=0, MOVIE=1,PICTURE=2}; //input
	enum {PLAY=0,PAUSE=1,STOP=2,RECORD=3}; //player
	//resolution
	#define MAXWIDTH 2000
	#define MAXHEIGHT 2000
#endif

#ifdef EYEAPI_EXPORTS
#define EYE_API __declspec(dllexport)
#define EXTERN
#else
#define EYE_API __declspec(dllimport)
#define EXTERN extern
#endif

class  EyeAPI{
public:
	virtual bool initialized()=0;
	/*Call to process each frame in a video/camera sequence (call before using getters)*/
	virtual bool detect(bool display=false,bool upscale=false)=0;
	/*Call to process a specific image when the position of the face is known (call before using getters)*/
	virtual bool detect(IplImage* image,CvRect* face)=0;
	/*Call to process a specific image when the position of the face is unknown (call before using getters)*/
	virtual bool detect(IplImage* image, bool display=false)=0;
	
	/*computes the centermap of a given image, "method" discards certain votes:
	-1 - only votes from dark to bright areas
	0 - consider all votes
	1 - only votes from bright to dark areas*/
	virtual IplImage* isocenters(IplImage *image, double sigma,double endsigma,int method=0,double colorweight=0)=0;

	/*Retrieves the found face region*/
	virtual CvRect* getFacePosition()=0;

	/*Retrieves the Maximum IsoCenter in the centermap for the left eye region*/
	virtual CvPoint getLeftMIC()=0;
	/*Retrieves the Maximum IsoCenter in the centermap for the right eye region*/
	virtual CvPoint getRightMIC()=0;

	/*Retrieves the MIC in the centermap closer to the center of the converged meanshift window for the left eye region*/
	virtual CvPoint getLeftEyePosition()=0;
	/*Retrieves the MIC in the centermap closer to the center of the converged meanshift window for the right eye region*/
	virtual CvPoint getRightEyePosition()=0;

	/*Retrieves the "centermap" for the left eye region*/
	virtual IplImage* getLeftIso()=0;
	/*Retrieves the "centermap" for the right eye region*/
	virtual IplImage* getRightIso()=0;

	/*Retrieves the coordinates and size of the left eye region*/
	virtual CvRect* getLeftROI()=0;
	/*Retrieves the coordinates and size of the right eye region*/
	virtual CvRect* getRightROI()=0;

	/*Returns the currently processed image, useful for display purposes*/
	virtual IplImage* getSourceImage()=0;

	/*Returns the list of the peaks in the given centermap*/
	virtual vector<CvPoint> getMaximas(IplImage* isocenters,CvRect* ROI,double threshold=3)=0;

	/*Retrieves the list of highest peaks on the left eye region*/
	virtual vector<CvPoint> getLeftMaximas()=0;
	/*Retrieves the list of highest peaks on the right eye region*/
	virtual vector<CvPoint> getRightMaximas()=0;

	/*Computes the closest point to the given "point" in the "pointvector"*/
	virtual CvPoint getNearest(CvPoint point,vector<CvPoint> pointvector)=0;

	/*Player Controls*/
	virtual void record()=0;
	virtual void play()=0;
	virtual void pause()=0;
	virtual void stop()=0;
	virtual void writeFrame(IplImage* frame)=0;

	/*saliency routines*/
	virtual IplImage* doCOI(int method=0)=0;
	virtual IplImage* doMultiCOI(int method=0)=0;
	virtual IplImage* getIsocenters()=0;/*Retrieves the "centermap" for image*/
	
	/*Computes the left and right eyecorners*/
	virtual vector<CvPoint> getCorners(CvPoint lefteye, CvPoint righteye)=0; 

	virtual ~EyeAPI(){};

	virtual void setSigma(int sigma)=0;
	virtual void setEndSigma(int endsigma)=0;


};
#if WIN32 
extern "C" EYE_API EyeAPI* APIENTRY GetEyeAPI(int mode=CAMERA,char* filename="",double sigma=2, double endsigma=18);
#endif
