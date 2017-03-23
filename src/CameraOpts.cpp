/*=============================================================================
  DNG4PS - software for converting Canon PS RAW files to DNG ones
  Copyright (C) 2008-2013 Denis Artyomov

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  =============================================================================
  File:      CameraOpts.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
			 Matt Dawson (coder@dawsons.id.au)
  ===========================================================================*/

#include "pch.h"

#include <stdlib.h>

#include "CameraOpts.h"
#include "dng_tag_values.h"

namespace
{
    const wchar_t *app_name_opt_str             = L"dng4ps2";
    const wchar_t *cam_count_opt_str            = L"CameraCount";
    const wchar_t *name_opt_str                 = L"Name";
    const wchar_t *width_opt_str                = L"SensorWidth";
    const wchar_t *height_opt_str               = L"SensorHeight";
    const wchar_t *cropped_width_opt_str        = L"CroppedWidth";
    const wchar_t *cropped_height_opt_str       = L"CroppedHeight";
    const wchar_t *active_origin_x_opt_str      = L"ActiveOriginX";
    const wchar_t *active_origin_y_opt_str      = L"ActiveOriginY";
    const wchar_t *active_origin_right_opt_str  = L"ActiveOriginRight";
    const wchar_t *active_origin_bottom_opt_str = L"ActiveOriginBottom";
    const wchar_t *black_level_opt_str          = L"BlackLevel";
    const wchar_t *white_level_opt_str          = L"WhiteLevel";
    const wchar_t *mosaic_opt_str               = L"Mosaic";
    const wchar_t *illum_opt_str                = L"Illuminant";
    const wchar_t *matrix_opt_str               = L"Matrix";
    const wchar_t *mult_opt_str                 = L"Mult";
    const wchar_t *id_opt_str                   = L"Id";
    const wchar_t *gr_count_opt_str             = L"GroupsCount";
	const wchar_t *file_size_opt_str            = L"FileSize";
	const wchar_t *short_name_opt_str           = L"ShortName";
	const wchar_t *bpu_str                      = L"BitsPerUnit";

	struct CameraDefValue
	{
		const wchar_t * model_name;
		const wchar_t * short_name;

		unsigned int width, height;
		unsigned int active_origin_x, active_origin_y;
		unsigned int active_origin_right, active_origin_bottom;

		unsigned int cropped_width, cropped_height;

		MosaicType mosaic;

		int illum1;
		float color_matrix1[9];
		float color_matrix1_mult[3];

		int illum2;
		float color_matrix2[9];
		float color_matrix2_mult[3];

		BitsPerUnit bits_per_unit;

		const wchar_t * id;
	};

	const CameraDefValue camera_def_values[] =
	{
		{	L"Canon PowerShot A610", 
			L"PowerShot A610",
			2672,1968, 14,10,46,2, 2592,1944,  MOSAIC_RGGB,
			17, {15591.0f,-6402.0f,-1592.0f, -5365.0f,13198.0f,2168.0f, -1300.0f,1824.0f,5075.0f }, {1.1f, 1.0f, 0.9f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90001}" },

		{	L"Canon PowerShot S2 IS", 
			L"PowerShot S2 IS",
			2672,1968, 14,10,46,2, 2592,1944,  MOSAIC_RGGB, // checked
			lsDaylight, {0.580618f,-0.182631f,-0.094502f, -0.192656f,0.650899f,0.058592f, -0.015379f,0.089453f,0.256275f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90002}" },

		{	L"Canon PowerShot A530", 
			L"PowerShot A530",
			2672,1968, 12,20,44,0, 2592,1944,  MOSAIC_RGGB, // checked
			lsDaylight, {0.725264f,-0.240542f,-0.122310f, -0.210248f,0.656077f,0.052390f, -0.011282f,0.070400f,0.300712f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90003}" },

		{	L"Canon PowerShot A620", 
			L"PowerShot A620",
			3152,2340, 38,14,6,2, 3072,2304,  MOSAIC_RGGB, // checked
			17, {15265.0f,-6193.0f,-1558.0f, -4125.0f,12116.0f,2010.0f, -888.0f,1639.0f,5220.0f}, {1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90004}" },

		{	L"Canon DIGITAL IXUS 850 IS / Canon PowerShot SD800 IS", 
			L"IXUS 850 IS / PowerShot SD800 IS",
			3152,2340, 38,14,6,2, 3072,2304,  MOSAIC_RGGB, // checked
			lsDaylight, { 0.661014f,-0.189364f,-0.115797f, -0.168772f,0.661827f,0.047392f, -0.035846f,0.107148f,0.233705f },
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90005}" },

		{	L"Canon DIGITAL IXUS 70 / Canon PowerShot SD1000",
			L"IXUS 70 / PowerShot SD1000",
			3152,2340, 38,14,6,2, 3072,2304,  MOSAIC_RGGB, // checked
			lsDaylight, {0.652674f,-0.172074f,-0.107575f, -0.139063f,0.594517f,0.060252f, -0.009088f,0.082013f,0.238048f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90006}" },

		{	L"Canon PowerShot A710 IS", 
			L"PowerShot A710 IS",
			3152,2340, 38,14,6,2, 3072,2304,  MOSAIC_RGGB,
			lsDaylight, {0.664401f,-0.206085f,-0.105935f, -0.208489f,0.655900f,0.051953f, -0.051591f,0.115760f,0.234946f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90007}" },

		{	L"Canon PowerShot A560", 
			L"PowerShot A560",
			3152,2340, 38,14,6,2, 3072,2304,  MOSAIC_RGGB,
			17, {15265.0f,-6193.0f,-1558.0f, -4125.0f,12116.0f,2010.0f, -888.0f,1639.0f,5220.0f}, {1.0f, 1.0f, 1.0f},
			-1,	{}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90008}" },

		{	L"Canon PowerShot A570 IS", 
			L"PowerShot A570 IS",
			3152,2340, 38,14,6,2, 3072,2304,  MOSAIC_RGGB,
			lsDaylight, {0.528283f,-0.144259f,-0.085966f, -0.202789f,0.736563f,0.073008f, -0.027130f,0.107702f,0.264543f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90009}" },

		{	L"Canon DIGITAL IXUS 700 / Canon PowerShot SD500", 
			L"IXUS 700 / PowerShot SD500",
			3152,2340, 38,14,6,2, 3072,2304,  MOSAIC_RGGB,
			17, {15265.0f,-6193.0f,-1558.0f, -4125.0f,12116.0f,2010.0f, -888.0f,1639.0f,5220.0f}, // BAD!
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9000A}" },

		{	L"Canon PowerShot A630", 
			L"PowerShot A630",
			3344,2484, 14,8,46,8, 3264,2448,  MOSAIC_RGGB, // checked
			lsDaylight, { 14201.0f,-5308.0f,-1757.0f, -6087.0f,14472.0f,1617.0f, -2191.0f,3105.0f,5348.0f }, // from DCRAW
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9000B}" },

		{	L"Canon PowerShot A720 IS", 
			L"PowerShot A720 IS",
			3336,2480, 10,8,34,6, 3264,2448,  MOSAIC_GBRG, // checked
			lsDaylight, {14573.0f,-5482.0f,-1546.0f, -1266.0f,9799.0f,1468.0f, -1040.0f,1912.0f,3810.0f}, // from DCRAW
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9000C}" },

		{	L"Canon PowerShot S5 IS", 
			L"PowerShot S5 IS",
			3336,2480, 10,8,34,6, 3264,2448,  MOSAIC_GBRG, // checked
			lsDaylight, {0.650591f,-0.199585f,-0.123118f, -0.069617f,0.583926f,0.034354f, -0.019113f,0.082163f,0.210786f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9000D}" },

		{	L"Canon DIGITAL IXUS 950 IS / Canon PowerShot SD850 IS", 
			L"IXUS 950 IS / PowerShot SD850 IS",
			3336,2480, 10,8,34,6, 3264,2448,  MOSAIC_GBRG, // checked
			lsDaylight, {14573.0f,-5482.0f,-1546.0f, -1266.0f,9799.0f,1468.0f, -1040.0f,1912.0f,3810.0f},  // BAD!
			{1.0f, 1.0f, 1.0f}, 
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9000E}" },

		{	L"Canon PowerShot A640", 
			L"PowerShot A640",
			3736,2772, 14,8,54,8, 3648,2736,  MOSAIC_RGGB,
			17, {13124.0f,-5329.0f,-1390.0f, -3602.0f,11658.0f,1944.0f, -1612.0f,2863.0f,4885.0f }, // from DCRAW
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9000F}" },

		{	L"Canon PowerShot G7", 
			L"PowerShot G7",
			3736,2772, 14,8,54,8, 3648,2736,  MOSAIC_RGGB, // checked
			lsDaylight, { 0.575419f,-0.185557f,-0.077898f, -0.213702f,0.733569f,0.081514f, -0.024604f,0.131906f,0.280378f },
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90010}" },

		{	L"Canon PowerShot S3 IS", 
			L"PowerShot S3 IS",
			2888,2136, 44,8,4,0, 2816,2112,  MOSAIC_RGGB, // checked
			17, {14062.0f,-5199.0f,-1446.0f, -4712.0f,12470.0f,2243.0f, -1286.0f,2028.0f,4836.0f }, // from DCRAW
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90011}" },

		{	L"Canon PowerShot A700", 
			L"PowerShot A700",
			2888,2136, 44,8,4,0, 2816,2112,  MOSAIC_RGGB,
			17, {14062.0f,-5199.0f,-1446.0f, -4712.0f,12470.0f,2243.0f, -1286.0f,2028.0f,4836.0f }, // BAD!
			{1.0f, 1.0f, 1.0f}, 
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90012}" },

		{	L"Canon DIGITAL IXUS 800 IS / Canon PowerShot SD700 IS", 
			L"IXUS 800 IS / PowerShot SD700 IS",
			2888,2136, 44,8,4,0, 2816,2112,  MOSAIC_RGGB,
			17, {0.796464f,-0.214669f,-0.122599f, -0.182793f,0.736859f,0.075607f, 0.020593f,0.080194f,0.277992f }, 
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90013}" },

		{	L"Canon PowerShot A540", 
			L"PowerShot A540",
			2888,2136, 44,8,4,0, 2816,2112,  MOSAIC_RGGB,
			lsDaylight, {0.687147f,-0.201964f,-0.125024f, -0.148403f,0.566810f,0.045401f, -0.009472f,0.063186f,0.208602f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90015}" },

		{	L"Canon PowerShot A650 IS", 
			L"PowerShot A650 IS",
			4104,3048, 52,14,28,18, 4000,3000,  MOSAIC_RGGB,
			lsDaylight, {9427,-3036,-959, -2581,10671,1911, -1039,1982,4430}, // from DCRAW
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90016}" },

		{	L"Canon PowerShot A460", 
			L"PowerShot A460",
			2664,1968, 6,6,46,6, 2592,1944,  MOSAIC_RGGB,
			lsDaylight, {0.649324f,-0.233893f,-0.088521f, -0.158955f,0.593407f,0.069775f, -0.044551f,0.136891f,0.254362f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90017}" },

		{	L"Canon DIGITAL IXUS 860 IS / Canon PowerShot SD870 IS", 
			L"IXUS 860 IS / PowerShot SD870 IS",
			3336,2480, 10,8,34,6, 3264,2448,  MOSAIC_GBRG,
			lsDaylight, {0.747939f, -0.268233f, -0.129266f,
						-0.050633f, 0.515687f, 0.023509f,
						-0.021341f, 0.076495f, 0.186385f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90018}" },

		{	L"Canon PowerShot TX1", 
			L"PowerShot TX1",
			3152,2340, 12,8,44,8, 3072,2304,  MOSAIC_RGGB, // checked
			lsDaylight, {0.510370f,-0.068998f,-0.086859f, -0.279980f,0.766686f,0.067944f, -0.014382f,0.113688f,0.239853f}, // poor?
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90019}" },

		{	L"Canon PowerShot SX100 IS", 
			L"PowerShot SX100 IS",
			3336,2480, 10,8,34,6, 3264,2448,  MOSAIC_GBRG,
			lsDaylight, {0.656793f,-0.168702f,-0.108030f, -0.002711f,0.661538f,0.037919f, 0.077255f,0.048834f,0.241797f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9001A}" },

		{	L"Canon Digital IXUS 60 / Canon PowerShot SD600", 
			L"IXUS 60 / PowerShot SD600",
			2888,2136, 44,8,4,0, 2816,2112,  MOSAIC_RGGB,
			lsDaylight, {0.767398f,-0.227658f,-0.144213f, -0.217466f,0.669319f,0.038180f, -0.023152f,0.077743f,0.238669f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9001B}" },

		{	L"Canon DIGITAL IXUS 750 / Canon PowerShot SD550", 
			L"IXUS 750 / PowerShot SD550",
			3152,2340, 36,12,4,0, 3072,2304,  MOSAIC_RGGB, // checked
			lsDaylight, {0.580280f,-0.172239f,-0.089707f, -0.206596f,0.634926f,0.063877f, 0.010377f,0.062053f,0.242646f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9001C}" },

		{	L"Canon DIGITAL IXUS 65 / Canon PowerShot SD630", 
			L"IXUS 65 / PowerShot SD630",
			2888,2136, 44,8,4,0, 2816,2112,  MOSAIC_RGGB,
			lsDaylight, {0.783384f,-0.227078f,-0.127919f, -0.181247f,0.661743f,0.058868f, 0.025727f,0.043891f,0.247655f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9001D}" },

		{	L"Canon PowerShot A450", 
			L"PowerShot A450",
			2664,1968, 6,6,46,6, 2592,1944,  MOSAIC_RGGB,
			lsDaylight, {0.652279f,-0.199279f,-0.101833f, -0.159819f,0.619308f,0.078172f, -0.058827f,0.150963f,0.264216f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9001E}" },

		{	L"Canon DIGITAL IXUS 960 IS / Canon PowerShot SD950 IS",
			L"IXUS 960 IS / PowerShot SD950 IS",
			4104,3048, 48,12,24,12, 4000,3000,  MOSAIC_RGGB,
			lsDaylight, {0.764117f,-0.223884f,-0.116036f, -0.035875f,0.582534f,0.032982f, 0.010441f,0.064164f,0.198520f}, // my
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9001F}" },

		{	L"Canon DIGITAL IXUS 40 / Canon PowerShot SD300", 
			L"IXUS 40 / PowerShot SD300",
			2400,1766, 12,12,44,2, 2272,1704,  MOSAIC_RGGB,
			lsDaylight, {0.652674f,-0.172074f,-0.107575f, -0.139063f,0.594517f,0.060252f, -0.009088f,0.082013f,0.23808f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90020}" },

		{	L"Canon DIGITAL IXUS 55 / Canon PowerShot SD450", 
			L"IXUS 55 / PowerShot SD450",
			2672,1968, 12,8,44,0, 2592,1944,  MOSAIC_RGGB, // Checked
			lsDaylight, {0.536034f,-0.173429f,-0.089823f, -0.290416f,0.735807f,0.047894f, -0.071455f,0.114314f,0.274533f}, // is good enough
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90021}" },

		{	L"Canon DIGITAL IXUS 80 IS / Canon PowerShot SD1100 IS", 
			L"IXUS 80 IS / PowerShot SD1100 IS",
			3336,2480, 6,6,32,4, 3264,2448,  MOSAIC_GBRG,
			lsDaylight, {0.723425f,-0.262760f,-0.106712f, -0.005402f,0.476882f,0.037287f, 0.021408f,0.039033f,0.194852f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90022}" },

		{	L"Canon PowerShot A550", 
			L"PowerShot A550",
			3152,2340, 36,12,4,0, 3072,2304,  MOSAIC_RGGB,
			lsDaylight, {0.673251f,-0.200684f,-0.098680f, -0.163638f,0.651247f,0.074004f, 0.014221f,0.052979f,0.265291f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90023}" },

		{	L"Canon PowerShot A590 IS", 
			L"PowerShot A590 IS",
			3336,2480, 8,8,32,0, 3264,2448,  MOSAIC_GBRG, // checked
			lsDaylight, {0.726857f,-0.176454f,-0.124118f, -0.071340f,0.592001f,0.075622f, 0.063222f,0.050547f,0.219582f}, // this matrix is not good
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90024}" },

		{	L"Canon DIGITAL IXUS 50 / Canon PowerShot SD400", 
			L"IXUS 50 / PowerShot SD400",
			2672,1968, 12,8,44,0, 2592,1944,  MOSAIC_RGGB,
			lsDaylight, {0.685247f,-0.204939f,-0.106531f, -0.267616f,0.790509f,0.073359f, 0.032401f,0.015655f,0.314892f}, // this matrix is not good
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90025}" },

		{	L"Canon PowerShot A430", 
			L"PowerShot A430",
			2392,1752, 0,8,56,4, 2272,1704,  MOSAIC_GBRG,
			lsDaylight, {0.479627f,-0.156240f,-0.084926f, -0.215238f,0.534902f,0.060219f, -0.096906f,0.148194f,0.191583f},
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f90026}" },

		{	L"Canon DIGITAL IXUS 970 IS / Canon PowerShot SD890 IS", 
			L"IXUS 970 IS / PowerShot SD890 IS",
			3720,2772, 8,12,32,0, 3648,2736,  MOSAIC_RGGB,
			17, {0.808011f,-0.261330f,-0.122838f, -0.036763f,0.600489f,0.041035f, 0.003414f,0.067582f,0.211157f }, 
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_12,
			L"{11111111-11110000-08a420c3-32f90027}" },

		{	L"Canon PowerShot SX110 IS", 
			L"PowerShot SX110 IS",
			3720,2772, 8,14,32,0, 3456,2592,  MOSAIC_RGGB,
			17, {14134,-5576,-1527, -1991,10719,1273, -1158,1929,3581 }, // from DCRAW
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_12,
			L"{11111111-11110000-08a420c3-32f90028}" },

		{	L"Canon DIGITAL IXUS 980 IS / Canon PowerShot SD990 IS", 
			L"IXUS 980 IS / PowerShot SD990 IS",
			4480,3348, 12,10,36,12, 4416,3312,  MOSAIC_GBRG,
			17, {0.836835f,-0.316442f,-0.130516f, -0.111253f,0.594589f,0.049307f, 0.010813f,0.071258f,0.206445f }, 
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_12,
			L"{11111111-11110000-08a420c3-32f90029}" },

		{	L"Canon PowerShot A470", 
			L"PowerShot A470",
			3152,2346, 12,6,44,10, 3072,2304,  MOSAIC_RGGB, 
			17, {0.624777f,-0.212311f,-0.083575f, -0.124817f,0.493647f,0.072081f, 0.009428f,0.046533f,0.211484f}, // Not good
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_10,
			L"{11111111-11110000-08a420c3-32f9002A}" },

		{	L"Canon PowerShot SX10 IS", 
			L"PowerShot SX10 IS",
			3720,2772, 4,12,28,0, 3648,2736,  MOSAIC_RGGB, // areas not checked!
			17, {13124.0f,-5329.0f,-1390.0f, -3602.0f,11658.0f,1944.0f, -1612.0f,2863.0f,4885.0f },
			{1.0f, 1.0f, 1.0f},
			-1, {}, {}, ppc_12,
			L"{11111111-11110000-08a420c3-32f9002B}" },

		{	L"Canon DIGITAL IXUS 870 IS / Canon PowerShot SD880 IS", 
			L"IXUS 870 IS / PowerShot SD880 IS",
			3720,2772, 4,12,24,0, 3648,2736,  
			MOSAIC_RGGB,
			17, {0.759444f,-0.270106f,-0.111462f, -0.005098f,0.519067f,0.041750f, 0.023861f,0.044290f,0.212756f }, {1.0f, 1.0f, 1.0f},
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f9002C}" },

		{	L"Canon PowerShot A1000 IS", 
			L"PowerShot A1000 IS",
			3720,2772, 14,8,3682,2764, 3648,2736,
			MOSAIC_RGGB,
			17, {827547,-290458,-126086, -12829,530507,50537, 5181,48183,245014}, {1.0f, 1.0f, 1.0f}, // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f9002D}" },

		{	L"Canon PowerShot A1100 IS",
			L"PowerShot A1100 IS",
			4104,3048, 20,24,4020,3024, 4000,3000,
			MOSAIC_GBRG,
			17, {793137,-271722,-113158, -55013,541027,32394, 21921,71395,181847}, {1.0f, 1.0f, 1.0f}, // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f9002E}" },

		{	L"Canon PowerShot A1200", 
			L"PowerShot A1200",
			4104,3048, 0,10,4072,3040, 4000,3000,
			MOSAIC_GBRG,
			17, {827547,-290458,-126086, -12829,530507,50537, 5181,48183,245014}, {1.0f, 1.0f, 1.0f},  // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f9002F}" },

		{	L"Canon PowerShot A1300", 
			L"PowerShot A1300",
			4704,3504, 16,26,4640,3486, 4608,3456,
			MOSAIC_GBRG,
			17, {827547,-290458,-126086, -12829,530507,50537, 5181,48183,245014}, {1.0f, 1.0f, 1.0f}, // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f90030}" },

		{	L"Canon PowerShot A2000 IS", 
			L"PowerShot A2000 IS",
			3720,2772, 14,8,3682,2764, 3648,2736,
			MOSAIC_RGGB,
			17, {827547,-290458,-126086, -12829,530507,50537, 5181,48183,245014}, {1.0f, 1.0f, 1.0f}, // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f90031}" },

		{	L"Canon PowerShot A2100 IS", 
			L"PowerShot A2100 IS",
			4104,3048, 20,24,4020,3024, 4000,3000,
			MOSAIC_GBRG,
			17, {793137,-271722,-113158, -55013,541027,32394, 21921,71395,181847}, {1.0f, 1.0f, 1.0f}, // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f90032}" },

		{	L"Canon PowerShot A2200", 
			L"PowerShot A2200",
			4416,3296, 16,29,4352,3275, 4320,3240,
			MOSAIC_GBRG,
			17, {827547,-290458,-126086, -12829,530507,50537, 5181,48183,245014}, {1.0f, 1.0f, 1.0f},  // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f90033}" },

		{	L"Canon PowerShot A3000 IS", 
			L"PowerShot A3000 IS",
			3720,2772, 0,0,3700,2772, 3648,2736,
			MOSAIC_GBRG,
			17, {587459,-200837,-96560, -64810,513113,42843, 10036,78911,231962}, {1.0f, 1.0f, 1.0f},
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f90034}" },

		{	L"Canon PowerShot A3100 IS", 
			L"PowerShot A3100 IS",
			4080,3048, 20,12,4056,3038, 4000,3000,
			MOSAIC_RGGB,
			17, {827547,-290458,-126086, -12829,530507,50537, 5181,48183,245014}, {1.0f, 1.0f, 1.0f}, // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f90035}" },

		{	L"Canon PowerShot A3200 IS", 
			L"PowerShot A3200 IS",
			4416,3296, 96,56,4416,3296, 4320,3240,
			MOSAIC_RGGB,
			17, {827547,-290458,-126086, -12829,530507,50537, 5181,48183,245014}, {1.0f, 1.0f, 1.0f}, // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f90036}" },

		{	L"Canon PowerShot A3300 IS", 
			L"PowerShot A3300 IS",
			4704,3504, 16,26,4640,3486, 4608,3456,
			MOSAIC_RGGB,
			17, {827547,-290458,-126086, -12829,530507,50537, 5181,48183,245014}, {1.0f, 1.0f, 1.0f}, // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f90037}" },

		{	L"Canon PowerShot A3400 IS", 
			L"PowerShot A3400 IS",
			4704,3504, 16,26,4640,3486, 4608,3456,
			MOSAIC_GBRG,
			17, {827547,-290458,-126086, -12829,530507,50537, 5181,48183,245014}, {1.0f, 1.0f, 1.0f}, // ?????
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f90038}" },

		{	L"Canon PowerShot A410", 
			L"PowerShot A410",
			2144,1560, 2,6,2090,1558, 2048,1536,
			MOSAIC_RGGB,
			17, {270041,-110546,-43914, -121712,256251,12623, -27956,36119,104654}, {1.0f, 1.0f, 1.0f},
			-1, {}, {},
			ppc_10,
			L"{11111111-11110000-08a420c3-32f90039}" },

		{	L"Canon DIGITAL IXUS 105 IS / Canon PowerShot SD1300 IS / Canon DIGITAL IXY 200F", 
			L"IXUS 105 / PowerShot SD1300 IS / IXY 200F",
			4104,3048, 6,12,28,18, 4000,3000,
			MOSAIC_GBRG,
			17, {1.405200f,-0.522900f,-0.115600f, -0.132500f,0.942000f,0.225200f, -0.049800f,0.195700f,0.411600f}, {1.0f, 1.0f, 1.0f},
			-1, {}, {},
			ppc_12,
			L"{11111111-11110000-08a420c3-32f9003A}" },

	};

	void copy_defaults(const CameraDefValue &def, CameraData &item)
	{
		item.id                   = def.id;
		item.model_name           = def.model_name;
		item.short_name           = def.short_name;
		item.width                = def.width;
		item.height               = def.height;
		item.active_origin_x      = def.active_origin_x;
		item.active_origin_y      = def.active_origin_y;
		item.active_origin_right  = (def.active_origin_right < 1000) ? def.active_origin_right : (def.width-def.active_origin_right);
		item.active_origin_bottom = (def.active_origin_bottom < 1000) ? def.active_origin_bottom : (def.height-def.active_origin_bottom);
		item.cropped_width        = def.cropped_width;
		item.cropped_height       = def.cropped_height;
		item.black_level          = ((1<<(def.bits_per_unit-5))-1);
		item.white_level          = ((1<<def.bits_per_unit)-1);
		item.mosaic               = (MosaicType)def.mosaic;
		item.bits_per_unit        = def.bits_per_unit;

		item.matrix1.illum = def.illum1;
		item.matrix1.matrix.assign(def.color_matrix1, def.color_matrix1+9);
		item.matrix1.mult.assign(def.color_matrix1_mult, def.color_matrix1_mult+3);

		item.matrix2.illum = def.illum2;
		item.matrix2.matrix.assign(def.color_matrix2, def.color_matrix2+9);
		item.matrix2.mult.assign(def.color_matrix2_mult, def.color_matrix2_mult+3);
	}

	const CameraDefValue *find_default(const wxString &id)
	{
		for (auto &def : camera_def_values) if (id == def.id) return &def;
		return NULL;
	}

} // end 'namespace'

CameraOpts::CameraOpts()
{
	sort_by_name();
}

void CameraOpts::set_defaults(bool clear_all)
{
	if (clear_all) items_.clear();

	for (auto &def : camera_def_values)
	{
		if (find_by_id(def.id)) continue;

		CameraData item;
		copy_defaults(def, item);
		add(item);
	}

	sort_by_name();
}

void CameraOpts::add(const CameraData &item, wxString *new_id)
{
	items_.push_back(item);
	if (items_.back().id.IsEmpty())
		items_.back().id = wxString::Format(L"{%08x-%08x-%08x-%08x}", rand()^(rand()<<16), rand()^(rand()<<16), rand()^(rand()<<16), rand()^(rand()<<16));
	if (new_id != NULL) *new_id = items_.back().id;
}

void CameraOpts::erase(const wxString& id)
{
	auto it = std::find_if(items_.begin(), items_.end(), [&](const CameraData &item) { return item.id == id; });
	if (it == items_.end()) return;
	items_.erase(it);
}

size_t CameraOpts::get_size() const
{
	return items_.size();
}

CameraData& CameraOpts::at(size_t index)
{
	return items_.at(index);
}

const CameraData& CameraOpts::at(size_t index) const
{
	return items_.at(index);
}

const CameraData* CameraOpts::find_by_id(const wxString &id) const
{
	for (std::vector<CameraData>::const_iterator it = items_.begin(), end = items_.end(); it != end; it++)
		if (it->id == id) return &*it;
	return NULL;
}

const CameraData* CameraOpts::find_by_name(const wxString &name) const
{
	wxString name_lc = name.Lower();
	wxStringTokenizer tok;
	for (std::vector<CameraData>::const_iterator it = items_.begin(), end = items_.end(); it != end; it++)
	{
		tok.SetString(it->model_name.Lower(), L"/");
		while ( tok.HasMoreTokens() )
		{
			wxString n = tok.GetNextToken().Trim(true).Trim(false);
			if (n == name_lc) return &*it;
		}
	}
	return NULL;
}

const CameraData* CameraOpts::find_by_file_size(unsigned int size) const
{
	Groups::const_iterator it = file_sizes_.find(size);
	if (it == file_sizes_.end()) return NULL;
	wxString id = it->second;
	for (std::vector<CameraData>::const_iterator it = items_.begin(), end = items_.end(); it != end; it++)
		if (it->id == id) return &*it;
	return NULL;
}

void CameraOpts::set_item(const wxString &id, const CameraData &item)
{
	for (std::vector<CameraData>::iterator it = items_.begin(), end = items_.end(); it != end; it++)
		if (it->id == id)
		{
			*it = item;
			it->id = id;
			break;
		}
}

void CameraOpts::reset_to_defaults(const wxString &id)
{
	const CameraDefValue *def = find_default(id);
	if (def == NULL) return;

	for (std::vector<CameraData>::iterator it = items_.begin(), end = items_.end(); it != end; it++)
		if (it->id == id)
		{
			copy_defaults(*def, *it);
			return;
		}
}

void CameraOpts::reset_to_defaults(const wxString &id, CameraData &item)
{
	const CameraDefValue *def = find_default(id);
	if (def == NULL) return;
	copy_defaults(*def, item);
}

void CameraOpts::sort_by_name()
{
	std::sort
	(
		items_.begin(),
		items_.end(),
		[] (const CameraData &item1, const CameraData &item2) {
			return item1.model_name < item2.model_name;
		}
	);
}

unsigned int CameraOpts::get_file_size(const CameraData &camera)
{
	return (camera.width*camera.height*(int)camera.bits_per_unit)/8;
}

void CameraOpts::enum_file_sizes(std::vector<size_t> &items)
{
	items.clear();
	for (const CameraData &item : items_) items.push_back(get_file_size(item));
	std::sort(items.begin(), items.end());
	items.erase(std::unique(items.begin(), items.end()), items.end());
}

wxString CameraOpts::vct_to_str(const std::vector<float> &vct, const wchar_t *format)
{
	wxString result;
	for (float value : vct) result += wxString::Format(format, value);
	return result.Trim();
}

void CameraOpts::save_matrix(wxConfigBase &config, const wxString &path, const CameraColorMatrix &matrix)
{
	config.Write(path+illum_opt_str, matrix.illum);
	config.Write(path+matrix_opt_str, vct_to_str(matrix.matrix, L"%0.6f "));
	config.Write(path+mult_opt_str, vct_to_str(matrix.mult, L"%0.1f "));
}

void CameraOpts::save()
{
	wxConfig config(app_name_opt_str);
	save(config, true);
}

void CameraOpts::save(wxConfigBase &config, bool save_groups)
{
	wxString path;

	config.Write(cam_count_opt_str, (int)items_.size());

	for (size_t i = 0; i < items_.size(); i++)
	{
		const CameraData &item = items_[i];
		path.Printf(wxT("Camera%03d/"), (int)i);

		config.Write(path+id_opt_str,                   item.id                       );
		config.Write(path+name_opt_str,                 item.model_name               );
		config.Write(path+short_name_opt_str,           item.short_name               );
		config.Write(path+width_opt_str,                (int)item.width               );
		config.Write(path+height_opt_str,               (int)item.height              );
		config.Write(path+cropped_width_opt_str,        (int)item.cropped_width       );
		config.Write(path+cropped_height_opt_str,       (int)item.cropped_height      );
		config.Write(path+active_origin_x_opt_str,      (int)item.active_origin_x     );
		config.Write(path+active_origin_y_opt_str,      (int)item.active_origin_y     );
		config.Write(path+active_origin_right_opt_str,  (int)item.active_origin_right );
		config.Write(path+active_origin_bottom_opt_str, (int)item.active_origin_bottom);
		config.Write(path+black_level_opt_str,          (int)item.black_level         );
		config.Write(path+white_level_opt_str,          (int)item.white_level         );
		config.Write(path+mosaic_opt_str,               (int)item.mosaic              );
		config.Write(path+bpu_str,                      (int)item.bits_per_unit    );
		save_matrix(config, path+wxT("Matrix1/"), item.matrix1);
		save_matrix(config, path+wxT("Matrix2/"), item.matrix2);
	}

	if (save_groups)
	{
		config.Write(gr_count_opt_str, (long)file_sizes_.size());
		int idx = 0;
		for (const Groups::value_type &item : file_sizes_)
		{
			path.Printf(L"Group%03d/", idx++);
			config.Write(path+file_size_opt_str, (long)item.first);
			config.Write(path+id_opt_str, item.second);
		}
	}
}

void CameraOpts::str_to_vect(std::vector<float> &vct, const wxString &txt, size_t count)
{
	wxString commas;

	double value = 0;
	wxString(L"0.5").ToDouble(&value);
	commas = (value > 0.1) ? L"." : L",";

	//wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT,wxLOCALE_CAT_NUMBER);

	wxString changed = txt;
	changed.Replace(L",", commas.c_str(), true);
	changed.Replace(L".", commas.c_str(), true);
	wxStringTokenizer tok(changed);
	std::vector<float> result;

	while (tok.HasMoreTokens())
	{
		wxString token = tok.GetNextToken();
		double value = 0;
		token.ToDouble(&value);
		result.push_back((float)value);
	}
	result.resize(count);
	vct.swap(result);
}

void CameraOpts::load_matrix(wxConfigBase &config, const wxString &path, CameraColorMatrix &matrix)
{
	matrix.illum = config.Read(path+illum_opt_str, -1);
	str_to_vect(matrix.matrix, config.Read(path+matrix_opt_str, wxString()), 9);
	str_to_vect(matrix.mult,   config.Read(path+mult_opt_str, wxString()),   3);
}

void CameraOpts::load()
{
	wxConfig config(app_name_opt_str);
	load(config);
}

void CameraOpts::load(wxConfigBase &config)
{
	CameraData item;
	wxString path;
	const unsigned int no_val = 0xFFFFFFFF;

	// read camera settings
	int count = config.Read(cam_count_opt_str, -1);
	if (count != -1)
	{
		items_.clear();
		for (int i = 0; i < count; i++)
		{
			path.Printf(L"Camera%03d/", i);

			item.id = config.Read(path+id_opt_str, wxString());
			if (item.id.IsEmpty()) continue;

			const CameraDefValue *def = find_default(item.id);
			if (def) copy_defaults(*def, item);
			else
			{
				item.model_name.Clear();
				item.short_name.Clear();

				item.width = item.height = item.cropped_width = item.cropped_height =
					item.active_origin_x = item.active_origin_y = item.active_origin_right =
					item.active_origin_bottom = item.black_level = item.white_level = no_val;

				item.mosaic = (MosaicType)no_val;
				item.bits_per_unit = ppc_10;
			}

			item.model_name = config.Read(path+name_opt_str, item.model_name);
			item.short_name = config.Read(path+short_name_opt_str, item.short_name);

			item.width = config.Read(path+width_opt_str, item.width);
			item.height = config.Read(path+height_opt_str, item.height);

			item.cropped_width = config.Read(path+cropped_width_opt_str, item.cropped_width);
			item.cropped_height = config.Read(path+cropped_height_opt_str, item.cropped_height);

			item.active_origin_x = config.Read(path+active_origin_x_opt_str, item.active_origin_x);
			item.active_origin_y = config.Read(path+active_origin_y_opt_str, item.active_origin_y);
			item.active_origin_right = config.Read(path+active_origin_right_opt_str, item.active_origin_right);
			item.active_origin_bottom = config.Read(path+active_origin_bottom_opt_str, item.active_origin_bottom);

			item.black_level = config.Read(path+black_level_opt_str, item.black_level);
			item.white_level = config.Read(path+white_level_opt_str, item.white_level);

			item.mosaic = (MosaicType)config.Read(path+mosaic_opt_str, (int)item.mosaic);
			item.bits_per_unit = (BitsPerUnit)config.Read(path+bpu_str, (int)item.bits_per_unit);

			if (item.id.IsEmpty() || item.model_name.IsEmpty() || (item.width == no_val) || (item.height == no_val) ||
				(item.cropped_width == no_val) || (item.cropped_height == no_val) ||
				(item.active_origin_x == no_val) || (item.active_origin_y == no_val) ||
				(item.active_origin_right == no_val) || (item.active_origin_bottom == no_val) ||
				(item.black_level == no_val) || (item.white_level == no_val) || (item.mosaic == (MosaicType)no_val)) continue;

			load_matrix(config, path+L"Matrix1/", item.matrix1);
			load_matrix(config, path+L"Matrix2/", item.matrix2);

			add(item);
		}
	}

	// read camera groups
	count = config.Read(gr_count_opt_str, -1);
	if (count != -1)
	{
		file_sizes_.clear();

		for (int i = 0; i < count; i++)
		{
			path.Printf(L"Group%03d/", i);
			unsigned int file_size = (unsigned int)config.Read(path+file_size_opt_str, (long)0);
			if (file_size == 0) continue;
			wxString id = config.Read(path+id_opt_str, wxString());
			if (id.IsEmpty()) continue;
			file_sizes_[file_size] = id;
		}
	}
}

void CameraOpts::clear_groups()
{
	file_sizes_.clear();
}

void CameraOpts::add_group(unsigned int size, const wxString &id)
{
	file_sizes_[size] = id;
}

wxString CameraOpts::get_group(unsigned int size) const
{
	Groups::const_iterator it = file_sizes_.find(size);
	return it == (file_sizes_.end()) ? wxString() : it->second;
}
