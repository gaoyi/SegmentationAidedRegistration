#ifndef reg_3d_affine_mse_h_
#define reg_3d_affine_mse_h_

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include "itkAffineTransform.h"

namespace afibReg
{
  template<typename fix_image_t, typename moving_image_t>
  itk::AffineTransform<double, 3>::Pointer
  affineMSERegistration(typename fix_image_t::Pointer fixedImage, typename moving_image_t::Pointer movingImage, double& finalCost);

}

#include "reg_3d_affine_mse.hxx"

#endif
