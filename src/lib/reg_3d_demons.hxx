#ifndef reg_3d_demons_txx_
#define reg_3d_demons_txx_

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


//itk
#include "itkImageRegionIterator.h"
#include "itkDemonsRegistrationFilter.h"
//#include "itkCastImageFilter.h"

#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkBSplineInterpolateImageFunction.h"

#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"


//local
#include "reg_3d_demons.h"
#include "utilities.h"

namespace afibReg
{
  template<typename fix_image_t, typename moving_image_t, typename output_image_t>
  typename itk::Image< itk::Vector< float, fix_image_t::ImageDimension >, fix_image_t::ImageDimension >::Pointer
  reg_3d_demons(typename fix_image_t::Pointer fixImg,                   \
                typename moving_image_t::Pointer movingImg,             \
                typename moving_image_t::PixelType fillInVal)
  {
    const unsigned int Dimension = fix_image_t::ImageDimension;

    typedef float InternalPixelType;
    typedef itk::Image< InternalPixelType, Dimension > InternalImageType;

    typename InternalImageType::Pointer fixImgInternal = castItkImage<typename fix_image_t::PixelType, InternalPixelType>(fixImg);
    typename InternalImageType::Pointer movingImgInternal = castItkImage<typename moving_image_t::PixelType, InternalPixelType>(movingImg);

    typedef itk::Vector< float, Dimension >    VectorPixelType;
    typedef itk::Image<  VectorPixelType, Dimension > DeformationFieldType;
    typedef itk::DemonsRegistrationFilter<InternalImageType, InternalImageType, DeformationFieldType>   RegistrationFilterType;
    typename RegistrationFilterType::Pointer filter = RegistrationFilterType::New();

    filter->SetFixedImage( fixImgInternal );
    filter->SetMovingImage( movingImgInternal );

    filter->SetNumberOfIterations( 100 );
    filter->SetStandardDeviations( 1.0 );

    filter->Update();

    return filter->GetOutput();
  }

}


#endif
