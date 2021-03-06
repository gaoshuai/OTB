/*
 * Copyright (C) 2005-2017 Centre National d'Etudes Spatiales (CNES)
 *
 * This file is part of Orfeo Toolbox
 *
 *     https://www.orfeo-toolbox.org/
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "otbTestMain.h"

void RegisterTests()
{
  REGISTER_TEST(otbOpticalDefaultImageMetadataInterfaceNew);
  REGISTER_TEST(otbSarImageMetadataInterfaceTest);
  REGISTER_TEST(otbDefaultImageMetadataInterfaceFactoryNew);
  REGISTER_TEST(otbOpticalDefaultImageMetadataInterfaceFactoryNew);
  REGISTER_TEST(otbPleiadesImageMetadataInterfaceNew);
  REGISTER_TEST(otbImageMetadataInterfaceBaseTest);
  REGISTER_TEST(otbSarDefaultImageMetadataInterface);
  REGISTER_TEST(otbSarDefaultImageMetadataInterfaceNew);
  REGISTER_TEST(otbOpticalImageMetadataInterfaceTest);
  REGISTER_TEST(otbSarDefaultImageMetadataInterfaceFactoryNew);
  REGISTER_TEST(otbFormosatImageMetadataInterfaceNew);
  REGISTER_TEST(otbOpticalDefaultImageMetadataInterface);
  REGISTER_TEST(otbSpot6ImageMetadataInterfaceNew);
  REGISTER_TEST(otbSpotImageMetadataInterfaceNew);
  REGISTER_TEST(otbQuickBirdImageMetadataInterfaceNew);
  REGISTER_TEST(otbIkonosImageMetadataInterfaceNew);
  REGISTER_TEST(otbTerraSarImageMetadataInterfaceNew);
  REGISTER_TEST(otbSentinel1ImageMetadataInterfaceNew);
  REGISTER_TEST(otbWorldView2ImageMetadataInterfaceNew);
  REGISTER_TEST(otbDefaultImageMetadataInterface);
  REGISTER_TEST(otbImageMetadataInterfaceTest2);
  REGISTER_TEST(otbNoDataHelperTest);
  REGISTER_TEST(otbSarCalibrationLookupDataTest);
  REGISTER_TEST(otbRadarsat2ImageMetadataInterfaceNew);
}
