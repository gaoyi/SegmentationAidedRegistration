#ifndef utilities_h_
#define utilities_h_

// itk
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkCastImageFilter.h"

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkBinaryThresholdImageFilter.h"

#include "itkMultiplyByConstantImageFilter.h"


#include <csignal>

namespace afibReg
{
  /**
   * readImage
   */
  template< typename itkImage_t >
  typename itkImage_t::Pointer readImage(const char *fileName);

  /**
   * writeImage
   */
  template< typename itkImage_t > void writeImage(typename itkImage_t::Pointer img, const char *fileName);

  /**
   * Cast image pixel type
   */
  template< typename inputPixel_t, typename outputPixel_t > 
  typename itk::Image<outputPixel_t, 3 >::Pointer
  castItkImage( typename itk::Image<inputPixel_t, 3>::Pointer inputImage );


  /**
   * Read a series of images.
   */
  template< typename itkImage_t > 
  std::vector< typename itkImage_t::Pointer >
  readImageSeries( const std::vector< std::string >& imageNameList );

  /*============================================================
   * readTextLineToListOfString   
   */
  template<typename TNull>
  std::vector< std::string > readTextLineToListOfString(const char* textFileName);

  template<typename image_t>
  double getVol(typename image_t::Pointer img, typename image_t::PixelType thld = 0);

  /**
   * binarilize image
   */
  template<typename input_image_t, typename output_image_t>
  typename output_image_t::Pointer
  binarilizeImage(typename input_image_t::Pointer input,                 \
                  typename input_image_t::PixelType thld,                         \
                  typename output_image_t::PixelType insideValue = 1);


  template<typename input_image_t, typename output_image_t>
  typename output_image_t::Pointer
  binarilizeImage(typename input_image_t::Pointer input,                 \
                  typename input_image_t::PixelType lowerT,                         \
                  typename input_image_t::PixelType upperT, \
                  typename output_image_t::PixelType insideValue = 1,               \
                  typename output_image_t::PixelType outsideValue = 0);


  template<typename input_image_t, typename output_image_t>
  typename output_image_t::Pointer
  thld3(typename input_image_t::Pointer input,                            \
        typename input_image_t::PixelType lowerT,                         \
        typename input_image_t::PixelType upperT, \
        typename output_image_t::PixelType insideValue = 1,               \
        typename output_image_t::PixelType outsideValue = 0);


  /**
   * Post process probabiliry map: Multiply by 200, then convert to
   * uchar image. This will make the final result to be easier
   * thresholded by Slicer.
   */
  template<typename image_t>
  typename itk::Image<unsigned char, 3>::Pointer
  postProcessProbabilityMap(typename image_t::Pointer probMap, typename image_t::PixelType c);


  /**
   * write a component of a vector image
   */
  template< typename itkVectorImage_t > 
  void 
  writeVectorImage(typename itkVectorImage_t::Pointer img, const char *fileName, int component);


  /**
   * Compute the non-zero region of the image
   */
  template<typename image_t>
  typename image_t::RegionType
  computeNonZeroRegion(typename image_t::Pointer img);

  /**
   * Enlarge the non-zero region so that the region is not too tightly around the non-zero reigon
   */
  template<typename image_t>
  typename image_t::RegionType
  enlargeNonZeroRegion(typename image_t::Pointer img, typename image_t::RegionType nonZeroRegion);


  /**
   * Crop the mask by its non-zero region
   */
  template<typename MaskImageType >
  typename MaskImageType::Pointer
  cropROIFromImage(typename MaskImageType::Pointer mask);


  /**
   * Extract the ROI from the image using the region
   */
  template<typename image_t>
  typename image_t::Pointer
  extractROI(typename image_t::Pointer img, typename image_t::RegionType region);


}// afibReg


#include "utilities.hxx"

#endif
