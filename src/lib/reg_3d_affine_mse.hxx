#ifndef reg_3d_affine_mse_hxx_
#define reg_3d_affine_mse_hxx_

#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


// itk
#include "itkAffineTransform.h"

#include "itkCenteredTransformInitializer.h"

#include "itkCommand.h"

#include "itkImage.h"
#include "itkImageRegistrationMethod.h"

#include "itkLinearInterpolateImageFunction.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkRegularStepGradientDescentOptimizer.h"


// local
#include "reg_3d_affine_mse.h"

namespace afibReg
{

  //  The following section of code implements a Command observer
  //  used to monitor the evolution of the registration process.
  //
  class CommandIterationUpdate : public itk::Command 
  {
  public:
    typedef  CommandIterationUpdate   Self;
    typedef  itk::Command             Superclass;
    typedef itk::SmartPointer<Self>   Pointer;
    itkNewMacro( Self );
  protected:
    CommandIterationUpdate() {};
  public:
    typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
    typedef   const OptimizerType *                  OptimizerPointer;

    void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

    void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( ! itk::IterationEvent().CheckEvent( &event ) )
        {
          return;
        }
      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
  };

  template<typename fix_image_t, typename moving_image_t>
  itk::AffineTransform<double, 3>::Pointer
  affineMSERegistration(typename fix_image_t::Pointer fixImg, typename moving_image_t::Pointer movingImg, double& finalCost)
  {
    typedef itk::AffineTransform< double, 3 > transform_t;

    typedef itk::RegularStepGradientDescentOptimizer OptimizerType;
    typedef itk::MeanSquaresImageToImageMetric< fix_image_t, moving_image_t >  MetricType;
    typedef itk::LinearInterpolateImageFunction< moving_image_t, double >    InterpolatorType;
    typedef itk::ImageRegistrationMethod< fix_image_t, moving_image_t >    RegistrationType;

    typename MetricType::Pointer         metric        = MetricType::New();
    typename OptimizerType::Pointer      optimizer     = OptimizerType::New();
    typename InterpolatorType::Pointer   interpolator  = InterpolatorType::New();
    typename RegistrationType::Pointer   registration  = RegistrationType::New();

    registration->SetMetric(        metric        );
    registration->SetOptimizer(     optimizer     );
    registration->SetInterpolator(  interpolator  );

    typename transform_t::Pointer  transform = transform_t::New();
    registration->SetTransform( transform );

    registration->SetFixedImage(  fixImg    );
    registration->SetMovingImage( movingImg );

    registration->SetFixedImageRegion( fixImg->GetBufferedRegion() );

    typedef itk::CenteredTransformInitializer< transform_t, fix_image_t, moving_image_t >  TransformInitializerType;
    typename TransformInitializerType::Pointer initializer = TransformInitializerType::New();
    initializer->SetTransform(   transform );
    initializer->SetFixedImage(  fixImg );
    initializer->SetMovingImage( movingImg );
    initializer->MomentsOn();
    //initializer->GeometryOn();
    initializer->InitializeTransform();


    registration->SetInitialTransformParameters( transform->GetParameters() );

    double translationScale = 1.0 / 1000.0;

    typedef OptimizerType::ScalesType       OptimizerScalesType;
    OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );

    optimizerScales[0] =  1.0;
    optimizerScales[1] =  1.0;
    optimizerScales[2] =  1.0;
    optimizerScales[3] =  1.0;
    optimizerScales[4] =  1.0;
    optimizerScales[5] =  1.0;
    optimizerScales[6] =  1.0;
    optimizerScales[7] =  1.0;
    optimizerScales[8] =  1.0;
    optimizerScales[9] =  translationScale;
    optimizerScales[10] =  translationScale;
    optimizerScales[11] =  translationScale;

    optimizer->SetScales( optimizerScales );

    double steplength = 0.1;
    optimizer->SetMaximumStepLength( steplength ); 
    optimizer->SetMinimumStepLength( 0.0005 );

    unsigned int maxNumberOfIterations = 2000;
    optimizer->SetNumberOfIterations( maxNumberOfIterations );
    optimizer->MinimizeOn();

    try 
      { 
        registration->StartRegistration(); 
        //     std::cout << "Optimizer stop condition: "
        //               << registration->GetOptimizer()->GetStopConditionDescription()
        //               << std::endl;
      } 
    catch( itk::ExceptionObject & err ) 
      { 
        std::cerr << "ExceptionObject caught !" << std::endl; 
        std::cerr << err << std::endl; 
        exit(-1);
      } 

    // record final cost function value
    finalCost = optimizer->GetValue();
  
    //   //tst
    //   std::cout<<"finalCostValue = "<<finalCostValue<<std::endl;
    //   //tst//

    OptimizerType::ParametersType finalParameters = registration->GetLastTransformParameters();

    typename transform_t::Pointer finalTransform = transform_t::New();
    finalTransform->SetParameters( finalParameters );
    finalTransform->SetFixedParameters( transform->GetFixedParameters() );

    return finalTransform;
  }


}

#endif
