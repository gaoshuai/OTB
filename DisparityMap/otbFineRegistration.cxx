/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#include "otbFineRegistration.h"

#include <iostream>
#include "otbCommandLineArgumentParser.h"

#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbStreamingImageFileWriter.h"
#include "otbFineRegistrationImageFilter.h"
#include "otbStandardWriterWatcher.h"
#include "otbVectorImage.h"
#include "otbImageList.h"
#include "otbImageListToVectorImageFilter.h"

#include "itkTimeProbe.h"
#include "itkFixedArray.h"
#include "itkNormalizedCorrelationImageToImageMetric.h"
#include "itkMeanReciprocalSquareDifferenceImageToImageMetric.h"
#include "itkMeanSquaresImageToImageMetric.h"
#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkAbsImageFilter.h"
#include "itkVectorIndexSelectionCastImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "otbStreamingWarpImageFilter.h"
#include "otbMultiChannelExtractROI.h"
#include "itkCastImageFilter.h"

namespace otb
{

const unsigned int Dimension = 2;
typedef double     PixelType;

typedef itk::FixedArray<PixelType, Dimension>                                 DeformationValueType;
typedef otb::Image< PixelType,  Dimension >                                  ImageType;
typedef otb::VectorImage<PixelType, Dimension>                                VectorImageType;
typedef otb::ImageList<ImageType>                                            ImageListType;
typedef otb::ImageListToVectorImageFilter<ImageListType, VectorImageType>     IL2VIFilterType;
typedef otb::Image<DeformationValueType, Dimension>                           FieldImageType;
typedef otb::ImageFileReader< ImageType >                                    ReaderType;
typedef otb::ImageFileReader< VectorImageType >                              VectorReaderType;
typedef otb::StreamingImageFileWriter< VectorImageType >                     WriterType;
typedef otb::FineRegistrationImageFilter<ImageType, ImageType, FieldImageType> RegistrationFilterType;
typedef itk::DiscreteGaussianImageFilter<ImageType, ImageType>                GaussianFilterType;
typedef itk::VectorIndexSelectionCastImageFilter<FieldImageType, ImageType>   VectorImageToImageFilterType;
typedef itk::AbsImageFilter<ImageType, ImageType>                             AbsFilterType;
typedef itk::BinaryThresholdImageFilter<ImageType, ImageType>                 BinaryThresholdImageFilterType;

template<class TVLV, class TFixedArray>
class VLVToFixedArray
{
public:
  TFixedArray operator()(const TVLV& vlv)
  {
    TFixedArray output;
    output[0] = vlv[0];
    output[1] = vlv[1];
    return output;
  }
};

int FineRegistration::Describe(ApplicationDescriptor* descriptor)
{
  descriptor->SetName("FineRegistration");
  descriptor->SetDescription("Estimate disparity map between two images. Output image contain x offset, y offset and metric value.");
  descriptor->AddOption("Reference", "The reference image",
                        "ref", 1, true, ApplicationDescriptor::InputImage);
  descriptor->AddOption("Secondary", "The secondary image",
                        "sec", 1, true, ApplicationDescriptor::InputImage);
  descriptor->AddOption("OutputImage", "The output image",
                        "out", 1, true, ApplicationDescriptor::OutputImage);
  descriptor->AddOption("ImageToWarp", "The image to warp after disparity estimation is complete",
                        "w", 1, false, ApplicationDescriptor::InputImage);
  descriptor->AddOption("WarpOutput", "The output warped image",
                        "wo", 1, false, ApplicationDescriptor::OutputImage);
  descriptor->AddOption("ExplorationRadius","Radius (in pixels) of the exploration window",
                        "er", 2, true, ApplicationDescriptor::Integer);
  descriptor->AddOption("MetricRadius","Radius (in pixels) of the metric computation window",
                        "mr", 2, true, ApplicationDescriptor::Integer);
  descriptor->AddOption("CoarseOffset","(optional) Coarse offset (in physical space) between the two images. Default is [0, 0].",
                        "co", 2, false, ApplicationDescriptor::Real);
  descriptor->AddOption("SubSamplingRate","(optional) Generate a result at a coarser resolution with a given sub-sampling rate in each direction (in pixels). Default is no sub-sampling",
                        "ssr", 2, false, ApplicationDescriptor::Real);
  descriptor->AddOption("ReferenceGaussianSmoothing","(optional) Perform a gaussian smoothing of the reference image. Parameters are gaussian sigma (in pixels) in each direction. Default is no smoothing.",
                        "rgs", 2, false, ApplicationDescriptor::Real);
  descriptor->AddOption("SecondaryGaussianSmoothing","(optional) Perform a gaussian smoothing of the secondary image. Parameters are gaussian sigma (in pixels) in each direction. Default is no smoothing.",
                        "sgs", 2, false, ApplicationDescriptor::Real);
  descriptor->AddOption("Metric","(optional) Choose the metric used for block matching. Available metrics are cross-correlation (CC), cross-correlation with subtracted mean (CCSM), mean-square difference (MSD), mean reciprocal square difference (MRSD) and mutual information (MI). Default is cross-correlation",
                        "m", 1, false, ApplicationDescriptor::String);
  descriptor->AddOption("SubPixelAccuracy","(optional) Metric extrema location will be refined up to the given accuracy. Default is 0.01",
                        "spa", 1, false, ApplicationDescriptor::Real);
  descriptor->AddOption("ValidityMask","(optional) Threshold to obtain a validity mask. Params are lowerThan or greaterThan and a threshold",
                        "vm", 2, false, ApplicationDescriptor::Real);
  return EXIT_SUCCESS;
}


int FineRegistration::Execute(otb::ApplicationOptionsResult* parseResult)
{

  // Read reference image
  ReaderType::Pointer freader = ReaderType::New();
  freader->SetFileName(parseResult->GetParameterString("Reference"));

  // Read secondary image
  ReaderType::Pointer mreader = ReaderType::New();
  mreader->SetFileName(parseResult->GetParameterString("Secondary"));

  // Retrieve main registration parameters
  RegistrationFilterType::SizeType radius, sradius;
  ImageType::OffsetType ssrate;
  sradius[0] = parseResult->GetParameterULong("ExplorationRadius", 0);
  sradius[1] = parseResult->GetParameterULong("ExplorationRadius", 1);
  radius[0] = parseResult->GetParameterULong("MetricRadius", 0);
  radius[1] = parseResult->GetParameterULong("MetricRadius", 1);

  double accuracy = 0.01;
  if(parseResult->IsOptionPresent("SubPixelAccuracy"))
    {
    accuracy = parseResult->GetParameterDouble("SubPixelAccuracy");
    }
  ssrate.Fill(1);
  if(parseResult->IsOptionPresent("SubSamplingRate"))
    {
    ssrate[0] = parseResult->GetParameterDouble("SubSamplingRate", 0);
    ssrate[1] = parseResult->GetParameterDouble("SubSamplingRate", 1);
    }
  RegistrationFilterType::SpacingType initialOffset;
  initialOffset.Fill(0);
  if(parseResult->IsOptionPresent("CoarseOffset"))
    {
    initialOffset[0] = parseResult->GetParameterDouble("CoarseOffset", 0);
    initialOffset[1] = parseResult->GetParameterDouble("CoarseOffset", 1);
    }

  // Display info
  std::cout<<"Reference image   : "<<freader->GetFileName()<<std::endl;
  std::cout<<"Secondary image   : "<<mreader->GetFileName()<<std::endl;

  std::cout<<"Exploration radius: "<<sradius<<" (pixels)"<<std::endl;
  std::cout<<"Metric radius     : "<<radius<<" (pixels)"<<std::endl;
  std::cout<<"Sub-sampling rate : "<<ssrate<<" (pixels)"<<std::endl;
  std::cout<<"Coarse offset     : "<<initialOffset<<" (physical unit)"<<std::endl;
  std::cout<<"Accuracy          : "<<accuracy<<" (physical unit)"<<std::endl;


  RegistrationFilterType::Pointer registration = RegistrationFilterType::New();
  registration->SetRadius(radius);
  registration->SetSearchRadius(sradius);
  registration->SetSubPixelAccuracy(accuracy);
  registration->SetGridStep(ssrate);
  registration->SetInitialOffset(initialOffset);

  GaussianFilterType::Pointer refGaussianFilter, secGaussianFilter;

  if(parseResult->IsOptionPresent("ReferenceGaussianSmoothing"))
    {
    refGaussianFilter = GaussianFilterType::New();
    refGaussianFilter->SetInput(freader->GetOutput());
    GaussianFilterType::ArrayType sigma;
    sigma[0] = parseResult->GetParameterDouble("ReferenceGaussianSmoothing", 0);
    sigma[1] = parseResult->GetParameterDouble("ReferenceGaussianSmoothing", 1);
    refGaussianFilter->SetVariance(sigma);
    refGaussianFilter->SetUseImageSpacingOff();
    std::cout<<"Reference image gaussian smoothing on."<<std::endl;
    std::cout<<"variance        : "<<sigma<<" (pixels)"<<std::endl;
    registration->SetFixedInput(refGaussianFilter->GetOutput());
    }
  else
    {
    std::cout<<"Reference image gaussian smoothing off."<<std::endl;
    registration->SetFixedInput(freader->GetOutput());
    }

  if(parseResult->IsOptionPresent("SecondaryGaussianSmoothing"))
      {
      secGaussianFilter = GaussianFilterType::New();
      secGaussianFilter->SetInput(mreader->GetOutput());
      GaussianFilterType::ArrayType sigma;
      sigma[0] = parseResult->GetParameterDouble("SecondaryGaussianSmoothing", 0);
      sigma[1] = parseResult->GetParameterDouble("SecondaryGaussianSmoothing", 1);
      secGaussianFilter->SetVariance(sigma);
      secGaussianFilter->SetUseImageSpacingOff();
      std::cout<<"Secondary image gaussian smoothing on."<<std::endl;
      std::cout<<"variance        : "<<sigma<<" (pixels)"<<std::endl;
      registration->SetMovingInput(secGaussianFilter->GetOutput());
      }
    else
      {
      std::cout<<"Secondary image gaussian smoothing off."<<std::endl;
      registration->SetMovingInput(mreader->GetOutput());
      }

  // Retrieve metric name
  std::string metricId = "CC";
  if(parseResult->IsOptionPresent("Metric"))
    {
    metricId = parseResult->GetParameterString("Metric");
    }

  if(metricId == "CC")
    {
    std::cout<<"Metric            : Cross-correlation"<<std::endl;
    typedef itk::NormalizedCorrelationImageToImageMetric<ImageType, ImageType> NCCType;
    NCCType::Pointer metricPtr = NCCType::New();
    metricPtr->SubtractMeanOff();
    registration->SetMetric(metricPtr);
    registration->MinimizeOn();
    }
  else if(metricId == "CCSM")
    {
    std::cout<<"Metric            : Cross-correlation (mean subtracted)"<<std::endl;
    typedef itk::NormalizedCorrelationImageToImageMetric<ImageType, ImageType> NCCType;
    NCCType::Pointer metricPtr = NCCType::New();
    metricPtr->SubtractMeanOn();
    registration->SetMetric(metricPtr);
    registration->MinimizeOn();
    }
  else if(metricId == "MSD")
    {
    std::cout<<"Metric            : Mean square difference"<<std::endl;
    typedef itk::MeanSquaresImageToImageMetric<ImageType, ImageType> MeanSquareType;
    MeanSquareType::Pointer metricPtr = MeanSquareType::New();
    registration->SetMetric(metricPtr);
    registration->MinimizeOn();
    }
  else if(metricId == "MRSD")
    {
    std::cout<<"Metric            : Mean reciprocal square difference"<<std::endl;
    typedef itk::MeanReciprocalSquareDifferenceImageToImageMetric<ImageType, ImageType> MRSDType;
    MRSDType::Pointer metricPtr = MRSDType::New();
    registration->SetMetric(metricPtr);
    registration->MinimizeOff();
    }
  else if(metricId == "MI")
    {
    std::cout<<"Metric            : Mutual information"<<std::endl;
    typedef itk::MattesMutualInformationImageToImageMetric<ImageType, ImageType> MIType;
    MIType::Pointer metricPtr = MIType::New();
    registration->SetMetric(metricPtr);
    registration->MinimizeOn();
    }
  else
    {
    std::cerr<<"Metric "<<metricId<<" not recognized."<<std::endl;
    std::cerr<<"Possible choices are: CC, CCSM, MSD, MRSD, MI"<<std::endl;
    return EXIT_FAILURE;
    }
  VectorImageToImageFilterType::Pointer xExtractor = VectorImageToImageFilterType::New();
  xExtractor->SetInput(registration->GetOutputDeformationField());
  xExtractor->SetIndex(0);

  VectorImageToImageFilterType::Pointer yExtractor = VectorImageToImageFilterType::New();
  yExtractor->SetInput(registration->GetOutputDeformationField());
  yExtractor->SetIndex(1);

  ImageListType::Pointer il = ImageListType::New();
  il->PushBack(xExtractor->GetOutput());
  il->PushBack(yExtractor->GetOutput());

  AbsFilterType::Pointer absFilter;

  // Invert correlation to get classical rendering
  if(metricId == "CC" || metricId == "CCSM")
    {
    absFilter = AbsFilterType::New();
    absFilter->SetInput(registration->GetOutput());
    il->PushBack(absFilter->GetOutput());
    }
  else
    {
    il->PushBack(registration->GetOutput());
    }

  registration->UpdateOutputInformation();

  BinaryThresholdImageFilterType::Pointer threshold;
  if(parseResult->IsOptionPresent("ValidityMask"))
    {
    threshold = BinaryThresholdImageFilterType::New();

    if(metricId == "CC" || metricId == "CCSM")
      {
      threshold->SetInput(absFilter->GetOutput());
      }
    else
      {
      threshold->SetInput(registration->GetOutput());
      }
    if(parseResult->GetParameterString("ValidityMask", 0)=="greaterThan")
      {
      threshold->SetLowerThreshold(parseResult->GetParameterDouble("ValidityMask", 1));
      }
    else if(parseResult->GetParameterString("ValidityMask", 0)=="lowerThan")
      {
      threshold->SetUpperThreshold(parseResult->GetParameterDouble("ValidityMask", 1));
      }
    else
      {
      std::cerr<<"Arguments of --ValidityMask option should begin with lowerThan or greaterThan"<<std::endl;
      return EXIT_FAILURE;
      }

    std::cout<<"A validity mask will be produced as the 4th band (valid pixels = 1.0, not valid = 0.0)."<<std::endl;
    std::cout<<"Pixels are considered valid if metric is "<<parseResult->GetParameterString("ValidityMask", 0)<<" ";
    std::cout<<parseResult->GetParameterDouble("ValidityMask", 1)<<std::endl;

    threshold->SetInsideValue(1.0);
    threshold->SetOutsideValue(0.);
    il->PushBack(threshold->GetOutput());
    }

  IL2VIFilterType::Pointer il2vi = IL2VIFilterType::New();
  il2vi->SetInput(il);

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName(parseResult->GetOutputImage());
  writer->SetInput(il2vi->GetOutput());

  std::cout<<std::endl;
  otb::StandardWriterWatcher watcher(writer, registration,"Fine Registration");

  writer->Update();

  if (parseResult->IsOptionPresent("ImageToWarp") && parseResult->IsOptionPresent("WarpOutput") )
    {
    // Now reuse the written deformation field to warp
    VectorReaderType::Pointer deformationReader = VectorReaderType::New();
    deformationReader->SetFileName(parseResult->GetOutputImage());

    typedef otb::MultiChannelExtractROI<PixelType, PixelType> ExtractROIFilterType;
    ExtractROIFilterType::Pointer extractROIFilter = ExtractROIFilterType::New();
    extractROIFilter->SetChannel(1);
    extractROIFilter->SetChannel(2);
    extractROIFilter->SetInput(deformationReader->GetOutput());

    typedef VLVToFixedArray<VectorImageType::PixelType, FieldImageType::PixelType> VLVToFixedArrayType;
    typedef itk::UnaryFunctorImageFilter<VectorImageType, FieldImageType, VLVToFixedArrayType> CastFilterType;
    CastFilterType::Pointer cast = CastFilterType::New();
    cast->SetInput(extractROIFilter->GetOutput());

    VectorReaderType::Pointer imageToWarpReader = VectorReaderType::New();
    imageToWarpReader->SetFileName(parseResult->GetParameterString("ImageToWarp"));

    typedef StreamingWarpImageFilter<VectorImageType, VectorImageType, FieldImageType> WarpFilterType;
    WarpFilterType::Pointer warp = WarpFilterType::New();

    warp->SetDeformationField(cast->GetOutput());
    warp->SetInput(imageToWarpReader->GetOutput());
    warp->SetOutputParametersFromImage(freader->GetOutput());

    WriterType::Pointer wrappedWriter = WriterType::New();
    wrappedWriter->SetFileName(parseResult->GetParameterString("WarpOutput"));
    wrappedWriter->SetInput(warp->GetOutput());
    otb::StandardWriterWatcher watcher2(wrappedWriter, warp,"Warp");
    wrappedWriter->Update();
    }

  return EXIT_SUCCESS;
}

}
