#ifndef reg_3d_demons_h_
#define reg_3d_demons_h_

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


namespace gth818n
{
  template<typename fix_image_t, typename moving_image_t>
  typename itk::Image< itk::Vector< float, fix_image_t::ImageDimension >, fix_image_t::ImageDimension >::Pointer
  reg_3d_demons(typename fix_image_t::Pointer fixImg,                     \
                typename moving_image_t::Pointer movingImg);
}


#include "reg_3d_demons.hxx"

#endif
