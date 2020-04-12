#pragma once
#include "stdafx.h"
#include "EyeAPI.h"

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


int main(int argc, char* argv[]){
	IplImage* image;
	CvRect* face;
	CvPoint left, right;
	
	//pragmatically set the current working directory from "build" to "bin"
	//this can be removed if "project settings->debugging->working directory" is set to $(ProjectDir)/../bin/
	char* cwd=new char[255];
	sprintf(cwd,"%s\\..\\bin\\",DIR);
	SetCurrentDirectory(cwd);
	delete[] cwd;

	//initialize eye tracker
	int input=CAMERA;
	char* filename="camera";
	if (argc>1){
		if(*argv[1]=='n') input= MEMORY;
		if(*argv[1]=='c') input= CAMERA;
		if(*argv[1]=='m') {input= MOVIE; filename=argv[2];}
		if(*argv[1]=='p') {input= PICTURE;filename=argv[2];}
	}
	EyeAPI *tracker= GetEyeAPI(input,filename);
	
	for(;;){
		//always call the tracker at least once
		if(!tracker->detect(true))//set to true to show the internal window
			continue; 
		
		//display left and right eye positions on cmd
		left=tracker->getLeftEyePosition();
		right=tracker->getRightEyePosition();
		printf("%d %d - ",left.x,left.y);
		printf("%d %d\n",right.x,right.y);
		
		//display the face position on the original image
		face=tracker->getFacePosition();
		
		//retrieve the current image
		image=tracker->getSourceImage();
		//display the eye points on the image
		cvCircle(image,left,3,cvScalar(0,255,0));
		cvCircle(image,right,3,cvScalar(0,255,0));
		//display the face region
		cvDrawRect(image,cvPoint(face->x,face->y),cvPoint(face->x+face->width,face->y+face->height),cvScalar(0,0,255));
		
		//display the results
		cvNamedWindow("Results");
		cvShowImage("Results", image);
		char c=cvWaitKey(1);
		//player commands
		switch(c){
			case 'q':
				delete tracker;
				ExitProcess(0);
				break;
			case 'p':
				tracker->play();
				break;
			case 's':
				tracker->stop();
				break;

		}
	}
}

