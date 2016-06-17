/*
 * Copyright (c) 2015-16 Motorola Mobility, LLC.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __CAMERA_EXT_CTRLS_H
#define __CAMERA_EXT_CTRLS_H

#define V4L2_CTRL_CLASS_MOD		0x00f00000
#define CID_CAM_EXT_CLASS_BASE	(V4L2_CTRL_CLASS_MOD | 0xf000)
#define MOD_CID_MOD_CLASS		(V4L2_CTRL_CLASS_MOD | 1)

/* int menu */
#define CAM_EXT_CID_COLOR_CORRECTION_ABERRATION_MODE CID_CAM_EXT_CLASS_BASE
enum {
	CAM_EXT_COLOR_CORRECTION_ABERRATION_OFF,
	CAM_EXT_COLOR_CORRECTION_ABERRATION_FAST,
	CAM_EXT_COLOR_CORRECTION_ABERRATION_HQ,

	CAM_EXT_COLOR_CORRECTION_ABERRATION_MAX =
			CAM_EXT_COLOR_CORRECTION_ABERRATION_HQ,
};

/* int menu */
#define CAM_EXT_CID_AE_ANTIBANDING_MODE (CID_CAM_EXT_CLASS_BASE + 1)
enum {
	CAM_EXT_AE_ANTIBANDING_OFF,
	CAM_EXT_AE_ANTIBANDING_50HZ,
	CAM_EXT_AE_ANTIBANDING_60HZ,
	CAM_EXT_AE_ANTIBANDING_AUTO,

	CAM_EXT_AE_ANTIBANDING_MAX = CAM_EXT_AE_ANTIBANDING_AUTO,
};

/* integer, range and step from mod */
#define CAM_EXT_CID_AE_EXPOSURE_COMPENSATION (CID_CAM_EXT_CLASS_BASE + 2)

/* boolean */
#define CAM_EXT_CID_AE_LOCK (CID_CAM_EXT_CLASS_BASE + 3)

/* int menu */
#define CAM_EXT_CID_AE_MODE (CID_CAM_EXT_CLASS_BASE + 4)
enum {
	CAM_EXT_AE_MODE_OFF,
	CAM_EXT_AE_MODE_ON,
	CAM_EXT_AE_MODE_ON_AUTO_FLASH,
	CAM_EXT_AE_MODE_ON_ALWAYS_FLASH,
	CAM_EXT_AE_MODE_ON_AUTO_FLASH_REDEYE,

	CAM_EXT_AE_MODE_MAX = CAM_EXT_AE_MODE_ON_AUTO_FLASH_REDEYE,
};

/* int menu, each menu item (64bit integer) stands for a range
 * (32bit high, 32bit low)
 * The available ranges (menu item list) are from MOD.
 */
#define CAM_EXT_CID_AE_TARGET_FPS_RANGE (CID_CAM_EXT_CLASS_BASE + 5)

/* int menu */
#define CAM_EXT_CID_AF_MODE (CID_CAM_EXT_CLASS_BASE + 6)
enum {
	CAM_EXT_AF_MODE_OFF,
	CAM_EXT_AF_MODE_AUTO,
	CAM_EXT_AF_MODE_MACRO,
	CAM_EXT_AF_MODE_CONTINUOUS_VIDEO,
	CAM_EXT_AF_MODE_CONTINUOUS_PICTURE,
	CAM_EXT_AF_MODE_EDOF,

	CAM_EXT_AF_MODE_MAX = CAM_EXT_AF_MODE_EDOF,
};

/* int menu */
#define CAM_EXT_CID_AF_TRIGGER (CID_CAM_EXT_CLASS_BASE + 7)
enum {
	CAM_EXT_AF_TRIGGER_IDLE,
	CAM_EXT_AF_TRIGGER_START,
	CAM_EXT_AF_TRIGGER_CANCEL,

	CAM_EXT_AF_TRIGGER_MAX = CAM_EXT_AF_TRIGGER_CANCEL,
};

/* boolean */
#define CAM_EXT_CID_AWB_LOCK (CID_CAM_EXT_CLASS_BASE + 8)

/* int menu */
#define CAM_EXT_CID_AWB_MODE (CID_CAM_EXT_CLASS_BASE + 9)
enum {
	CAM_EXT_AWB_MODE_OFF,
	CAM_EXT_AWB_MODE_AUTO,
	CAM_EXT_AWB_MODE_INCANDESCENT,
	CAM_EXT_AWB_MODE_FLUORESCENT,
	CAM_EXT_AWB_MODE_DAYLIGHT,
	CAM_EXT_AWB_MODE_CLOUDY_DAYLIGHT,
	CAM_EXT_AWB_MODE_TWILIGHT,
	CAM_EXT_AWB_MODE_SHADE,

	CAM_EXT_AWB_MODE_MAX = CAM_EXT_AWB_MODE_SHADE,
};

/* int menu */
#define CAM_EXT_CID_EFFECT_MODE (CID_CAM_EXT_CLASS_BASE + 10)
enum {
	CAM_EXT_EFFECT_MODE_OFF,
	CAM_EXT_EFFECT_MODE_MONO,
	CAM_EXT_EFFECT_MODE_NEGATIVE,
	CAM_EXT_EFFECT_MODE_SOLARIZE,
	CAM_EXT_EFFECT_MODE_SEPIA,
	CAM_EXT_EFFECT_MODE_POSTERIZE,
	CAM_EXT_EFFECT_MODE_WHITEBOARD,
	CAM_EXT_EFFECT_MODE_BLACKBOARD,
	CAM_EXT_EFFECT_MODE_AQUA,

	CAM_EXT_EFFECT_MODE_MAX = CAM_EXT_EFFECT_MODE_AQUA,
};

/* int menu */
#define CAM_EXT_CID_CONTROL_MODE (CID_CAM_EXT_CLASS_BASE + 11)
enum {
	CAM_EXT_CONTROL_MODE_OFF,
	CAM_EXT_CONTROL_MODE_AUTO,
	CAM_EXT_CONTROL_MODE_USE_SCENE_MODE,
	CAM_EXT_CONTROL_MODE_OFF_KEEP_STATE,

	CAM_EXT_CONTROL_MODE_MAX = CAM_EXT_CONTROL_MODE_OFF_KEEP_STATE,
};

/* int menu */
#define CAM_EXT_CID_SCENE_MODE (CID_CAM_EXT_CLASS_BASE + 12)
enum {
	CAM_EXT_SCENE_MODE_DISABLED,
	CAM_EXT_SCENE_MODE_FACE_PRIORITY,
	CAM_EXT_SCENE_MODE_ACTION,
	CAM_EXT_SCENE_MODE_PORTRAIT,
	CAM_EXT_SCENE_MODE_LANDSCAPE,
	CAM_EXT_SCENE_MODE_NIGHT,
	CAM_EXT_SCENE_MODE_NIGHT_PORTRAIT,
	CAM_EXT_SCENE_MODE_THEATRE,
	CAM_EXT_SCENE_MODE_BEACH,
	CAM_EXT_SCENE_MODE_SNOW,
	CAM_EXT_SCENE_MODE_SUNSET,
	CAM_EXT_SCENE_MODE_STEADYPHOTO,
	CAM_EXT_SCENE_MODE_FIREWORKS,
	CAM_EXT_SCENE_MODE_SPORTS,
	CAM_EXT_SCENE_MODE_PARTY,
	CAM_EXT_SCENE_MODE_CANDLELIGHT,
	CAM_EXT_SCENE_MODE_BARCODE,
	CAM_EXT_SCENE_MODE_HDR,

	CAM_EXT_SCENE_MODE_MAX = CAM_EXT_SCENE_MODE_HDR,
};

/* int menu */
#define CAM_EXT_CID_VIDEO_STABILIZATION_MODE (CID_CAM_EXT_CLASS_BASE + 13)
enum {
	CAM_EXT_VIDEO_STABILIZATION_MODE_ON,
	CAM_EXT_VIDEO_STABILIZATION_MODE_OFF,

	CAM_EXT_VIDEO_STABILIZATION_MODE_MAX =
		CAM_EXT_VIDEO_STABILIZATION_MODE_OFF,
};

/* double[2] */
#define CAM_EXT_CID_JPEG_GPS_LOCATION (CID_CAM_EXT_CLASS_BASE + 14)

/* int menu */
#define CAM_EXT_CID_JPEG_ORIENTATION (CID_CAM_EXT_CLASS_BASE + 15)
enum {
	CAM_EXT_JPEG_ORIENTATION_0,
	CAM_EXT_JPEG_ORIENTATION_90,
	CAM_EXT_JPEG_ORIENTATION_180,
	CAM_EXT_JPEG_ORIENTATION_270,

	CAM_EXT_JPEG_ORIENTATION_MAX = CAM_EXT_JPEG_ORIENTATION_270,
};

/* int 0 - 100 */
#define CAM_EXT_CID_JPEG_QUALITY (CID_CAM_EXT_CLASS_BASE + 16)

/* int read only */
#define CAM_EXT_CID_LENS_FACING (CID_CAM_EXT_CLASS_BASE + 17)
enum {
	CAM_EXT_LENS_FACING_FRONT,
	CAM_EXT_LENS_FACING_BACK,
	CAM_EXT_LENS_FACING_EXTERNAL,

	CAM_EXT_LENS_FACING_MAX = CAM_EXT_LENS_FACING_EXTERNAL,
};

/* int menu */
#define CAM_EXT_CID_FLASH_MODE (CID_CAM_EXT_CLASS_BASE + 18)
enum {
	CAM_EXT_FLASH_MODE_OFF,
	CAM_EXT_FLASH_MODE_SINGLE,
	CAM_EXT_FLASH_MODE_TORCH,

	CAM_EXT_FLASH_MODE_MAX = CAM_EXT_FLASH_MODE_TORCH,
};

/* float menu (floats defined at mod side) */
#define CAM_EXT_CID_FOCAL_LENGTH (CID_CAM_EXT_CLASS_BASE + 19)

/* int menu read only */
#define CAM_EXT_CID_CAPABILITIES (CID_CAM_EXT_CLASS_BASE + 20)
enum {
	CAM_EXT_CAPABILITIES_BACKWARD_COMPATIBLE,
	CAM_EXT_CAPABILITIES_MANUAL_SENSOR,
	CAM_EXT_CAPABILITIES_POST_PROCESSING,
	CAM_EXT_CAPABILITIES_RAW,
	CAM_EXT_CAPABILITIES_PRIVATE_REPROCESSING,
	CAM_EXT_CAPABILITIES_READ_SENSOR_SETTING,
	CAM_EXT_CAPABILITIES_BURST_CAPTURE,
	CAM_EXT_CAPABILITIES_YUV_REPROCESSING,
	CAM_EXT_CAPABILITIES_DEPTH_OUTPUT,
	CAM_EXT_CAPABILITIES_CONSTRAINED_HIGHT_SPEED_VIDEO,

	CAM_EXT_CAPABILITIES_MAX
		= CAM_EXT_CAPABILITIES_CONSTRAINED_HIGHT_SPEED_VIDEO,
};

/* integer read only */
#define CAM_EXT_CID_MAX_NUM_OUTPUT_PROC (CID_CAM_EXT_CLASS_BASE + 21)

/* integer read only */
#define CAM_EXT_CID_MAX_NUM_OUTPUT_PROC_STALLING (CID_CAM_EXT_CLASS_BASE + 22)

/* intger read only */
#define CAM_EXT_CID_MAX_NUM_OUTPUT_RAW (CID_CAM_EXT_CLASS_BASE + 23)

/* integer read only */
#define CAM_EXT_CID_PIPLELINE_MAX_DEPTH (CID_CAM_EXT_CLASS_BASE + 24)

/* float read only */
#define CAM_EXT_CID_SCALER_MAX_DIGITAL_ZOOM (CID_CAM_EXT_CLASS_BASE + 25)

/* integer menu, read only */
#define CAM_EXT_CID_SCALER_CROPPING_TYPE (CID_CAM_EXT_CLASS_BASE + 26)
enum {
	CAM_EXT_SCALER_CROPPING_TYPE_CENTER_ONLY,
	CAM_EXT_SCALER_CROPPING_TYPE_FREEFORM,

	CAM_EXT_SCALER_CROPPING_TYPE_MAX =
		CAM_EXT_SCALER_CROPPING_TYPE_FREEFORM,
};

/* integer [4]  */
#define CAM_EXT_CID_SCALER_CROP_REGION (CID_CAM_EXT_CLASS_BASE + 27)

/* float [2] ==> w, h read only */
#define CAM_EXT_CID_SENSOR_INFO_PHYSICAL_SIZE (CID_CAM_EXT_CLASS_BASE + 28)

/* integer [2] ==> w, h read only */
#define CAM_EXT_CID_SENSOR_INFO_PIXEL_ARRAY_SIZE (CID_CAM_EXT_CLASS_BASE + 29)

/* integer [4] ==> left, top, right, bottom read only */
#define CAM_EXT_CID_SENSOR_INFO_PRE_CORRECTION_ACTIVE_ARRAY_SIZE \
	(CID_CAM_EXT_CLASS_BASE + 30)

/* integer [4] ==> left, top, right, bottom read only */
#define CAM_EXT_CID_SENSOR_INFO_ACTIVE_ARRAY_SIZE (CID_CAM_EXT_CLASS_BASE + 31)

/* integer read only */
#define CAM_EXT_CID_SENSOR_INFO_TIMESTAMP_SOURCE (CID_CAM_EXT_CLASS_BASE + 32)
enum {
	CAM_EXT_SENSOR_INFO_TIMESTAMP_SOURCE_UNKNOWN,
	CAM_EXT_SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME,

	CAM_EXT_SENSOR_INFO_TIMESTAMP_SOURCE_MAX =
		CAM_EXT_SENSOR_INFO_TIMESTAMP_SOURCE_REALTIME,
};

/* integer read only */
#define CAM_EXT_CID_SENSOR_ORIENTATION (CID_CAM_EXT_CLASS_BASE + 33)
enum {
	CAM_EXT_SENSOR_ORIENTATION_0,
	CAM_EXT_SENSOR_ORIENTATION_90,
	CAM_EXT_SENSOR_ORIENTATION_180,
	CAM_EXT_SENSOR_ORIENTATION_270,

	CAM_EXT_SENSOR_ORIENTATION_MAX = CAM_EXT_SENSOR_ORIENTATION_270,
};

/* int menu */
#define CAM_EXT_CID_STATISTICS_FACE_DETECT_MODE (CID_CAM_EXT_CLASS_BASE + 34)
enum {
	CAM_EXT_STATISTICS_FACE_DETECT_MODE_OFF,
	CAM_EXT_STATISTICS_FACE_DETECT_MODE_SIMPLE,
	CAM_EXT_STATISTICS_FACE_DETECT_MODE_FULL,

	CAM_EXT_STATISTICS_FACE_DETECT_MODE_MAX =
		CAM_EXT_STATISTICS_FACE_DETECT_MODE_FULL,

};

/* int read only */
#define CAM_EXT_CID_STATISTICS_INFO_MAX_FACE_COUNT (CID_CAM_EXT_CLASS_BASE + 35)

/* int read only */
#define CAM_EXT_CID_SYNC_MAX_LATENCY (CID_CAM_EXT_CLASS_BASE + 36)
enum {
	CAM_EXT_SYNC_MAX_LATENCY_PER_FRAME_CONTROL,
	CAM_EXT_SYNC_MAX_LATENCY_UNKNOWN,

	CAM_EXT_SYNC_MAX_LATENCY_MAX = CAM_EXT_SYNC_MAX_LATENCY_UNKNOWN,
};

/*************************OPTIONAL CONTROLS***********************************/
/* int menu */
#define CAM_EXT_CID_CONTROL_AE_PRECATURE_TRIGGER (CID_CAM_EXT_CLASS_BASE + 37)
enum {
	CAM_EXT_CONTROL_AE_PRECATURE_TRIGGER_IDLE,
	CAM_EXT_CONTROL_AE_PRECATURE_TRIGGER_START,
	CAM_EXT_CONTROL_AE_PRECATURE_TRIGGER_CANCEL,

	CAM_EXT_CONTROL_AE_PRECATURE_TRIGGER_MAX =
			CAM_EXT_CONTROL_AE_PRECATURE_TRIGGER_CANCEL,
};

/* int read only */
#define CAM_EXT_CID_LENS_INFO_FOCUS_DISTANCE_CALIBRATION \
		(CID_CAM_EXT_CLASS_BASE + 38)
enum {
	CAM_EXT_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_UNCALIBRATED,
	CAM_EXT_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_APPROXIMATE,
	CAM_EXT_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_CALIBRATED,

	CAM_EXT_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_MAX =
		CAM_EXT_LENS_INFO_FOCUS_DISTANCE_CALIBRATION_CALIBRATED,
};

/* float read only */
#define CAM_EXT_CID_LENS_INFO_FOCUS_HYPERFOCAL_DISTANCE \
		(CID_CAM_EXT_CLASS_BASE + 39)

/* float read only */
#define CAM_EXT_CID_LENS_INFO_MINIMUM_FOCUS_DISTANCE \
		(CID_CAM_EXT_CLASS_BASE + 40)

/* float */
#define CAM_EXT_CID_LENS_FOCUS_DISTANCE (CID_CAM_EXT_CLASS_BASE + 41)

/* int menu */
#define CAM_EXT_CID_LENS_OPTICAL_STABILIZATION_MODE \
		(CID_CAM_EXT_CLASS_BASE + 42)
enum {
	CAM_EXT_CID_LENS_OPTICAL_STABILIZATION_MODE_OFF,
	CAM_EXT_CID_LENS_OPTICAL_STABILIZATION_MODE_ON,

	CAM_EXT_CID_LENS_OPTICAL_STABILIZATION_MODE_MAX =
		CAM_EXT_CID_LENS_OPTICAL_STABILIZATION_MODE_ON,
};
/* float */
#define CAM_EXT_CID_REPROCESS_EFFECTIVE_EXPOSURE_FACTOR \
		(CID_CAM_EXT_CLASS_BASE + 43)

/* integer read only */
#define CAM_EXT_CID_REPROCESS_MAX_CAPTURE_STALL (CID_CAM_EXT_CLASS_BASE + 44)

/* boolean read only */
#define CAM_EXT_CID_DEPTH_DEPTH_IS_EXCLUSIVE (CID_CAM_EXT_CLASS_BASE + 45)

/* boolean */
#define CAM_EXT_CID_BLACK_LEVEL_LOCK (CID_CAM_EXT_CLASS_BASE + 46)

/* int menu */
#define CAM_EXT_CID_COLOR_CORRECTION_MODE (CID_CAM_EXT_CLASS_BASE + 47)
enum {
	CAM_EXT_COLOR_CORRECTION_MODE_TRANSFORM_MATRIX,
	CAM_EXT_COLOR_CORRECTION_MODE_FAST,
	CAM_EXT_COLOR_CORRECTION_MODE_HIGH_QUALITY,

	CAM_EXT_COLOR_CORRECTION_MODE_MAX =
		CAM_EXT_COLOR_CORRECTION_MODE_HIGH_QUALITY,
};

/* float [4] ==> RGGB gains */
#define CAM_EXT_CID_COLOR_CORRECTION_GAINS (CID_CAM_EXT_CLASS_BASE + 48)

/* float [3][3] ==> matrix transform RGB to sRGB */
#define CAM_EXT_CID_COLOR_CORRECTION_TRANSFORM (CID_CAM_EXT_CLASS_BASE + 49)

/* int menu */
#define CAM_EXT_CID_EDGE_MODE (CID_CAM_EXT_CLASS_BASE + 50)
enum {
	CAM_EXT_EDGE_MODE_OFF,
	CAM_EXT_EDGE_MODE_FAST,
	CAM_EXT_EDGE_MODE_HIGH_QUALITY,
	CAM_EXT_EDGE_MODE_ZERO_SHUTTER_LAG,

	CAM_EXT_EDGE_MODE_MAX = CAM_EXT_EDGE_MODE_ZERO_SHUTTER_LAG,
};

/* float menu, need to read list of float from mod */
#define CAM_EXT_CID_LENS_APERTURES (CID_CAM_EXT_CLASS_BASE + 51)

/* float menu, need to read list of float from mod */
#define CAM_EXT_CID_LENS_FILTER_DENSITY (CID_CAM_EXT_CLASS_BASE + 52)

/* int menu */
#define CAM_EXT_CID_NOISE_REDUCTION_MODE (CID_CAM_EXT_CLASS_BASE + 53)
enum {
	CAM_EXT_NOISE_REDUCTION_MODE_OFF,
	CAM_EXT_NOISE_REDUCTION_MODE_FAST,
	CAM_EXT_NOISE_REDUCTION_MODE_HIGH_QUALITY,
	CAM_EXT_NOISE_REDUCTION_MODE_MINIMAL,
	CAM_EXT_NOISE_REDUCTION_MODE_ZERO_SHUTTER_LAG,

	CAM_EXT_NOISE_REDUCTION_MODE_MAX =
		CAM_EXT_NOISE_REDUCTION_MODE_ZERO_SHUTTER_LAG,
};

/* int (0,1) read only */
#define CAM_EXT_CID_REQUEST_MAX_NUM_INPUT_STREAM (CID_CAM_EXT_CLASS_BASE + 54)

/* int >= 1 read only */
#define CAM_EXT_CID_REQUEST_PARTIAL_RESULT_COUNT (CID_CAM_EXT_CLASS_BASE + 55)

/* integer64 */
#define CAM_EXT_CID_SENSOR_EXPOSURE_TIME (CID_CAM_EXT_CLASS_BASE + 56)

/* intger64 */
#define CAM_EXT_CID_SENSOR_FRAME_DURATION (CID_CAM_EXT_CLASS_BASE + 57)

/* integer ISO*/
#define CAM_EXT_CID_SENSOR_SENSITIVITY (CID_CAM_EXT_CLASS_BASE + 58)

/* integer read only */
#define CAM_EXT_CID_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT \
		(CID_CAM_EXT_CLASS_BASE + 59)
enum {
	CAM_EXT_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_RGGB,
	CAM_EXT_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GRBG,
	CAM_EXT_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_GBRG,
	CAM_EXT_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_BGGR,
	CAM_EXT_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_RGB,

	CAM_EXT_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_MAX =
		CAM_EXT_SENSOR_INFO_COLOR_FILTER_ARRANGEMENT_RGB,
};

/* integer read only */
#define CAM_EXT_CID_SENSOR_MAX_ANALOG_SENSITIVITY \
		(CID_CAM_EXT_CLASS_BASE + 60)

/* int menu */
#define CAM_EXT_CID_SHADING_MODE (CID_CAM_EXT_CLASS_BASE + 61)
enum {
	CAM_EXT_SHADING_MODE_OFF,
	CAM_EXT_SHADING_MODE_FAST,
	CAM_EXT_SHADING_MODE_HIGH_QUALITY,

	CAM_EXT_SHADING_MODE_MAX = CAM_EXT_SHADING_MODE_HIGH_QUALITY,
};

/* int menu */
#define CAM_EXT_CID_STATISTICS_LENS_SHADING_MAP_MODE \
		(CID_CAM_EXT_CLASS_BASE + 62)
enum {
	CAM_EXT_CID_STATISTICS_LENS_SHADING_MAP_MODE_OFF,
	CAM_EXT_CID_STATISTICS_LENS_SHADING_MAP_MODE_ON,

	CAM_EXT_CID_STATISTICS_LENS_SHADING_MAP_MODE_MAX =
		CAM_EXT_CID_STATISTICS_LENS_SHADING_MAP_MODE_ON,
};

/* float of array [M][3][2], size is from MOD */
#define CAM_EXT_CID_TONEMAP_CURVE (CID_CAM_EXT_CLASS_BASE + 63)

/* float */
#define CAM_EXT_CID_TONEMAP_GAMMA (CID_CAM_EXT_CLASS_BASE + 64)

/* int menu */
#define CAM_EXT_CID_TONEMAP_MODE (CID_CAM_EXT_CLASS_BASE + 65)
enum {
	CAM_EXT_TONEMAP_MODE_CONTRAST_CURVE,
	CAM_EXT_TONEMAP_MODE_FAST,
	CAM_EXT_TONEMAP_MODE_HIGH_QUALITY,
	CAM_EXT_TONEMAP_MODE_GAMMA_VALUE,
	CAM_EXT_TONEMAP_MODE_PRESET_CURVE,

	CAM_EXT_TONEMAP_MODE_MAX =
		CAM_EXT_TONEMAP_MODE_PRESET_CURVE,
};

/* int menu */
#define CAM_EXT_CID_TONEMAP_PRESET_CURVE (CID_CAM_EXT_CLASS_BASE + 66)
enum {
	CAM_EXT_TONEMAP_PRESET_CURVE_SRGB,
	CAM_EXT_TONEMAP_PRESET_CURVE_REC709,

	CAM_EXT_TONEMAP_PRESET_CURVE_MAX = CAM_EXT_TONEMAP_PRESET_CURVE_REC709,
};

/* integer read only */
#define CAM_EXT_CID_TONEMAP_MAX_CURVE_POINSTS (CID_CAM_EXT_CLASS_BASE + 67)

/* list of integer [4] x,y,w,h */
#define CAM_EXT_CID_AE_REGIONS (CID_CAM_EXT_CLASS_BASE + 68)

/* list of integer [4] x,y,w,h */
#define CAM_EXT_CID_AF_REGIONS (CID_CAM_EXT_CLASS_BASE + 69)

/* list of integer [4] x,y,w,h */
#define CAM_EXT_CID_AWB_REGIONS (CID_CAM_EXT_CLASS_BASE + 70)

/* int menu */
#define CAM_EXT_CID_HOT_PIXEL_MODE (CID_CAM_EXT_CLASS_BASE + 71)
enum {
	CAM_EXT_HOT_PIXEL_MODE_OFF,
	CAM_EXT_HOT_PIXEL_MODE_FAST,
	CAM_EXT_HOT_PIXEL_MODE_HIGH_QUALITY,

	CAM_EXT_HOT_PIXEL_MODE_MAX = CAM_EXT_HOT_PIXEL_MODE_HIGH_QUALITY,
};

/* float [5] f_x, f_y, c_x, c_y, s read only */
#define CAM_EXT_CID_LENS_INTRINSIC_CALIBRATION (CID_CAM_EXT_CLASS_BASE + 72)

/* float [7] x, y, z, w, a_x, a_y, a_z read only */
#define CAM_EXT_CID_LENS_POSE_ROTATION (CID_CAM_EXT_CLASS_BASE + 73)

/* float [3] x, y, z read only */
#define CAM_EXT_CID_LENS_POSE_TRANSLATION (CID_CAM_EXT_CLASS_BASE + 74)

/* float [6] kappa_0, kappa_1, kappa_2, kappa_3, kappa_4, kappa_5 read only */
#define CAM_EXT_CID_LENS_RADIAL_DISTORTION (CID_CAM_EXT_CLASS_BASE + 75)

/* integer [4] rggb, solid color test pattern */
#define CAM_EXT_CID_SENSOR_TEST_PATTERN_DATA (CID_CAM_EXT_CLASS_BASE + 76)

/* int menu */
#define CAM_EXT_CID_SENSOR_TEST_PATTERN_MODE (CID_CAM_EXT_CLASS_BASE + 77)
enum {
	CAM_EXT_SENSOR_TEST_PATTERN_MODE_OFF,
	CAM_EXT_SENSOR_TEST_PATTERN_MODE_SOLID_COLOR,
	CAM_EXT_SENSOR_TEST_PATTERN_MODE_COLOR_BARS,
	CAM_EXT_SENSOR_TEST_PATTERN_MODE_COLOR_BARS_FADE_TO_GRAY,
	CAM_EXT_SENSOR_TEST_PATTERN_MODE_PN9,
	CAM_EXT_SENSOR_TEST_PATTERN_MODE_CUSTOM1,

	CAM_EXT_SENSOR_TEST_PATTERN_MODE_MAX =
		CAM_EXT_SENSOR_TEST_PATTERN_MODE_CUSTOM1,
};

/* integer [4] read only */
#define CAM_EXT_CID_SENSOR_BLACK_LEVEL_PATTERN (CID_CAM_EXT_CLASS_BASE + 78)

/* float[3][3] read only */
#define CAM_EXT_CID_SENSOR_CALIBRATION_TRANSFORM1 \
		(CID_CAM_EXT_CLASS_BASE + 79)

/* float[3][3] read only */
#define CAM_EXT_CID_SENSOR_CALIBRATION_TRANSFORM2 \
		(CID_CAM_EXT_CLASS_BASE + 80)

/* float[3][3] read only */
#define CAM_EXT_CID_SENSOR_COLOR_TRANSFORM1 (CID_CAM_EXT_CLASS_BASE + 81)

/* float[3][3] read only */
#define CAM_EXT_CID_SENSOR_COLOR_TRANSFORM2 (CID_CAM_EXT_CLASS_BASE + 82)

/* float[3][3] read only */
#define CAM_EXT_CID_SENSOR_FORWARD_MATRIX1 (CID_CAM_EXT_CLASS_BASE + 83)

/* float[3][3] read only */
#define CAM_EXT_CID_SENSOR_FORWARD_MATRIX2 (CID_CAM_EXT_CLASS_BASE + 84)

/* boolean read only */
#define CAM_EXT_CID_SENSOR_INFO_LENS_SHADING_APPLIED \
		(CID_CAM_EXT_CLASS_BASE + 85)

/* integer > 255 read only */
#define CAM_EXT_CID_SENSOR_INFO_WHITE_LEVEL (CID_CAM_EXT_CLASS_BASE + 86)

/* int read only */
#define CAM_EXT_CID_SENSOR_PREFERENCE_ILLUMINANT1 (CID_CAM_EXT_CLASS_BASE + 87)
enum {
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_DAYLIGHT,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_FLUORESCENT,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_TUNGSTEN,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_FLASH,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_FINE_WEATHER,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_CLOUDY_WHEATHER,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_SHADE,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_DAYLIGHT_FLUORESCENT,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_DAY_WHITE_FLUORESCENT,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_COOL_WHITE_FLUORESCENT,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_WHITE_FLUORESCENT,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_STANDARD_A,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_STANDARD_B,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_STANDARD_C,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_D55,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_D65,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_D75,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_D50,
	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_ISO_STUDIO_TUNGSTEN,

	CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_MAX =
		CAM_EXT_SENSOR_PREFERENCE_ILLUMINANT_ISO_STUDIO_TUNGSTEN,
};

/* int read only */
#define CAM_EXT_CID_SENSOR_PREFERENCE_ILLUMINANT2 (CID_CAM_EXT_CLASS_BASE + 88)

/* boolean */
#define CAM_EXT_CID_STATISTICS_HOT_PIXEL_MAP_MODE (CID_CAM_EXT_CLASS_BASE + 89)

/* integer [M][2] */
#define CAM_EXT_CID_HOT_PIXEL_MAP (CID_CAM_EXT_CLASS_BASE + 90)

/* bit mask, write only */
#define CAM_EXT_CID_CAPTURE (CID_CAM_EXT_CLASS_BASE + 91)

enum {
	CAM_EXT_CID_CAPTURE_STILL_CAPTURE       = 1,
	CAM_EXT_CID_CAPTURE_VIDEO_SNAPSHOT      = 1 << 1,
	CAM_EXT_CID_CAPTURE_ZSL_CAPTURE         = 1 << 2,
	CAM_EXT_CID_CAPTURE_RAW                 = 1 << 3,
	CAM_EXT_CID_CAPTURE_JPG                 = 1 << 4,
	CAM_EXT_CID_CAPTURE_BURST               = 1 << 5,

	CAM_EXT_CID_CAPTURE_MAX = CAM_EXT_CID_CAPTURE_STILL_CAPTURE
				| CAM_EXT_CID_CAPTURE_VIDEO_SNAPSHOT
				| CAM_EXT_CID_CAPTURE_ZSL_CAPTURE
				| CAM_EXT_CID_CAPTURE_RAW
				| CAM_EXT_CID_CAPTURE_JPG
				| CAM_EXT_CID_CAPTURE_BURST
};

/* int menu, as extension to CAM_EXT_CID_AF_MODE */
#define CAM_EXT_CID_AF_MODE_EXT (CID_CAM_EXT_CLASS_BASE + 92)

enum {
	CAM_EXT_AF_MODE_EXT_NULL,
	CAM_EXT_AF_MODE_EXT_INFINITY,
	CAM_EXT_AF_MODE_EXT_FIXED,

	CAM_EXT_AF_MODE_EXT_MAX = CAM_EXT_AF_MODE_EXT_FIXED,
};

/* int menu */
#define CAM_EXT_CID_ISO (CID_CAM_EXT_CLASS_BASE + 93)

enum {
	CAM_EXT_ISO_AUTO,
	CAM_EXT_ISO_50,
	CAM_EXT_ISO_100,
	CAM_EXT_ISO_200,
	CAM_EXT_ISO_400,
	CAM_EXT_ISO_800,
	CAM_EXT_ISO_1600,
	CAM_EXT_ISO_3200,

	CAM_EXT_ISO_MAX = CAM_EXT_ISO_3200,
};

/* int menu */
#define CAM_EXT_CID_ND_FILTER (CID_CAM_EXT_CLASS_BASE + 94)

enum {
	CAM_EXT_ND_FILTER_AUTO,
	CAM_EXT_ND_FILTER_ON,
	CAM_EXT_ND_FILTER_OFF,

	CAM_EXT_ND_FILTER_MAX = CAM_EXT_ND_FILTER_OFF,
};

/* integer */
#define CAM_EXT_CID_JPEG_SHARPNESS (CID_CAM_EXT_CLASS_BASE + 95)

/* integer */
#define CAM_EXT_CID_JPEG_CONTRAST (CID_CAM_EXT_CLASS_BASE + 96)

/* integer */
#define CAM_EXT_CID_JPEG_SATURATION (CID_CAM_EXT_CLASS_BASE + 97)

/* integer64 */
#define CAM_EXT_CID_TIME_SYNC (CID_CAM_EXT_CLASS_BASE + 98)

/* integer64 */
#define CAM_EXT_CID_JPEG_GPS_TIMESTAMP (CID_CAM_EXT_CLASS_BASE + 99)

/* string */
#define CAM_EXT_CID_JPEG_GPS_PROC_METHOD (CID_CAM_EXT_CLASS_BASE + 100)

/* int menu */
#define CAM_EXT_CID_FACE_DETECTION (CID_CAM_EXT_CLASS_BASE + 101)

enum {
	CAM_EXT_CID_FACE_DETECTION_STOP,
	CAM_EXT_CID_FACE_DETECTION_START,

	CAM_EXT_CID_FACE_DETECTION_MAX = CAM_EXT_CID_FACE_DETECTION_START,
};

/* int[3] ready only: [has_uvc, vid, pid] */
#define CAM_EXT_CID_MOD_CAPS_UVC_SNAPSHOT (CID_CAM_EXT_CLASS_BASE + 102)

/* int read only */
#define CAM_EXT_CID_MOD_META_DATA_PATH (CID_CAM_EXT_CLASS_BASE + 103)
enum {
	CAM_EXT_CID_MOD_META_DATA_PATH_NONE,
	CAM_EXT_CID_MOD_META_DATA_PATH_CSI,
	CAM_EXT_CID_MOD_META_DATA_PATH_GB,

	CAM_EXT_CID_MOD_META_DATA_PATH_MAX = CAM_EXT_CID_MOD_META_DATA_PATH_GB,
};

/* int, read only. For GB path, it's packet size. For CSI path, it's line num */
#define CAM_EXT_CID_MOD_META_DATA_SIZE (CID_CAM_EXT_CLASS_BASE + 104)

/* boolean */
#define CAM_EXT_CID_ZOOM_LOCK_1X (CID_CAM_EXT_CLASS_BASE + 105)

/* boolean */
#define CAM_EXT_CID_SHUTTER_LOCK (CID_CAM_EXT_CLASS_BASE + 106)

/* string read only */
#define CAM_EXT_CID_MODEL_NUMBER (CID_CAM_EXT_CLASS_BASE + 107)

/* string read only */
#define CAM_EXT_CID_FIRMWARE_VERSION (CID_CAM_EXT_CLASS_BASE + 108)

/* int menu */
#define CAM_EXT_CID_AE_MODE_EXT (CID_CAM_EXT_CLASS_BASE + 109)
enum {
	CAM_EXT_AE_MODE_EXT_NULL,
	CAM_EXT_AE_MODE_EXT_OFF_FLASH,
	CAM_EXT_AE_MODE_EXT_OFF_FLASH_REDEYE,

	CAM_EXT_AE_MODE_EXT_MAX = CAM_EXT_AE_MODE_EXT_OFF_FLASH_REDEYE,
};

/* int menu */
#define CAM_EXT_CID_SCENE_MODE_EXT (CID_CAM_EXT_CLASS_BASE + 110)
enum {
	CAM_EXT_SCENE_MODE_EXT_NULL,
	CAM_EXT_SCENE_MODE_EXT_AUTO_HDR,
	CAM_EXT_SCENE_MODE_EXT_BACKLIGHT_PORTRAIT,
	CAM_EXT_SCENE_MODE_EXT_CLOSEUP,
	CAM_EXT_SCENE_MODE_EXT_DUSK_DAWN,
	CAM_EXT_SCENE_MODE_EXT_FOOD,
	CAM_EXT_SCENE_MODE_EXT_NIGHT_LANDSCAPE,
	CAM_EXT_SCENE_MODE_EXT_PET_PORTRAIT,

	CAM_EXT_SCENE_MODE_EXT_MAX = CAM_EXT_SCENE_MODE_EXT_PET_PORTRAIT,
};

/* int 0 - 100 */
#define CAM_EXT_CID_MANUAL_FOCUS_POSITION (CID_CAM_EXT_CLASS_BASE + 111)

/* integer, limit the max zoom level, 100 means 1x, 1000 means 10x */
#define CAM_EXT_CID_ZOOM_LIMIT (CID_CAM_EXT_CLASS_BASE + 112)

/* boolean */
#define CAM_EXT_CID_FOCUS_KEY_LOCK (CID_CAM_EXT_CLASS_BASE + 113)

/* integer [][4]: eis_w, eis_h, w, h. optional for MOD */
#define CAM_EXT_CID_EIS_FRAME_SIZE_MAP (CID_CAM_EXT_CLASS_BASE + 114)

/* integer [][2]: w, h for thumbnail */
#define CAM_EXT_CID_JPEG_AVAILABLE_THUMBNAIL_SIZES \
		(CID_CAM_EXT_CLASS_BASE + 115)

/* int: index of thumbnail in JPEG_AVAILABLE_THUMBNAIL_SIZE, [0...] */
#define CAM_EXT_CID_JPEG_THUMBNAIL_SIZE_INDEX (CID_CAM_EXT_CLASS_BASE + 116)

/* string */
#define CAM_EXT_CID_PHONE_VERSION (CID_CAM_EXT_CLASS_BASE + 117)

/* int, mask for key capability, read only */
#define CAM_EXT_CID_SUPPLEMENTAL_KEY_MASK (CID_CAM_EXT_CLASS_BASE + 118)
enum {
	CAM_EXT_HW_KEY_POWER            = 1,
	CAM_EXT_HW_KEY_POWER_EVT        = 1 << 1,
	CAM_EXT_HW_KEY_ZOOM_IN          = 1 << 2,
	CAM_EXT_HW_KEY_ZOOM_IN_EVT      = 1 << 3,
	CAM_EXT_HW_KEY_ZOOM_OUT         = 1 << 4,
	CAM_EXT_HW_KEY_ZOOM_OUT_EVT     = 1 << 5,
	CAM_EXT_HW_KEY_FOCUS            = 1 << 6,
	CAM_EXT_HW_KEY_FOCUS_EVT        = 1 << 7,
	CAM_EXT_HW_KEY_CAMERA           = 1 << 8,
	CAM_EXT_HW_KEY_CAMERA_EVT       = 1 << 9,

	CAM_EXT_HW_KEY_MAX = CAM_EXT_HW_KEY_POWER
					| CAM_EXT_HW_KEY_POWER_EVT
					| CAM_EXT_HW_KEY_ZOOM_IN
					| CAM_EXT_HW_KEY_ZOOM_IN_EVT
					| CAM_EXT_HW_KEY_ZOOM_OUT
					| CAM_EXT_HW_KEY_ZOOM_OUT_EVT
					| CAM_EXT_HW_KEY_FOCUS
					| CAM_EXT_HW_KEY_FOCUS_EVT
					| CAM_EXT_HW_KEY_CAMERA
					| CAM_EXT_HW_KEY_CAMERA_EVT
};

/* interger, to group controls */
#define CAM_EXT_CID_GROUP_IND (CID_CAM_EXT_CLASS_BASE + 119)
enum {
	CAM_EXT_CID_GROUP_IND_BEGIN,
	CAM_EXT_CID_GROUP_IND_END,

	CAM_EXT_CID_GROUP_IND_MAX = CAM_EXT_CID_GROUP_IND_END,
};

/* boolean */
#define CAM_EXT_CID_VIDEO_RECORD_HINT (CID_CAM_EXT_CLASS_BASE + 120)

/* float[3] r/g/b gain used in raw to yuv conversion */
#define CAM_EXT_CID_RAW_TO_YUV_GAIN (CID_CAM_EXT_CLASS_BASE + 121)

/* int menu */
#define CAM_EXT_CID_EFFECT_MODE_EXT (CID_CAM_EXT_CLASS_BASE + 122)
enum {
	CAM_EXT_EFFECT_MODE_EXT_NULL,
	CAM_EXT_EFFECT_MODE_EXT_BLACK_BLUE,
	CAM_EXT_EFFECT_MODE_EXT_PURE,
	CAM_EXT_EFFECT_MODE_EXT_MIRROR,
	CAM_EXT_EFFECT_MODE_EXT_BUBBLE,
	CAM_EXT_EFFECT_MODE_EXT_NEON,
	CAM_EXT_EFFECT_MODE_EXT_CARTOON,
	CAM_EXT_EFFECT_MODE_EXT_SOFT,
	CAM_EXT_EFFECT_MODE_EXT_DIORAMA,

	CAM_EXT_EFFECT_MODE_EXT_MAX = CAM_EXT_EFFECT_MODE_EXT_DIORAMA,
};

/* int */
#define CAM_EXT_CID_ZSL_BUFFER_DEPTH (CID_CAM_EXT_CLASS_BASE + 123)

/* string */
#define CAM_EXT_CID_CUSTOM_PARAMETER (CID_CAM_EXT_CLASS_BASE + 124)

#endif /* __CAMERA_EXT_CTRLS_H */
