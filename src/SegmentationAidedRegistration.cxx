//std
#include <string>

//local
#include "reg_3d_affine_mse.h"
#include "reg_3d_demons.h"
#include "transformImage.h"
#include "utilities.h"
#include "itkImageRegionIterator.h"


// by slicer
#include "SegmentationAidedRegistrationCLP.h"

int main(int argc, char** argv)
{
  PARSE_ARGS;

  typedef float PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image<PixelType, Dimension> ImageType;

  typedef unsigned char LabelPixelType;
  typedef itk::Image<LabelPixelType, Dimension> LabelImageType;

  // Read in images
  ImageType::Pointer postMRIFull = gth818n::readImage<ImageType>(postMRIFileName.c_str());
  ImageType::Pointer preMRIFull = gth818n::readImage<ImageType>(preMRIFileName.c_str());

  ImageType::Pointer postEndoFull = gth818n::readImage<ImageType>(postEndoFileName.c_str());
  ImageType::Pointer preEndoFull = gth818n::readImage<ImageType>(preEndoFileName.c_str());

  if (preMRIFull->GetLargestPossibleRegion() != preEndoFull->GetLargestPossibleRegion() )
    {
      std::cerr<<"Error: 1st grayscale-binary pair should have the same region.\n";
    }

  if (postMRIFull->GetLargestPossibleRegion() != postEndoFull->GetLargestPossibleRegion() )
    {
      std::cerr<<"Error: 2nd grayscale-binary pair should have the same region.\n";
    }


  ImageType::Pointer postEndo = gth818n::cropROIFromImage<ImageType>(postEndoFull);
  ImageType::Pointer preEndo = gth818n::cropROIFromImage<ImageType>(preEndoFull);

  // affine register
  typedef itk::AffineTransform<double, Dimension> AffineTransformType;
  double finalRegCost = 0.0;
  AffineTransformType::Pointer trans = gth818n::affineMSERegistration<ImageType, ImageType>(preEndo, postEndo, finalRegCost);

  double fillInValue = 0.0;

  // demons register
  typedef itk::Vector< float, ImageType::ImageDimension > VectorType;
  typedef itk::Image< VectorType, ImageType::ImageDimension > DisplacementFieldType;

  if (!deformableRegistrationLocally)
    {
      ImageType::Pointer postToPreEndoAffineFull = gth818n::transformImage<ImageType, ImageType>(trans, postEndoFull, preEndoFull, fillInValue);
      ImageType::Pointer postToPreMRIAffineFull = gth818n::transformImage<ImageType, ImageType>(trans, postMRIFull, preMRIFull, fillInValue);

      // deformable registration on the entire volumes
      DisplacementFieldType::Pointer demonsField = gth818n::reg_3d_demons<ImageType, ImageType>(preEndoFull, postToPreEndoAffineFull);

      ImageType::Pointer postToPreMRIDemonsFull                                 \
        = gth818n::warpImage<ImageType, ImageType, DisplacementFieldType>(demonsField, postToPreMRIAffineFull, preMRIFull, fillInValue);

      gth818n::writeImage<ImageType>(postToPreMRIDemonsFull, postToPreMRIFileName.c_str());
    }
  else
    {
      ImageType::Pointer postToPreEndoAffine = gth818n::transformImage<ImageType, ImageType>(trans, postEndo, preEndo, fillInValue);
      ImageType::Pointer postToPreMRIAffine = gth818n::transformImage<ImageType, ImageType>(trans, postMRIFull, preEndo, fillInValue);

      // deformable registration only around the target regions

      DisplacementFieldType::Pointer demonsField = gth818n::reg_3d_demons<ImageType, ImageType>(preEndo, postToPreEndoAffine);

      ImageType::Pointer postToPreMRIDemons                                 \
        = gth818n::warpImage<ImageType, ImageType, DisplacementFieldType>(demonsField, postToPreMRIAffine, preEndo, fillInValue);

      gth818n::writeImage<ImageType>(postToPreMRIDemons, postToPreMRIFileName.c_str());
    }

  return 0;
}
