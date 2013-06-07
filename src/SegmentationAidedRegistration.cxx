//std
#include <string>

//local
#include "reg_3d_affine_mse.h"
#include "reg_3d_demons.h"
#include "transformImage.h"
#include "utilities.h"

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
  ImageType::Pointer postMRI = gth818n::readImage<ImageType>(postMRIFileName.c_str());
  ImageType::Pointer preMRI = gth818n::readImage<ImageType>(preMRIFileName.c_str());

  ImageType::Pointer postEndoTmp = gth818n::readImage<ImageType>(postEndoFileName.c_str());
  ImageType::Pointer preEndoTmp = gth818n::readImage<ImageType>(preEndoFileName.c_str());

  ImageType::Pointer postEndo = gth818n::cropROIFromImage<ImageType>(postEndoTmp);
  ImageType::Pointer preEndo = gth818n::cropROIFromImage<ImageType>(preEndoTmp);

  // affine register
  typedef itk::AffineTransform<double, Dimension> AffineTransformType;
  double finalRegCost = 0.0;
  AffineTransformType::Pointer trans = gth818n::affineMSERegistration<ImageType, ImageType>(preEndo, postEndo, finalRegCost);

  double fillInValue = 0.0;
  //ImageType::Pointer postToPreEndoAffine = gth818n::transformImage<ImageType, ImageType>(trans, postEndo, preEndo, fillInValue);
  ImageType::Pointer postToPreMRIAffine = gth818n::transformImage<ImageType, ImageType>(trans, postMRI, preMRI, fillInValue);


  // // demons register
  // typedef itk::Image< itk::Vector< float, ImageType::ImageDimension >, ImageType::ImageDimension > DisplacementFieldType;
  // DisplacementFieldType::Pointer demonsField = gth818n::reg_3d_demons<ImageType, ImageType>(preEndo, postToPreEndoAffine);

  // ImageType::Pointer postToPreMRIDemons                                 \
  //   = gth818n::warpImage<ImageType, ImageType, DisplacementFieldType>(demonsField, postToPreMRIAffine, preMRI, fillInValue);


  // output
  //gth818n::writeImage<ImageType>(postToPreMRIDemons, postToPreMRIFileName.c_str());
  gth818n::writeImage<ImageType>(postToPreMRIAffine, postToPreMRIFileName.c_str());


  return 0;
}
