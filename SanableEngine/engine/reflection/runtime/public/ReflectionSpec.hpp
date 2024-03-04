#pragma once

#if defined(__STIX_REFLECTION_GENERATING) || defined(__clang__)
#define __STIX_REFLECTION_ANNOTATE(...) [[clang::annotate(__VA_ARGS__)]] 
#else
#define __STIX_REFLECTION_ANNOTATE(...)
#endif


//#define STIX_REFLECTION_VISIBLE     __STIX_REFLECTION_ANNOTATE("stix::should_reflect=yes")
//#define STIX_REFLECTION_NOT_VISIBLE __STIX_REFLECTION_ANNOTATE("stix::should_reflect=no")


#pragma region Image capture controls

//Enable/disable attributes are nestable. For example:
//
//class STIX_DISABLE_IMAGE_CAPTURE MyFunkyClass {
//  MyFunkyClass(); //Ctor not reflected
//  class STIX_ENABLE_IMAGE_CAPTURE MyFunkySubclass {}; //Ctor will be reflected
//};
//

#define STIX_DISABLE_IMAGE_CAPTURE __STIX_REFLECTION_ANNOTATE("stix::do_image_capture=no")
#define STIX_ENABLE_IMAGE_CAPTURE __STIX_REFLECTION_ANNOTATE("stix::do_image_capture=yes")

//Valid only on the class itself
#define STIX_IMAGE_CAPTURE_BACKEND(id)      __STIX_REFLECTION_ANNOTATE("stix::image_capture_backend=" #id)
#define STIX_IMAGE_CAPTURE_WITH_CDO         STIX_IMAGE_CAPTURE_BACKEND(cdo)
#define STIX_IMAGE_CAPTURE_WITH_DISASSEMBLY STIX_IMAGE_CAPTURE_BACKEND(disassembly)
#define STIX_IMAGE_CAPTURE_WITH_DEFAULT     STIX_IMAGE_CAPTURE_BACKEND(default)

//#define STIX_IMAGE_CAPTURE_PREFER_CTOR __STIX_REFLECTION_ANNOTATE("stix::image_capture_prefer_ctor")

#pragma endregion Image capture controls
