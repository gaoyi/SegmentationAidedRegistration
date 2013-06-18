cmake_minimum_required(VERSION 2.8.9)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  set(EXTENSION_NAME SegmentationAidedRegistration)
  set(EXTENSION_HOMEPAGE "http://www.slicer.org/slicerWiki/index.php/Documentation/Nightly/Modules/SegmentationAidedRegistration")
  set(EXTENSION_CATEGORY "Registration")
  set(EXTENSION_CONTRIBUTORS "Yi Gao (BWH/UAB), Liang-Jia Zhu (UAB), Josh Cates (Utah), Alan Morris (Utah), Danny Perry (Utah), Greg Gardner (Utah), Rob MacLeod (Utah), Sylvain Bouix (BWH), Allen Tannenbaum (UAB)")
  set(EXTENSION_DESCRIPTION "Performe the registration with the aid from segmentation so that the segmented regions have close match.")
  set(EXTENSION_ICONURL "http://www.slicer.org/slicerWiki/images/b/ba/SegAidedRegSquareFocus128.png")
  set(EXTENSION_SCREENSHOTURLS "http://www.slicer.org/slicerWiki/images/b/ba/SegmentationAidedRegistrationUsageScreenShot.png")
endif()

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  find_package(Slicer REQUIRED)
  include(${Slicer_USE_FILE})
endif()

#-----------------------------------------------------------------------------
add_subdirectory(SegmentationAidedRegistration)

#-----------------------------------------------------------------------------
if(NOT Slicer_SOURCE_DIR)
  include(${Slicer_EXTENSION_CPACK})
endif()
