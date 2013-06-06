#ifndef transformImage_h_
#define transformImage_h_


#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

// itk
#include "itkAffineTransform.h"

namespace afibReg
{
  /**
   * AFFINE Transform the image. 
   * 
   * interpolationType = 0 for NN interp, 1 for linear interp, 2 for
   * bspline interp. Default is 1
   */
  template<typename InputImageType, typename OutputImageType>
  typename OutputImageType::Pointer
  transformImage(typename itk::AffineTransform<double, InputImageType::ImageDimension>::Pointer transform, \
                 typename InputImageType::Pointer inputImage, typename OutputImageType::Pointer referenceImage, \
                 typename OutputImageType::PixelType fillInValue, char interpolationType = 1);



  /**
   * Warp image using vector image
   * 
   * interpolationType = 0 for NN interp, 1 for linear interp, 2 for
   * bspline interp. Default is 1
   */
  template<typename InputImageType, typename OutputImageType, typename DeformationFieldType>
  typename OutputImageType::Pointer
  warpImage(typename DeformationFieldType::Pointer vectorField, \
            typename InputImageType::Pointer inputImage, typename OutputImageType::Pointer referenceImage, \
            typename OutputImageType::PixelType fillInValue, char interpolationType = 1);


}// afibReg

#include "transformImage.hxx"


#endif //transformImage_h_
