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

  // if (argc < 7)
  //   {
  //     std::cerr<<"args: postMRI preMRI postEndo preEndo postToPreMRI postToPreEndo\n";
  //     exit(-1);
  //   }

  // std::string postMRIFileName(argv[1]);
  // std::string preMRIFileName(argv[2]);
  // std::string postEndoFileName(argv[3]);
  // std::string preEndoFileName(argv[4]);
  // std::string postToPreMRIFileName(argv[5]);
  // std::string postToPreEndoFileName(argv[6]);

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
  //ImageType::Pointer postToPreEndoAffineFull = gth818n::transformImage<ImageType, ImageType>(trans, postEndoFull, preEndoFull, fillInValue);
  ImageType::Pointer postToPreMRIAffineFull = gth818n::transformImage<ImageType, ImageType>(trans, postMRIFull, preMRIFull, fillInValue);

  ImageType::Pointer postToPreEndoAffine = gth818n::transformImage<ImageType, ImageType>(trans, postEndo, preEndo, fillInValue);
  ImageType::Pointer postToPreMRIAffine = gth818n::transformImage<ImageType, ImageType>(trans, postMRIFull, preEndo, fillInValue);

  // demons register
  typedef itk::Vector< float, ImageType::ImageDimension > VectorType;
  typedef itk::Image< VectorType, ImageType::ImageDimension > DisplacementFieldType;
  DisplacementFieldType::Pointer demonsField = gth818n::reg_3d_demons<ImageType, ImageType>(preEndo, postToPreEndoAffine);

  ImageType::Pointer postToPreMRIDemons                                 \
    = gth818n::warpImage<ImageType, ImageType, DisplacementFieldType>(demonsField, postToPreMRIAffine, preEndo, fillInValue);

  // // crop affine registered images
  // typename ImageType::RegionType ROIRegion = gth818n::computeNonZeroRegion<ImageType>(postToPreEndoAffineFull);
  // typename ImageType::RegionType enlargedROIRegion = gth818n::enlargeNonZeroRegion<ImageType>(postToPreEndoAffineFull, ROIRegion);
  // typename ImageType::Pointer postToPreEndoAffineFullCropped = gth818n::extractROI<ImageType>(postToPreEndoAffineFull, enlargedROIRegion);


  // // demons register
  // typedef itk::Vector< float, ImageType::ImageDimension > VectorType;
  // typedef itk::Image< VectorType, ImageType::ImageDimension > DisplacementFieldType;
  // DisplacementFieldType::Pointer demonsField = gth818n::reg_3d_demons<ImageType, ImageType>(preEndo, postToPreEndoAffineFullCropped);

  // DisplacementFieldType::Pointer demonsFieldFull = DisplacementFieldType::New();
  // demonsFieldFull->SetRegions(postToPreEndoAffineFull->GetLargestPossibleRegion() );
  // demonsFieldFull->Allocate();
  // demonsFieldFull->SetSpacing(postToPreEndoAffineFull->GetSpacing());
  // VectorType v;
  // v.Fill(0);
  // demonsFieldFull->FillBuffer(v);
  // demonsFieldFull->CopyInformation(postToPreEndoAffineFull);

  // {
  //   typedef itk::ImageRegionIterator< DisplacementFieldType > itkImageRegionIterator_t;
  //   //typedef itk::ImageRegionConstIterator< DisplacementFieldType > itkImageRegionConstIterator_t;

  //   {
  //     itkImageRegionIterator_t itROI(demonsField, demonsField->GetLargestPossibleRegion());
  //     itkImageRegionIterator_t itNew(demonsFieldFull, enlargedROIRegion);

  //     itROI.GoToBegin();
  //     itNew.GoToBegin();
  //     for (; !itROI.IsAtEnd(); ++itROI, ++itNew)
  //       {
  //         itNew.Set(itROI.Get());
  //       }
  //   }
  // }

  // ImageType::Pointer postToPreMRIDemons                                 \
  //   = gth818n::warpImage<ImageType, ImageType, DisplacementFieldType>(demonsFieldFull, postToPreMRIAffineFull, preMRIFull, fillInValue);


  // output
  gth818n::writeImage<ImageType>(postToPreMRIDemons, postToPreMRIFileName.c_str());
  //gth818n::writeImage<ImageType>(postToPreMRIAffine, postToPreMRIFileName.c_str());


  return 0;
}
