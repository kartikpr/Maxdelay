// --- CMAKE generated variables for your plugin

#include "pluginstructures.h"

#ifndef _plugindescription_h
#define _plugindescription_h

#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)
#define AU_COCOA_VIEWFACTORY_STRING STR(AU_COCOA_VIEWFACTORY_NAME)
#define AU_COCOA_VIEW_STRING STR(AU_COCOA_VIEW_NAME)

// --- AU Plugin Cocoa View Names (flat namespace)
#define AU_COCOA_VIEWFACTORY_NAME AUCocoaViewFactory_2A7AB9A7DAE54D28BD1AD1042231C2FD
#define AU_COCOA_VIEW_NAME AUCocoaView_2A7AB9A7DAE54D28BD1AD1042231C2FD

// --- BUNDLE IDs (MacOS Only)
const char* kAAXBundleID = "karmicfq.aax.Delayproject3.bundleID";
const char* kAUBundleID = "karmicfq.au.Delayproject3.bundleID";
const char* kVST3BundleID = "karmicfq.vst3.Delayproject3.bundleID";

// --- Plugin Names
const char* kPluginName = "Delayproject3";
const char* kShortPluginName = "Delayproject3";
const char* kAUBundleName = "Delayproject3";
const char* kAAXBundleName = "Delayproject3";
const char* kVSTBundleName = "Delayproject3";

// --- bundle name helper
inline static const char* getPluginDescBundleName()
{
#ifdef AUPLUGIN
	return kAUBundleName;
#endif

#ifdef AAXPLUGIN
	return kAAXBundleName;
#endif

#ifdef VSTPLUGIN
	return kVSTBundleName;
#endif

	// --- should never get here
	return kPluginName;
}

// --- Plugin Type
const pluginType kPluginType = pluginType::kFXPlugin;

// --- VST3 UUID
const char* kVSTFUID = "{2A7AB9A7-DAE5-4D28-BD1A-D1042231C2FD}";

// --- 4-char codes
const int32_t kFourCharCode = 'EqlK';
const int32_t kAAXProductID = 'OW5Z';
const int32_t kManufacturerID = 'VNDR';

// --- Vendor information
const char* kVendorName = "KarmicFQ";
const char* kVendorURL = "www.yourcompany.com";
const char* kVendorEmail = "";

// --- Plugin Options
const bool kProcessFrames = true;
const uint32_t kBlockSize = DEFAULT_AUDIO_BLOCK_SIZE;
const bool kWantSidechain = false;
const uint32_t kLatencyInSamples = 0;
const double kTailTimeMsec = 0.000;
const bool kVSTInfiniteTail = false;
const bool kVSTSAA = false;
const uint32_t kVST3SAAGranularity = 1;
const uint32_t kAAXCategory = 0;

#endif



