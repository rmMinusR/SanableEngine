#pragma once

#ifdef __STIX_REFELCTION_GENERATING
#define __STIX_REFELCTION_ANNOTATE(...) [[clang::annotate(__VA_ARGS__)]] 
#else
#define __STIX_REFELCTION_ANNOTATE(...)
#endif

//#define STIX_REFLECTION_VISIBLE     __STIX_REFELCTION_ANNOTATE("stix::should_reflect=yes")
//#define STIX_REFLECTION_NOT_VISIBLE __STIX_REFELCTION_ANNOTATE("stix::should_reflect=no")

//Apply to a class or constructor to control image capture
//For example: class STIX_DISABLE_IMAGE_CAPTURE MyFunkyClass {};
#define STIX_DISABLE_IMAGE_CAPTURE __STIX_REFELCTION_ANNOTATE("stix::no_image_capture")
//#define STIX_IMAGE_CAPTURE_PREFER_CTOR __STIX_REFELCTION_ANNOTATE("stix::image_capture_prefer_ctor")
//#define STIX_IMAGE_CAPTURE_WITH_CDO        __STIX_REFELCTION_ANNOTATE("stix::image_capture_with_cdo")
//#define STIX_IMAGE_CAPTURE_WITH_DISASSEMBLY __STIX_REFELCTION_ANNOTATE("stix::image_capture_with_disassembly")
