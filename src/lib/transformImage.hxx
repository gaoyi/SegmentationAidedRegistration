#ifndef transformImage_hxx_
#define transformImage_hxx_


#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

// itk
#include "itkBSplineInterpolateImageFunction.h"
#include "itkImage.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkTransform.h"
#include "itkWarpImageFilter.h"

// local
#include "transformImage.h"

namespace afibReg
{
  template<typename InputImageType, typename OutputImageType>
  typename OutputImageType::Pointer
  transformImage(typename itk::AffineTransform<double, InputImageType::ImageDimension>::Pointer transform, \
                 typename InputImageType::Pointer inputImage, typename OutputImageType::Pointer referenceImage, \
                 typename OutputImageType::PixelType fillInValue, char interpolationType)
  {
    typedef double CoordinateRepresentationType ;

    typename itk::InterpolateImageFunction<InputImageType, CoordinateRepresentationType>::Pointer interpolator;

    if (interpolationType == 0)
      {
        // NN interpolation
        typedef itk::NearestNeighborInterpolateImageFunction<InputImageType, CoordinateRepresentationType> InterpolatorType;
        interpolator = InterpolatorType::New();
      }
    else if(interpolationType == 1)
      {
        // linear 
        typedef itk::LinearInterpolateImageFunction<InputImageType, CoordinateRepresentationType> InterpolatorType;
        interpolator = InterpolatorType::New();
      }
    else if(interpolationType == 2)
      {
        // bspline
        typedef double CoefficientType;
        typedef itk::BSplineInterpolateImageFunction<InputImageType, CoordinateRepresentationType, CoefficientType> InterpolatorType;
        interpolator = InterpolatorType::New();
      }


    typedef itk::ResampleImageFilter< InputImageType, OutputImageType > ResampleFilterType;
    typename ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    resampler->SetTransform( transform );
    resampler->SetInput( inputImage );
    resampler->SetSize( referenceImage->GetLargestPossibleRegion().GetSize() );
    resampler->SetOutputOrigin(  referenceImage->GetOrigin() );
    resampler->SetOutputSpacing( referenceImage->GetSpacing() );
    resampler->SetOutputDirection( referenceImage->GetDirection() );
    resampler->SetDefaultPixelValue( fillInValue );
    resampler->SetInterpolator(  interpolator  );
    resampler->Update();
  
    return resampler->GetOutput();
  }



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
            typename OutputImageType::PixelType fillInValue, char interpolationType)
  {
    typename itk::InterpolateImageFunction<InputImageType, double>::Pointer interpolator;

    if (interpolationType == 0)
      {
        // NN interpolation
        typedef itk::NearestNeighborInterpolateImageFunction<InputImageType, double> InterpolatorType;
        interpolator = InterpolatorType::New();
      }
    else if(interpolationType == 1)
      {
        // linear 
        typedef itk::LinearInterpolateImageFunction<InputImageType, double> InterpolatorType;
        interpolator = InterpolatorType::New();
      }
    else if(interpolationType == 2)
      {
        // bspline
        typedef itk::BSplineInterpolateImageFunction<InputImageType, double, double> InterpolatorType;
        interpolator = InterpolatorType::New();
      }

    typedef itk::WarpImageFilter<InputImageType, OutputImageType, DeformationFieldType > WarperType;
    typename WarperType::Pointer warper = WarperType::New();
    warper->SetInput( inputImage );
    warper->SetInterpolator( interpolator );
    warper->SetOutputSpacing( referenceImage->GetSpacing() );
    warper->SetOutputOrigin( referenceImage->GetOrigin() );
    warper->SetDeformationField( vectorField );
    warper->Update();

    //tst
    //writeVectorImage<DeformationFieldType>(filter->GetOutput(), "defomationField.nrrd", 2);
    //tst//

    // InternalImageType::Pointer movingImgInternal = castItkImage<typename moving_image_t::PixelType, typename OutputImageType::PixelType>(movingImg);


    // typedef itk::CastImageFilter< InternalImageType, output_image_t > CastFilterType;
    // typename CastFilterType::Pointer  caster =  CastFilterType::New();
    // caster->SetInput( warper->GetOutput() );
    // caster->Update();

    return warper->GetOutput();

  }


}// afibReg



#endif //transformImage_hxx_
