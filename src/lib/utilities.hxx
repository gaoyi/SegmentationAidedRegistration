#ifndef utilities_hxx_
#define utilities_hxx_

#include <csignal>

// itk
#include "itkBinaryThresholdImageFilter.h"
#include "itkCastImageFilter.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"

#include "itkMultiplyByConstantImageFilter.h"

#include "itkRegionOfInterestImageFilter.h"

// local
#include "utilities.h"

namespace afibReg
{
  /**
   * readImage
   */
  template< typename itkImage_t >
  typename itkImage_t::Pointer readImage(const char *fileName)
  {
    typedef itk::ImageFileReader< itkImage_t > ImageReaderType;
    typename ImageReaderType::Pointer reader = ImageReaderType::New();
    reader->SetFileName(fileName);

    typename itkImage_t::Pointer image;
    
    try
      {
        reader->Update();
        image = reader->GetOutput();
      }
    catch ( itk::ExceptionObject &err)
      {
        std::cerr<< "ExceptionObject caught !" << std::endl; 
        std::cerr<< err << std::endl; 
        raise(SIGABRT);
      }

    return image;
  }


  /**
   * writeImage
   */
  template< typename itkImage_t > void writeImage(typename itkImage_t::Pointer img, const char *fileName)
  {
    typedef itk::ImageFileWriter< itkImage_t > WriterType;

    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( fileName );
    writer->SetInput(img);
    writer->UseCompressionOn();

    try
      {
        writer->Update();
      }
    catch ( itk::ExceptionObject &err)
      {
        std::cout << "ExceptionObject caught !" << std::endl; 
        std::cout << err << std::endl; 
        raise(SIGABRT);   
      }
  }


  /**
   * Read a series of images.
   */
  template< typename inputPixel_t, typename outputPixel_t > 
  typename itk::Image<outputPixel_t, 3 >::Pointer
  castItkImage( typename itk::Image<inputPixel_t, 3>::Pointer inputImage )
  {
    const unsigned int Dimension = 3;

    typedef itk::Image<inputPixel_t, Dimension> inputImage_t;
    typedef itk::Image<outputPixel_t, Dimension> outputImage_t;

    typedef itk::CastImageFilter< inputImage_t, outputImage_t > itkCastFilter_t;

    typename itkCastFilter_t::Pointer caster = itkCastFilter_t::New();
    caster->SetInput( inputImage );
    caster->Update();


    return caster->GetOutput();
  }


  /**
   * Read a series of images.
   */
  template< typename itkImage_t > 
  std::vector< typename itkImage_t::Pointer >
  readImageSeries( const std::vector< std::string >& imageNameList )
  {
    typedef typename itkImage_t::Pointer itkImagePointer_t;
    typedef std::vector< itkImagePointer_t > itkImageList_t;
    typedef itk::ImageFileReader< itkImage_t > itkImageReader_t;


    itkImageList_t imageSeries;
    
    int n = imageNameList.size();
    for (int i = 0; i < n; ++i)
      {
        std::string thisName = imageNameList[i];

        typename itkImageReader_t::Pointer reader = itkImageReader_t::New();
        reader->SetFileName(thisName);

        itkImagePointer_t img;

        try
          {
            reader->Update();
            img = reader->GetOutput();
          }
        catch ( itk::ExceptionObject &err)
          {
            std::cerr<< "ExceptionObject caught !" << std::endl; 
            std::cerr<< err << std::endl; 
            raise(SIGABRT);
          }


        imageSeries.push_back(img);
      }

    return imageSeries;
  }

  /*============================================================
   * readTextLineToListOfString   
   */
  template<typename TNull>
  std::vector< std::string > readTextLineToListOfString(const char* textFileName)
  {
    /* The file MUST end with an empty line, then each line will be
       stored as an element in the returned vector object. */


    // Here speed and space is not a big issue, so just let it copy and return stuff.
    std::vector< std::string > listOfStrings;

    std::ifstream f(textFileName);
    std::string thisLine;

    if (f.good())
      {
        while( std::getline(f, thisLine) )
          {
            listOfStrings.push_back(thisLine);
          }
      }
    else
      {
        std::cerr<<"Error: can not open file:"<<textFileName<<std::endl;
        raise(SIGABRT);
      }

    f.close();

    return listOfStrings;
  }


  template<typename image_t>
  double getVol(typename image_t::Pointer img, typename image_t::PixelType thld = 0)
  {
    typedef itk::ImageRegionConstIterator<image_t> ImageRegionConstIterator_t;
    ImageRegionConstIterator_t it(img, img->GetLargestPossibleRegion() );

    double cell = (img->GetSpacing()[0])*(img->GetSpacing()[1])*(img->GetSpacing()[2]);

    double v = 0.0;

    for (it.GoToBegin(); !it.IsAtEnd(); ++it)
      {
        typename image_t::PixelType f = it.Get();

        v += (f>thld?cell:0.0);
      }

    return v;
  }



  template<typename input_image_t, typename output_image_t>
  typename output_image_t::Pointer
  thld3(typename input_image_t::Pointer input,                            \
        typename input_image_t::PixelType lowerT,                         \
        typename input_image_t::PixelType upperT, \
        typename output_image_t::PixelType insideValue = 1,               \
        typename output_image_t::PixelType outsideValue = 0)
  {
    /**
     * O(x) :=    I(x) \in [lowerT, upperT] ? insideValue : outsideValue
     */

    //tst
    //   std::cout<<lowerT<<std::endl;
    //   std::cout<<upperT<<std::endl;
    //tst//

    typedef itk::BinaryThresholdImageFilter<input_image_t, output_image_t> binaryThresholdImageFilter_t;

    typename binaryThresholdImageFilter_t::Pointer thlder = binaryThresholdImageFilter_t::New();
    thlder->SetInput(input);
    thlder->SetInsideValue(insideValue);
    thlder->SetOutsideValue(outsideValue);
    thlder->SetUpperThreshold(upperT);
    thlder->SetLowerThreshold(lowerT);
    thlder->Update();
  
    return thlder->GetOutput();
  }



  /**
   * Post process probabiliry map: Multiply by 200, then convert to
   * uchar image. This will make the final result to be easier
   * thresholded by Slicer.
   */
  template<typename image_t>
  typename itk::Image<unsigned char, 3>::Pointer
  postProcessProbabilityMap(typename image_t::Pointer probMap, typename image_t::PixelType c)
  {
    typedef itk::MultiplyByConstantImageFilter< image_t, typename image_t::PixelType, itk::Image<unsigned char, 3> > \
      itkMultiplyByConstantImageFilter_t;

    typename itkMultiplyByConstantImageFilter_t::Pointer mul = itkMultiplyByConstantImageFilter_t::New();
    mul->SetInput(probMap);
    mul->SetConstant(c);
    mul->Update();
    
    return mul->GetOutput();
  }


  template<typename input_image_t, typename output_image_t>
  typename output_image_t::Pointer
  binarilizeImage(typename input_image_t::Pointer input,                 \
                  typename input_image_t::PixelType lowerT,                         \
                  typename input_image_t::PixelType upperT, \
                  typename output_image_t::PixelType insideValue = 1,               \
                  typename output_image_t::PixelType outsideValue = 0)
  {
    /**
     * O(x) :=    I(x) \in [lowerT, upperT] ? insideValue : outsideValue
     */

    //tst
    //   std::cout<<lowerT<<std::endl;
    //   std::cout<<upperT<<std::endl;
    //tst//

    typedef itk::BinaryThresholdImageFilter<input_image_t, output_image_t> binaryThresholdImageFilter_t;

    typename binaryThresholdImageFilter_t::Pointer thlder = binaryThresholdImageFilter_t::New();
    thlder->SetInput(input);
    thlder->SetInsideValue(insideValue);
    thlder->SetOutsideValue(outsideValue);
    thlder->SetUpperThreshold(upperT);
    thlder->SetLowerThreshold(lowerT);
    thlder->Update();
  
    return thlder->GetOutput();
  }


  template<typename input_image_t, typename output_image_t>
  typename output_image_t::Pointer
  binarilizeImage(typename input_image_t::Pointer input,                \
                  typename input_image_t::PixelType thld,               \
                  typename output_image_t::PixelType insideValue = 1)
  {
    typename input_image_t::PixelType lowerT = thld;
    typename input_image_t::PixelType upperT = static_cast<typename input_image_t::PixelType>(1e16);
    typename output_image_t::PixelType outsideValue = 0;

    return binarilizeImage<input_image_t, output_image_t>(input, lowerT, upperT, insideValue, outsideValue);
  }


  /**
   * write a component of a vector image
   */
  template< typename itkVectorImage_t > 
  void 
  writeVectorImage(typename itkVectorImage_t::Pointer img, const char *fileName, int component)
  {
    typedef itk::Image<double, itkVectorImage_t::ImageDimension> itkImage_t;
    typename itkImage_t::Pointer componentImg = itkImage_t::New();
    componentImg->SetRegions(img->GetLargestPossibleRegion() );
    componentImg->Allocate();


    typedef itk::ImageRegionIterator<itkVectorImage_t> VectorIteratorType;
    VectorIteratorType vIter(img, img->GetLargestPossibleRegion());

    typedef itk::ImageRegionIterator<itkImage_t> IteratorType;
    IteratorType iter(componentImg, componentImg->GetLargestPossibleRegion());

    for (vIter.GoToBegin(), iter.GoToBegin(); !vIter.IsAtEnd(); ++iter, ++vIter)
      {
        iter.Set(vIter.Get()[component]);
      }

    typedef itk::ImageFileWriter< itkImage_t > WriterType;

    typename WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( fileName );
    writer->SetInput(componentImg);
    writer->UseCompressionOn();

    try
      {
        writer->Update();
      }
    catch ( itk::ExceptionObject &err)
      {
        std::cout << "ExceptionObject caught !" << std::endl; 
        std::cout << err << std::endl; 
        abort();   
      }
  }


  template<typename image_t>
  typename image_t::RegionType
  computeNonZeroRegion(typename image_t::Pointer img)
  {
    /**
     * Given the img, compute the region where outside this region,
     * the image is all zero.
     *
     * The minx, y, z are initialized as sizeX, y, z; then, whenever
     * encounter an non-zero voxel, the minx, y, z are updated
     * accordingly. Similar for maxX, y, z except that they are
     * intialized to 0, 0, 0
     */
    typedef typename image_t::RegionType imageRegion_t;
    typedef typename image_t::IndexType imageIndex_t;
    typedef typename image_t::SizeType imageSize_t;


    imageRegion_t entireRegion = img->GetLargestPossibleRegion();

    long minX = entireRegion.GetSize()[0];
    long minY = entireRegion.GetSize()[1];
    long minZ = entireRegion.GetSize()[2];

    long maxX = 0;
    long maxY = 0;
    long maxZ = 0;

    //    std::cout<<"hahaha = "<<minX<<'\t'<<minY<<'\t'<<minZ<<'\t'<<maxX<<'\t'<<maxX<<'\t'<<maxX<<'\n';

    typedef itk::ImageRegionConstIteratorWithIndex< image_t > itkImageRegionConstIteratorWithIndex_t;

    itkImageRegionConstIteratorWithIndex_t it(img, entireRegion);

    char foundNonZero = 0;

    {
      imageIndex_t idx;
      for (it.GoToBegin(); !it.IsAtEnd(); ++it)
        {
          if (it.Get() != 0)
            {
              foundNonZero = 1;

              idx = it.GetIndex();

              minX = minX<idx[0]?minX:idx[0];
              minY = minY<idx[1]?minY:idx[1];
              minZ = minZ<idx[2]?minZ:idx[2];

              maxX = maxX>idx[0]?maxX:idx[0];
              maxY = maxY>idx[1]?maxY:idx[1];
              maxZ = maxZ>idx[2]?maxZ:idx[2];
            }
        }
    }

    imageRegion_t nonZeroRegion;

    if (1 == foundNonZero)
      {
        imageIndex_t startIdx;
        startIdx[0] = minX;
        startIdx[1] = minY;
        startIdx[2] = minZ;

        imageSize_t size;
        size[0] = maxX - minX;
        size[1] = maxY - minY;
        size[2] = maxZ - minZ;

        nonZeroRegion.SetSize( size );
        nonZeroRegion.SetIndex( startIdx );
      }
    else
      {
        imageIndex_t startIdx;
        startIdx[0] = 0;
        startIdx[1] = 0;
        startIdx[2] = 0;

        imageSize_t size;
        size[0] = entireRegion.GetSize()[0];
        size[1] = entireRegion.GetSize()[1];
        size[2] = entireRegion.GetSize()[2];

        nonZeroRegion.SetSize( size );
        nonZeroRegion.SetIndex( startIdx );
      }

    
    return nonZeroRegion;
  }


  /**
   * Enlarge the region by 1/5 at each end, care is taken at the
   * boundary.
   */
  template<typename image_t>
  typename image_t::RegionType
  enlargeNonZeroRegion(typename image_t::Pointer img, typename image_t::RegionType nonZeroRegion)
  {
    typedef typename image_t::RegionType imageRegion_t;
    typedef typename image_t::IndexType imageIndex_t;
    typedef typename image_t::SizeType imageSize_t;

    imageRegion_t entireRegion = img->GetLargestPossibleRegion();
    imageSize_t entireSize = entireRegion.GetSize();

    imageIndex_t start = nonZeroRegion.GetIndex();
    imageSize_t sz = nonZeroRegion.GetSize();

    start[0] = std::max(0l, static_cast<long>(start[0] - sz[0]/5));
    start[1] = std::max(0l, static_cast<long>(start[1] - sz[1]/5));
    start[2] = std::max(0l, static_cast<long>(start[2] - sz[2]/5));

    sz[0] = std::min(entireSize[0] - start[0], 7*sz[0]/5);
    sz[1] = std::min(entireSize[1] - start[1], 7*sz[1]/5);
    sz[2] = std::min(entireSize[2] - start[2], 7*sz[2]/5);

    
    /**********************************************************************************
    {
      //tst
      std::cout<<"\t\t start =    "<<start<<std::endl<<std::flush;
      std::cout<<"\t\t entireSize =    "<<entireSize<<std::endl<<std::flush;
      std::cout<<"\t\t entireSize[1] - start[1], 7*sz[1]/5   "<<entireSize[1] - start[1]<<'\t'<<7*sz[1]/5<<'\t'<<sz[1]<<std::endl<<std::flush;
      //tst//
    }
    **********************************************************************************/

    imageRegion_t largerRegion;
    largerRegion.SetSize( sz );
    largerRegion.SetIndex( start );

    return largerRegion;
  }


  /**
   * Extract the ROI from the image using the region
   */
  template<typename image_t>
  typename image_t::Pointer
  extractROI(typename image_t::Pointer img, typename image_t::RegionType region)
  {
    typedef itk::RegionOfInterestImageFilter<image_t, image_t> itkRegionOfInterestImageFilter_t;

    typename itkRegionOfInterestImageFilter_t::Pointer ROIfilter = itkRegionOfInterestImageFilter_t::New();
    ROIfilter->SetInput( img );
    ROIfilter->SetRegionOfInterest( region );
    ROIfilter->Update();

    return ROIfilter->GetOutput();
  }


  /**
   * Crop the image by the non-zero region given by the mask 
   */
  template<typename MaskImageType >
  typename MaskImageType::Pointer
  cropROIFromImage(typename MaskImageType::Pointer mask)
  {
    typename MaskImageType::RegionType ROIRegion = computeNonZeroRegion<MaskImageType>(mask);

    typename MaskImageType::RegionType enlargedROIRegion = enlargeNonZeroRegion<MaskImageType>(mask, ROIRegion);

    typename MaskImageType::Pointer ROIMask = extractROI<MaskImageType>(mask, enlargedROIRegion);

    return ROIMask;
  }


}// afibReg

#endif
