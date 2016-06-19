/*
By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.


                          License Agreement
               For Open Source Computer Vision Library
                       (3-clause BSD License)

Copyright (C) 2016, OpenCV Foundation, all rights reserved.
Third party copyrights are property of their respective owners.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the names of the copyright holders nor the names of the contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

This software is provided by the copyright holders and contributors "as is" and
any express or implied warranties, including, but not limited to, the implied
warranties of merchantability and fitness for a particular purpose are
disclaimed. In no event shall copyright holders or contributors be liable for
any direct, indirect, incidental, special, exemplary, or consequential damages
(including, but not limited to, procurement of substitute goods or services;
loss of use, data, or profits; or business interruption) however caused
and on any theory of liability, whether in contract, strict liability,
or tort (including negligence or otherwise) arising in any way out of
the use of this software, even if advised of the possibility of such damage.
*/

/*
Implementation of the PCAFlow algorithm from the following paper:
http://files.is.tue.mpg.de/black/papers/cvpr2015_pcaflow.pdf

@inproceedings{Wulff:CVPR:2015,
  title = {Efficient Sparse-to-Dense Optical Flow Estimation using a Learned Basis and Layers},
  author = {Wulff, Jonas and Black, Michael J.},
  booktitle = { IEEE Conf. on Computer Vision and Pattern Recognition (CVPR) 2015},
  month = jun,
  year = {2015}
}
*/

#ifndef __OPENCV_OPTFLOW_PCAFLOW_HPP__
#define __OPENCV_OPTFLOW_PCAFLOW_HPP__

#include "opencv2/core.hpp"
#include "opencv2/video.hpp"

namespace cv
{
namespace optflow
{

/*
 * This class can be used for imposing a learned prior on the resulting optical flow.
 * Solution will be regularized according to this prior.
 * You need to generate appropriate prior file with "learn_prior.py" script beforehand.
 */
class CV_EXPORTS_W PCAPrior
{
private:
  Mat L1;
  Mat L2;
  Mat c1;
  Mat c2;

public:
  PCAPrior( const char *pathToPrior );

  int getPadding() const { return L1.size().height; }

  int getBasisSize() const { return L1.size().width; }

  void fillConstraints( float *A1, float *A2, float *b1, float *b2 ) const;
};

class CV_EXPORTS_W OpticalFlowPCAFlow : public DenseOpticalFlow
{
protected:
  const Ptr<const PCAPrior> prior;
  const Size basisSize;
  const float sparseRate;              // (0 .. 0.1)
  const float retainedCornersFraction; // [0 .. 1]
  const float occlusionsThreshold;
  const float dampingFactor;

public:
  OpticalFlowPCAFlow( Ptr<const PCAPrior> _prior = Ptr<const PCAPrior>(), const Size _basisSize = Size( 18, 14 ),
                      float _sparseRate = 0.024, float _retainedCornersFraction = 0.2,
                      float _occlusionsThreshold = 0.0003, float _dampingFactor = 0.00002 );

  void calc( InputArray I0, InputArray I1, InputOutputArray flow );
  void collectGarbage();

private:
  void findSparseFeatures( UMat &from, UMat &to, std::vector<Point2f> &features,
                           std::vector<Point2f> &predictedFeatures ) const;

  void removeOcclusions( UMat &from, UMat &to, std::vector<Point2f> &features,
                         std::vector<Point2f> &predictedFeatures ) const;

  void getSystem( OutputArray AOut, OutputArray b1Out, OutputArray b2Out, const std::vector<Point2f> &features,
                  const std::vector<Point2f> &predictedFeatures, const Size size );

  void getSystem( OutputArray A1Out, OutputArray A2Out, OutputArray b1Out, OutputArray b2Out,
                  const std::vector<Point2f> &features, const std::vector<Point2f> &predictedFeatures,
                  const Size size );
};

CV_EXPORTS_W Ptr<DenseOpticalFlow> createOptFlow_PCAFlow();
}
}

#endif
