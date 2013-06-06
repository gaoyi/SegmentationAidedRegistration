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
  ImageType::Pointer postMRI = afibReg::readImage<ImageType>(postMRIFileName.c_str());
  ImageType::Pointer preMRI = afibReg::readImage<ImageType>(preMRIFileName.c_str());

  ImageType::Pointer postEndo = afibReg::readImage<ImageType>(postEndoFileName.c_str());
  ImageType::Pointer preEndo = afibReg::readImage<ImageType>(preEndoFileName.c_str());


  // affine register
  typedef itk::AffineTransform<double, Dimension> AffineTransformType;
  double finalRegCost = 0.0;
  AffineTransformType::Pointer trans = afibReg::affineMSERegistration<ImageType, ImageType>(preEndo, postEndo, finalRegCost);

  double fillInValue = 0.0;
  //ImageType::Pointer postToPreEndo = afibReg::transformImage<ImageType, ImageType>(trans, postEndo, preEndo, fillInValue);
  ImageType::Pointer postToPreMRI = afibReg::transformImage<ImageType, ImageType>(trans, postMRI, preMRI, fillInValue);

  // output
  //afibReg::writeImage<ImageType>(postToPreEndo, postToPreEndoFileName.c_str());
  afibReg::writeImage<ImageType>(postToPreMRI, postToPreMRIFileName.c_str());


  return 0;
}
