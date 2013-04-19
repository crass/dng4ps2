/*=============================================================================
  DNG4PS - software for converting Canon PS RAW files to DNG ones
  Copyright (C) 2007-2008 Denis Artyomov

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
  File:      Utils.cpp
  Author(s): Denis Artyomov (denis.artyomov@gmail.com)
  ===========================================================================*/

#include "pch.h"

// DNG SDK
#include "dng_color_space.h"
#include "dng_exceptions.h"
#include "dng_file_stream.h"
#include "dng_globals.h"
#include "dng_host.h"
#include "dng_ifd.h"
#include "dng_image_writer.h"
#include "dng_info.h"
#include "dng_linearization_info.h"
#include "dng_mosaic_info.h"
#include "dng_negative.h"
#include "dng_render.h"
#include "dng_simple_image.h"
#include "dng_tag_codes.h"
#include "dng_tag_types.h"
#include "dng_tag_values.h"
#include "dng_xmp_sdk.h"
#include "dng_camera_profile.h"
#include "dng_memory_stream.h"
#include "dng_xmp.h"
#include "dng_read_image.h"
#include "dng_camera_profile.h"

// Exiflib
#include "exif-data.h"
#include "exif-utils.h"
#include "exif-loader.h"
#include "exif-tag.h"
#include "exif-utils.h"
#include "exif-mnote-data.h"

#include "Exception.hpp"
#include "ProgramObjects.h"
#include "Utils.h"
#include "Options.h"
#include "gui/ProcessDialog.h"
#include "CameraOpts.h"

const int medium_preview_size = 1280;

extern wxString exe_dir, path_sep;
extern ProcessDialog * process_dialog;

struct ExifDataAgr
{
	ExifData *ed;
	ExifMnoteData *n;

	ExifDataAgr() : ed(NULL), n(NULL) {}

	~ExifDataAgr()
	{
		if (ed) exif_data_unref(ed);
	}
};

inline int FC(unsigned int row, unsigned int col, unsigned int pattern)
{
	return (pattern >> ((((row) << 1 & 14) + ((col) & 1)) << 1) & 3);
}

// Utils::get_exif_entry
ExifEntry* Utils::get_exif_entry(ExifData* ed, unsigned int tag)
{
	for (unsigned int i = 0; i < EXIF_IFD_COUNT; i++)
	{
		ExifEntry *entry = exif_content_get_entry(ed->ifd[i], (ExifTag)tag);
		if (entry) return entry;
	}
	return NULL;
}

// Utils::copy_exif
void Utils::copy_exif(ExifData* ed, unsigned int tag, dng_string & str, bool force)
{
	if (!str.IsEmpty() && !force) return;
	ExifEntry * entry = get_exif_entry(ed, tag);
	if (!entry) return;
	char buffer[1024];
	memset(buffer, 0, 1024);
	exif_entry_get_value(entry, buffer, 1024);
	str.Set(buffer);
}

// Utils::get_str_exif
wxString Utils::get_str_exif(ExifData* ed, unsigned int tag)
{
	ExifEntry * entry = get_exif_entry(ed, tag);
	if (!entry) return wxString();
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	exif_entry_get_value(entry, buffer, 1024);
	return wxString(buffer, wxConvLocal);
}

// Utils::get_short_exif
int Utils::get_short_exif(ExifData* ed, unsigned int tag)
{
	ExifEntry * entry = get_exif_entry(ed, tag);
	if (!entry) return 0;
	return exif_get_short(entry->data, EXIF_BYTE_ORDER_INTEL);
}

// Utils::copy_exif
bool Utils::copy_exif(ExifData* ed, unsigned int tag, dng_date_time & value)
{
	ExifEntry * entry = get_exif_entry(ed, tag);
	if (!entry) return false;
	char buffer[1024];
	memset(buffer, 0, 1024);
	exif_entry_get_value(entry, buffer, 1024);

	wxString date_time_string(buffer, wxConvLibc);

	wxStringTokenizer date_time_string2(date_time_string, L" ");

	wxStringTokenizer date_items(date_time_string2.GetNextToken(), L":");
	wxStringTokenizer time_items(date_time_string2.GetNextToken(), L":");

	long year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;

	date_items.GetNextToken().ToLong(&year);
	date_items.GetNextToken().ToLong(&month);
	date_items.GetNextToken().ToLong(&day);
	time_items.GetNextToken().ToLong(&hour);
	time_items.GetNextToken().ToLong(&minute);
	time_items.GetNextToken().ToLong(&second);

	value.fYear = year;
	value.fMonth = month;
	value.fDay = day;
	value.fHour = hour;
	value.fMinute = minute;
	value.fSecond = second;

	return true;
}

// Utils::copy_exif
void Utils::copy_exif(ExifData* ed, unsigned int tag, dng_urational & value)
{
	ExifEntry * entry = get_exif_entry(ed, tag);
	if (!entry) return;
	ExifRational val = exif_get_rational(entry->data, EXIF_BYTE_ORDER_INTEL);
	value = dng_urational(val.numerator, val.denominator);
}

// Utils::copy_exif
void Utils::copy_exif(ExifData* ed, unsigned int tag, dng_srational & value)
{
	ExifEntry * entry = get_exif_entry(ed, tag);
	if (!entry) return;
	ExifSRational val = exif_get_srational(entry->data, EXIF_BYTE_ORDER_INTEL);
	value = dng_srational(val.numerator, val.denominator);
}

// Utils::get_mnote_value
char* Utils::get_mnote_value(ExifMnoteData * n, const char * name, char * buffer, int buf_len)
{
	if (n == NULL) return NULL;

	unsigned int c = exif_mnote_data_count(n);
	for (unsigned int i = 0; i < c; i++)
	{
		const char * item_name = exif_mnote_data_get_name(n, i);
		if (item_name == NULL) continue;
		if (strcmp(name, item_name) == 0)
			return exif_mnote_data_get_value(n, i, buffer, 256);
	}
	return NULL;
}

// Utils::get_mnote_short
int Utils::get_mnote_short(ExifMnoteData * n, const char * name, const CorrMNoteData *values, int def_val)
{
	char buffer[256];
	const char * value = get_mnote_value(n, name, buffer, sizeof(buffer));
	if (value == NULL) return -1;
	if (values == NULL) return atoi(value);

	for (int i = 0; values[i].text; i++)
		if (strcmp(values[i].text, value) == 0) return values[i].value;

	return def_val;
}

// Utils::copy_mnote (dng_string)
void Utils::copy_mnote(ExifMnoteData * n, const char * name, dng_string & str, bool force)
{
	char buffer[1024];
	const char * value = get_mnote_value(n, name, buffer, sizeof(buffer));
	if (value == NULL) return;
	if (!force && !str.IsEmpty()) return;
	str.Set_SystemEncoding(value);
}

// Utils::copy_mnote (dng_urational)
void Utils::copy_mnote(ExifMnoteData *n, const char *name, dng_urational &data, float coef)
{
	char buffer[256];
	const char * value = get_mnote_value(n, name, buffer, sizeof(buffer));
	if (value == NULL) return;
	float fvalue = (float)atof(value)*coef;
	data.n = (unsigned int)(fvalue*1000+0.5);
	data.d = 1000;
}

// Utils::copy_mnote (dng_srational)
void Utils::copy_mnote(ExifMnoteData *n, const char *name, dng_srational &data, float coef)
{
	char buffer[256];
	const char * value = get_mnote_value(n, name, buffer, sizeof(buffer));
	if (value == NULL) return;
	float fvalue = (float)atof(value)*coef;
	data.n = (unsigned int)(fvalue*1000+(fvalue < 0 ? -0.5 : 0.5));
	data.d = 1000;
}

// Utils::copy_mnote (unsigned int)
void Utils::copy_mnote(ExifMnoteData *n, const char *name, uint32 &data)
{
	char buffer[1024];
	const char * value = get_mnote_value(n, name, buffer, sizeof(buffer));
	if (value == NULL) return;
	data = atoi(value);
}

const CorrMNoteData exposure_programs_str[] =
{
	{ "Program",     epProgramNormal    },
	{ "Tv-priority", epShutterPriority  },
	{ "Av-priority", epAperturePriority },
	{ "Manual",      epManual           },
	{ NULL }
};

const CorrMNoteData metering_modes_str[] =
{
	{ "Default",                 mmUnidentified          },
	{ "Evaluative",              mmMultiSpot             },
	{ "Unknown",                 mmUnidentified          },
	{ "Average",                 mmAverage               },
	{ "avg",                     mmAverage               },
	{ "Center-weighted average", mmCenterWeightedAverage },
	{ "Center-weight",           mmCenterWeightedAverage },
	{ "Spot",                    mmSpot                  },
	{ "Multi Spot",              mmMultiSpot             },
	{ "Pattern",                 mmPattern               },
	{ "Partial",                 mmPartial               },
	{ "Other",                   mmOther                 },
	{ NULL }
};

const CorrMNoteData light_source_str[] =
{
	{"Sunny", lsDaylight },
	{"Cloudy", lsFluorescent },
	{"Tungsten", lsTungsten},
	{"Fluorescent", lsFluorescent },
	{"Flash", lsFlash},
	{"Shade", lsShade },
	{"Daylight fluorescent",lsDaylightFluorescent},

	{ "Unknown",                     lsUnknown },
	{ "Daylight",                    lsDaylight },
	{ "Fluorescent",                 lsFluorescent },
	{ "Tungsten incandescent light", lsTungsten },
	{ "Tungsten",                    lsTungsten },
	{ "Flash",                       lsFlash },
	{ "Fine weather",                lsFineWeather },
	{ "Cloudy weather",              lsCloudyWeather },
	{ "cloudy",                      lsCloudyWeather },
	{ "Shade",                       lsShade },
	{ "Daylight fluorescent",        lsDaylightFluorescent },
	{ "Day white fluorescent",       lsDayWhiteFluorescent },
	{ "Cool white fluorescent",      lsCoolWhiteFluorescent },
	{ "White fluorescent",           lsWhiteFluorescent },
	{ "Standard light A",            lsStandardLightA },
	{ "Standard light B",            lsStandardLightB },
	{ "Standard light C",            lsStandardLightC },
	{ "D55",                         lsD55 },
	{ "D65",                         lsD65 },
	{ "D75",                         lsD75 },
	{ "ISO studio tungsten",         lsISOStudioTungsten },
	{ "Other",                       lsOther },
	{ NULL }
};

// Utils::add_metadata_from_jpeg
void Utils::add_metadata_from_jpeg(dng_host & host, dng_negative* negative, const ExifDataAgr &jpeg)
{
	dng_exif * exif = negative->GetExif();

	exif->fLensName.Clear();

	exif->fISOSpeedRatings[1] = exif->fISOSpeedRatings[2] = 0;

	if (jpeg.ed)
	{

		// Orientation
		dng_orientation orient;
		orient.SetTIFF( get_short_exif(jpeg.ed, EXIF_TAG_ORIENTATION) );
		negative->SetBaseOrientation(orient);

		exif->fMeteringMode             = get_short_exif(jpeg.ed, EXIF_TAG_METERING_MODE);               // Metering Mode
		exif->fFlash                    = get_short_exif(jpeg.ed, EXIF_TAG_FLASH);                       // Flash
		exif->fFlashPixVersion          = get_short_exif(jpeg.ed, EXIF_TAG_FLASH_PIX_VERSION);           // FlashPixVersion
		exif->fSensingMethod            = get_short_exif(jpeg.ed, EXIF_TAG_SENSING_METHOD);              // Sensing Method
		exif->fExposureMode             = get_short_exif(jpeg.ed, EXIF_TAG_EXPOSURE_MODE);               // Exposure Mode
		exif->fWhiteBalance             = get_short_exif(jpeg.ed, EXIF_TAG_WHITE_BALANCE);               // White Balance

		exif->fFocalPlaneResolutionUnit = get_short_exif(jpeg.ed, EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT); // Focal Plane Resolution Unit
		exif->fSceneCaptureType         = get_short_exif(jpeg.ed, EXIF_TAG_SCENE_CAPTURE_TYPE);          // Scene Capture Type
		exif->fISOSpeedRatings[0]       = get_short_exif(jpeg.ed, EXIF_TAG_ISO_SPEED_RATINGS);           // ISO
		exif->fLightSource              = get_short_exif(jpeg.ed, EXIF_TAG_LIGHT_SOURCE);                // LightSource

		copy_exif(jpeg.ed, EXIF_TAG_DATE_TIME_DIGITIZED,      exif->fDateTimeDigitized);     // DateTime Digitized
		copy_exif(jpeg.ed, EXIF_TAG_DATE_TIME_ORIGINAL,       exif->fDateTimeOriginal);      // DateTime Original

		dng_date_time dt_shot;
		copy_exif(jpeg.ed, EXIF_TAG_DATE_TIME,                dt_shot);              // DateTime
		negative->UpdateDateTime(dt_shot, 0);

		copy_exif(jpeg.ed, EXIF_TAG_MODEL,                    exif->fModel, true);           // Model
		copy_exif(jpeg.ed, EXIF_TAG_MAKE,                     exif->fMake, true);            // Manufacturer

		copy_exif(jpeg.ed, EXIF_TAG_EXPOSURE_TIME,            exif->fExposureTime);          // Exposure time

		copy_exif(jpeg.ed, EXIF_TAG_FNUMBER,                  exif->fFNumber);               // FNumber
		copy_exif(jpeg.ed, EXIF_TAG_SHUTTER_SPEED_VALUE,      exif->fShutterSpeedValue);     // ShutterSpeedValue
		copy_exif(jpeg.ed, EXIF_TAG_APERTURE_VALUE,           exif->fApertureValue);         // Aperture

		copy_exif(jpeg.ed, EXIF_TAG_EXPOSURE_BIAS_VALUE,      exif->fExposureBiasValue);     // Exposure Bias
		copy_exif(jpeg.ed, EXIF_TAG_MAX_APERTURE_VALUE,       exif->fMaxApertureValue);      // MaxApertureValue
		copy_exif(jpeg.ed, EXIF_TAG_FOCAL_LENGTH,             exif->fFocalLength);           // Focal Length
		copy_exif(jpeg.ed, EXIF_TAG_FOCAL_PLANE_X_RESOLUTION, exif->fFocalPlaneXResolution); // Focal Plane x-Resolution
		copy_exif(jpeg.ed, EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION, exif->fFocalPlaneYResolution); // Focal Plane y-Resolution
		copy_exif(jpeg.ed, EXIF_TAG_DIGITAL_ZOOM_RATIO,       exif->fDigitalZoomRatio);      // Digital Zoom Ratio
		copy_exif(jpeg.ed, EXIF_TAG_ARTIST,                   exif->fArtist);                // Artist
	}

	if (jpeg.n)
	{
		int focal_units = get_mnote_short(jpeg.n, "Focal units per mm", NULL, 1000);

		if (exif->fISOSpeedRatings[0] == 0) exif->fISOSpeedRatings[0] = get_mnote_short(jpeg.n, "ISO", 0); // ISO
		if (exif->fISOSpeedRatings[0] == 0)
		{
			double base_iso = get_mnote_short(jpeg.n, "Shot ISO", NULL, 0);
			double auto_iso = get_mnote_short(jpeg.n, "Auto ISO", NULL, 0);
			if ((base_iso > 0) && (auto_iso > 0))
				exif->fISOSpeedRatings[0] = (int)(auto_iso*base_iso/100.0+0.5);
		}

		exif->fExposureProgram = get_mnote_short(jpeg.n, "Exposure mode", exposure_programs_str); // Exposure Program
		exif->fMeteringMode    = get_mnote_short(jpeg.n, "Metering mode", metering_modes_str); // Metering Mode
		if (!exif->fLightSource)
			exif->fLightSource = get_mnote_short(jpeg.n, "White balance", light_source_str, lsUnknown); // LightSource
		copy_mnote(jpeg.n, "OwnerName",                   exif->fArtist);
		copy_mnote(jpeg.n, "Long focal length of lens",   exif->fLensInfo[1], 1.0/focal_units);
		copy_mnote(jpeg.n, "Short focal length of lens",  exif->fLensInfo[0], 1.0/focal_units);
		copy_mnote(jpeg.n, "FirmwareVersion",             exif->fFirmware);

		copy_mnote(jpeg.n, "Focus distance upper", exif->fSubjectDistance, 1/100.0);

		//exif->fSubjectDistance

		/*
				ExifEntry * entry = get_exif_entry(ed, EXIF_TAG_MAKER_NOTE);
				if (entry)
				{
					int offset = 0;
					AutoPtr<dng_memory_block> block(host.Allocate(entry->size-offset));
					memcpy(block->Buffer(), entry->data+offset, entry->size-offset);
					negative->SetMakerNoteSafety(true);
					negative->SetMakerNote(block);

					wxFile file(L"c:\\mnote.bin", wxFile::write);
					file.Write(entry->data+offset, entry->size-offset);
				}

				unsigned char *buffer = NULL;
				unsigned int buf_len = 0;
				exif_mnote_data_set_offset(n, 0);
				exif_mnote_data_save(n, &buffer, &buf_len);
				AutoPtr<dng_memory_block> block(host.Allocate(buf_len));
				memcpy(block->Buffer(), buffer, buf_len);
				negative->SetMakerNoteSafety(true);
				negative->SetMakerNote(block);
				free(buffer);

				*/
	}
}

// Utils::create_dng_file
void Utils::create_dng_file
(
    const wxString& new_file,
    const wxDateTime& date_time,
    const CameraData* camera_data,
    bool jpeg_exists,
    const wxString &real_jpeg,
    const ExifDataAgr &jpeg,
    const std::vector<unsigned short> &raw_data,
    double matrix[3][3],
    wxImage *resultImage
)
{
	wxLogNull logNo;

	dng_memory_allocator memalloc(gDefaultDNGMemoryAllocator);
	dng_memory_stream stream(memalloc);
	stream.Put(&raw_data.front(), raw_data.size()*sizeof(unsigned short));

	dng_host host(&memalloc);

	host.SetKeepStage1(true);

	AutoPtr<dng_image> image(new dng_simple_image(dng_rect(camera_data->height, camera_data->width), 1, ttShort, 1<<10, memalloc));

	AutoPtr<dng_camera_profile> camprofile(new dng_camera_profile);
	camprofile->SetName(camera_data->model_name.char_str());

	dng_matrix m1(3, 3);
	dng_matrix m2(3, 3);

	if (matrix == NULL)
	{
		if (camera_data->matrix1.illum != -1)
		{
			for (int i = 0; i < 9; i++) m1[i/3][i%3] = (float)camera_data->matrix1.matrix[i];
			camprofile->SetColorMatrix1(m1);
			camprofile->SetCalibrationIlluminant1(camera_data->matrix1.illum);
		}

		if (camera_data->matrix2.illum != -1)
		{
			for (int i = 0; i < 9; i++) m2[i/3][i%3] = (float)camera_data->matrix2.matrix[i];
			camprofile->SetColorMatrix2(m2);
			camprofile->SetCalibrationIlluminant2(camera_data->matrix2.illum);
		}
	}
	else
	{
		for (int x = 0; x < 3; x++) for (int y = 0; y < 3; y++)
				m1[y][x] = matrix[x][y];
		camprofile->SetColorMatrix1(m1);
		camprofile->SetCalibrationIlluminant1(camera_data->matrix1.illum);
	}


	dng_ifd ifd;

	ifd.fUsesNewSubFileType = true;
	ifd.fNewSubFileType = 0;
	ifd.fImageWidth = camera_data->width;
	ifd.fImageLength = camera_data->height;
	ifd.fBitsPerSample[0] = 16;
	ifd.fBitsPerSample[1] = 0;
	ifd.fBitsPerSample[2] = 0;
	ifd.fBitsPerSample[3] = 0;
	ifd.fCompression = ccUncompressed;
	ifd.fPredictor = 1;
	ifd.fCFALayout = 1;
	ifd.fPhotometricInterpretation = piCFA;
	ifd.fFillOrder = 1;
	ifd.fOrientation = 0;
	ifd.fSamplesPerPixel = 1;
	ifd.fPlanarConfiguration = 1;
	ifd.fXResolution = 0.0;
	ifd.fYResolution = 0.0;
	ifd.fResolutionUnit = 0;

	ifd.fUsesStrips = true;
	ifd.fUsesTiles = false;

	ifd.fTileWidth = camera_data->width;
	ifd.fTileLength = camera_data->height;
	ifd.fTileOffsetsType = 4;
	ifd.fTileOffsetsCount = 0;

	ifd.fSubIFDsCount = 0;
	ifd.fSubIFDsOffset = 0;
	ifd.fExtraSamplesCount = 0;
	ifd.fSampleFormat[0] = 1;
	ifd.fSampleFormat[1] = 1;
	ifd.fSampleFormat[2] = 1;
	ifd.fSampleFormat[3] = 1;

	ifd.fLinearizationTableType = 0;
	ifd.fLinearizationTableCount = 0;
	ifd.fLinearizationTableOffset = 0;

	ifd.fBlackLevelRepeatRows = 1;
	ifd.fBlackLevelRepeatCols = 1;
	ifd.fBlackLevel[0][0][0] = camera_data->black_level;
	ifd.fBlackLevelDeltaHType = 0;
	ifd.fBlackLevelDeltaHCount = 0;
	ifd.fBlackLevelDeltaHOffset = 0;
	ifd.fBlackLevelDeltaVType = 0;
	ifd.fBlackLevelDeltaVCount = 0;
	ifd.fBlackLevelDeltaVOffset = 0;
	ifd.fWhiteLevel[0] = camera_data->white_level;
	ifd.fWhiteLevel[1] = 65535;
	ifd.fWhiteLevel[2] = 65535;
	ifd.fWhiteLevel[3] = 65535;

	ifd.fDefaultScaleH = dng_urational(1, 1);
	ifd.fDefaultScaleV = dng_urational(1, 1);
	ifd.fBestQualityScale = dng_urational(1, 1);

	ifd.fCFARepeatPatternRows = 0;
	ifd.fCFARepeatPatternCols = 0;

	ifd.fBayerGreenSplit = 0;
	ifd.fChromaBlurRadius = dng_urational(0, 0);
	ifd.fAntiAliasStrength = dng_urational(100, 100);

	ifd.fActiveArea = dng_rect
	                  (
	                      camera_data->active_origin_y,
	                      camera_data->active_origin_x,
	                      camera_data->height-camera_data->active_origin_bottom,
	                      camera_data->width-camera_data->active_origin_right
	                  );


	ifd.fDefaultCropOriginH = dng_urational((camera_data->width - camera_data->active_origin_right - camera_data->active_origin_x + 1 - camera_data->cropped_width)/2, 1);
	ifd.fDefaultCropOriginV = dng_urational((camera_data->height - camera_data->active_origin_bottom - camera_data->active_origin_y + 1 - camera_data->cropped_height)/2, 1);

	ifd.fDefaultCropSizeH = dng_urational(camera_data->cropped_width, 1);
	ifd.fDefaultCropSizeV = dng_urational(camera_data->cropped_height, 1);

	ifd.fMaskedAreaCount = 0;

	ifd.fLosslessJPEGBug16 = false;
	ifd.fSampleBitShift = 0;

	ifd.ReadImage(host, stream, *image.Get());

	AutoPtr<dng_negative> negative(host.Make_dng_negative());

	negative->SetDefaultScale(ifd.fDefaultScaleH, ifd.fDefaultScaleV);
	negative->SetDefaultCropOrigin(ifd.fDefaultCropOriginH, ifd.fDefaultCropOriginV);
	negative->SetDefaultCropSize(ifd.fDefaultCropSizeH, ifd.fDefaultCropSizeV);
	negative->SetActiveArea(ifd.fActiveArea);

	negative->SetModelName(camera_data->model_name.char_str());
	negative->SetLocalName(camera_data->model_name.char_str());

	negative->SetColorChannels(3);
	negative->SetColorKeys(colorKeyRed, colorKeyGreen, colorKeyBlue);
	negative->SetBayerMosaic(camera_data->mosaic);

	negative->SetWhiteLevel(camera_data->white_level);
	negative->SetBlackLevel(camera_data->black_level);

	negative->SetBaselineExposure(-0.5);
	negative->SetBaselineNoise(1.0);
	negative->SetBaselineSharpness(1.33);

	negative->SetAntiAliasStrength(dng_urational(100, 100));
	negative->SetLinearResponseLimit(1.0);
	negative->SetShadowScale( dng_urational(1, 1) );

	negative->SetAnalogBalance(dng_vector_3(1.0,1.0,1.0));

	negative->SetEmbeddedCameraProfile(camprofile);

	// Add EXIF data from JPEG
	if (sys().options->add_metadata && jpeg_exists) add_metadata_from_jpeg(host, negative.Get(), jpeg);
	else
	{
		negative->GetExif()->fModel.Set_ASCII(camera_data->model_name.char_str());
		negative->GetExif()->fMake.Set_ASCII("Canon");

		// Time from original shot
		dng_date_time dt;
		dt.fYear = date_time.GetYear();
		dt.fMonth = date_time.GetMonth()+1;
		dt.fDay = date_time.GetDay();
		dt.fHour = date_time.GetHour();
		dt.fMinute = date_time.GetMinute();
		dt.fSecond = date_time.GetSecond();

		negative->GetExif()->fDateTimeOriginal = dt;
		negative->GetExif()->fDateTimeDigitized = dt;
		negative->UpdateDateTime(dt, 0);
	}

	// Artist
	if (sys().options->use_artist) negative->GetExif()->fArtist.Set_SystemEncoding(sys().options->artist.char_str());

	// Clear "Camera WhiteXY"
	negative->SetCameraWhiteXY(dng_xy_coord());

	negative->SetStage1Image(image);
	negative->BuildStage2Image(host);
	negative->BuildStage3Image(host, -1);

	negative->SynchronizeMetadata();
	negative->RebuildIPTC();

	// Preview image
	AutoPtr<dng_jpeg_preview> jpeg_preview;
	if ((sys().options->preview_type == pt_Medium) || (sys().options->preview_type == pt_Full))
	{
		wxImage pre_image;
		jpeg_preview.Reset(new dng_jpeg_preview);

		if (!jpeg_exists || resultImage)
		{
			AutoPtr<dng_image> finalImage;
			dng_render tiff_render(host, *negative);
			tiff_render.SetFinalSpace(dng_space_sRGB::Get());
			tiff_render.SetFinalPixelType(ttByte);
			if (sys().options->preview_type == pt_Medium) tiff_render.SetMaximumSize(1280);
			finalImage.Reset(tiff_render.Render());

			dng_image_writer tiff_writer;
			dng_render pre_render(host, *negative);
			pre_render.SetFinalSpace(negative->IsMonochrome() ? dng_space_GrayGamma22::Get() : dng_space_sRGB::Get ());
			pre_render.SetFinalPixelType(ttByte);

			AutoPtr<dng_memory_stream> dms(new dng_memory_stream(gDefaultDNGMemoryAllocator));

			tiff_writer.WriteTIFF(host, *dms, *finalImage.Get(), finalImage->Planes() >= 3 ? piRGB : piBlackIsZero, ccUncompressed, negative.Get(), &pre_render.FinalSpace());

			std::vector<char> tif_mem_buffer(dms->Length());
			dms->SetReadPosition(0);
			dms->Get(&tif_mem_buffer.front(), tif_mem_buffer.size());
			dms.Reset();

			wxMemoryInputStream mis(&tif_mem_buffer.front(), tif_mem_buffer.size());

			try
			{
				pre_image.LoadFile(mis, wxBITMAP_TYPE_TIF);
				if (resultImage) *resultImage = pre_image;
			}
			catch (...) { }
		}
		else
		{
			pre_image.LoadFile(real_jpeg);
			if (sys().options->preview_type == pt_Medium)
			{
				int width = pre_image.GetWidth();
				int height = pre_image.GetHeight();
				if (!width && !height) return;
				if (width > height)
				{
					height = medium_preview_size*height/width;
					width = medium_preview_size;
				}
				else
				{
					width = medium_preview_size*width/height;
					height = medium_preview_size;
				}
				pre_image.Rescale(width, height, wxIMAGE_QUALITY_HIGH);
			}
		}

		wxMemoryOutputStream mos;

		pre_image.SetOption(L"quality", 90);
		pre_image.SaveFile(mos, wxBITMAP_TYPE_JPEG);

		size_t jpeg_preview_file_size = mos.GetLength();

		wxMemoryInputStream mis(mos);

		jpeg_preview->fPhotometricInterpretation = piYCbCr;
		jpeg_preview->fPreviewSize.v = pre_image.GetHeight();
		jpeg_preview->fPreviewSize.h = pre_image.GetWidth();
		jpeg_preview->fCompressedData.Reset( host.Allocate(jpeg_preview_file_size) );

		mis.Read(jpeg_preview->fCompressedData->Buffer_char(), jpeg_preview_file_size);
	}

	// Build thumbnail image.
	AutoPtr<dng_image> thumbnail;
	dng_render thumbnail_render(host, *negative);
	thumbnail_render.SetFinalSpace(dng_space_sRGB::Get());
	thumbnail_render.SetFinalPixelType(ttByte);
	thumbnail_render.SetMaximumSize(256);
	thumbnail.Reset(thumbnail_render.Render());

	// Writes DNG file data into memory stream (as dng_file_stream doesn't support of unicode in file names)
	dng_image_writer writer;
	dng_memory_stream filestream(gDefaultDNGMemoryAllocator);
	writer.WriteDNG(host, filestream, *negative.Get(), *thumbnail.Get(), sys().options->compress_dng ? ccJPEG : ccUncompressed, jpeg_preview.Get());

	// Writes memory stream into file
	wxFile dng_file(new_file.c_str(), wxFile::write);
	if (dng_file.Error()) throw Exception(L"Can't open file for writing");
	static const size_t BlockSize = 1024*64;
	size_t mem_stream_size = filestream.Length();
	std::vector<char> block_buffer(BlockSize);
	filestream.SetReadPosition(0);
	for (size_t p = 0; p < mem_stream_size; p += BlockSize)
	{
		size_t cur_clock_size = mem_stream_size-p;
		if (cur_clock_size > BlockSize) cur_clock_size = BlockSize;
		if (cur_clock_size == 0) break;
		filestream.Get(&block_buffer.front(), cur_clock_size);
		dng_file.Write(&block_buffer.front(), cur_clock_size);
	}
	dng_file.Close();
}


inline unsigned short swap_bytes_uint16(unsigned short src)
{
	unsigned char upper = ((src & 0xff00) >> 8);
	unsigned char lower = (src & 0x00ff);
	return ((lower << 8) | upper);
}

// Utils::load_raw_file
void Utils::load_raw_file(const wxString & raw_file_name, size_t file_size, std::vector<unsigned short> & data, int bits_per_unit)
{
	size_t size = 8*file_size/bits_per_unit;
	std::vector<unsigned char> raw_data;
	int vbits=0, buf=0;

	unsigned short mask = (1 << bits_per_unit)-1;

	raw_data.resize(file_size);

    wxFile file(raw_file_name, wxFile::read);
    if (!file.IsOpened()) throw Exception(wxString() << _("cant_open_file") << L" \"" << raw_file_name << L"\"");
    file.Read((char*)&raw_data.front(), raw_data.size());
    file.Close();

	data.resize(size);

	// Thanks cdraw for this code:
	const unsigned short* dp = (const unsigned short*)&raw_data.front();
	for (size_t i = 0; i < size; i++)
	{
#if qDNGBigEndian
		if (vbits < bits_per_unit) buf = (vbits += 16, (buf << 16) + swap_bytes_uint16(*dp++));
#else
		if (vbits < bits_per_unit) buf = (vbits += 16, (buf << 16) + *dp++);
#endif
		data[i] = buf >> (vbits -= bits_per_unit) & mask;
	}
}

// Utils::remove_bad_pixels (idea from dcraw)
void Utils::remove_bad_pixels(std::vector<unsigned short> & data, int width, int height, unsigned black_level, int mosaic_type)
{
	int row, col, tot, n, r, c;
	unsigned int pattern;

	switch (mosaic_type)
	{
	case MOSAIC_RGGB:
	default:
		pattern = 0x94949494;
		break;
	case MOSAIC_GBRG:
		pattern = 0x49494949;
		break;
	}

	for (row=0; row < height; row++)
		for (col=0; col < width; col++)
			if (pixel(data, col, row, width, height) < black_level)
			{
				tot = n = 0;
				for (r = row-2; r <= row+2; r++)
					for (c = col-2; c <= col+2; c++)
					{
						unsigned short pix = pixel(data, c, r, width, height);
						if ((r < height) && (c < width) && (FC(r,c,pattern) == FC(row,col,pattern)) && (pix >= black_level))
						{
							tot += pix;
							n++;
						}
					}
				if (n) pixel(data, col, row, width, height) = tot/n;
			}
}

// Utils::get_dng_path_and_name
void Utils::get_dng_path_and_name(const wxString& raw_file_name, wxString& path, wxString& name)
{
	wxFileName dng_fn = raw_file_name;

	dng_fn.SetPath(sys().options->output_path.IsEmpty() ? sys().options->path_to_files : sys().options->output_path);
	dng_fn.SetExt(L"DNG");

	if (sys().options->use_date_for_path)
	{
		wxFileName raw_fn = raw_file_name;
		wxDateTime date;
		bool ok = raw_fn.GetTimes(NULL, &date, NULL);
		if (ok)
		{
			const wchar_t *format = NULL;
			switch (sys().options->date_type)
			{
			case dt_YYYY_MM_DD:
				format = L"%Y-%m-%d";
				break;
			case dt_DD_MM_YYYY:
				format = L"%d-%m-%Y";
				break;
			case dt_YY_MM_DD:
				format = L"%y-%m-%d";
				break;
			case dt_DD_MM_YY:
				format = L"%d-%m-%y";
				break;
			}
			if (format) dng_fn.AppendDir(date.Format(format));
		}
	}

	path = dng_fn.GetPath();
	name = dng_fn.GetFullPath();
}

// Utils::process_file
void Utils::process_file
(
    const wxString & raw_file,
    const wxString & jpg_file_name,
    wxImage *resultImage,
    double matrix[3][3],
    const std::function<void(const wxString&)> &on_log
)
{
	wxString dng_file_name, dng_file_path;

	get_dng_path_and_name(raw_file, dng_file_path, dng_file_name);

	bool jpeg_exists;
	wxArrayString output, errors;
	std::vector<unsigned short> raw_data;
	wxFileName dng_fn = dng_file_name;

	if (resultImage == NULL)
	{
		jpeg_exists = wxFileExists(jpg_file_name) && (jpg_file_name.Lower() != raw_file.Lower());

		// Pass file if need
		if (sys().options->dont_overwrite && wxFileExists(dng_file_name)) return;

		// Create directory
		if (!wxFileName::DirExists(dng_file_path)) wxFileName::Mkdir(dng_file_path);

		// Delete existing file
		if ( wxFileExists(dng_file_name) ) wxRemoveFile(dng_file_name);

		// Start log
		if (on_log) on_log(raw_file);
	}
	else jpeg_exists = false;

	// Which camera type?
	unsigned long file_size = wxFileName::GetSize(raw_file).GetLo();
	ExifDataAgr jpeg_exif;
	const CameraData * camera = NULL;
	if (jpeg_exists)
	{
		ExifLoader *l = exif_loader_new();
		exif_loader_write_file(l, jpg_file_name.char_str());
		jpeg_exif.ed = exif_loader_get_data(l);
		jpeg_exif.n = exif_data_get_mnote_data(jpeg_exif.ed);
		exif_loader_unref(l);
		wxString camera_name = get_str_exif(jpeg_exif.ed, EXIF_TAG_MODEL);
		camera = sys().cameras->find_by_name(camera_name);
		if (camera != NULL)
		{
			unsigned int camera_file_size = CameraOpts::get_file_size(*camera);
			if (file_size != camera_file_size)
				throw Exception(wxString::Format(_("raw_file_size_not_correspond"), file_size, camera_name.c_str()));
		}
	}
	else
	{
		camera = sys().cameras->find_by_file_size(file_size);
	}

	if (camera == NULL) throw Exception(_("wrong_camera_type"));

	// Load RAW-file
	if (on_log) on_log(L" ("+wxString(camera->model_name)+L") ... ");
	load_raw_file(raw_file, file_size, raw_data, (int)camera->bits_per_unit);
	remove_bad_pixels(raw_data, camera->width, camera->height, camera->black_level, camera->mosaic);

	// Read original time
	wxDateTime dtAccess, dtMod, dtCreate, dtMin;
	wxFileName time_file = raw_file;
	bool times_is_ok = false;
	if (resultImage == NULL)
	{
		times_is_ok = time_file.GetTimes(&dtAccess, &dtMod, &dtCreate);
		dtMin = (dtAccess < dtMod) ? dtAccess : dtMod;
		if (dtCreate < dtMin) dtMin = dtCreate;
	}

	// Process with DNG
	create_dng_file(dng_file_name, dtMin, camera, jpeg_exists, jpg_file_name, jpeg_exif, raw_data, matrix, resultImage);

	if (resultImage == NULL)
	{
		// Set time of DNG file
		if (times_is_ok) dng_fn.SetTimes(&dtAccess, &dtMod, &dtCreate);

		// End log
		if (on_log) on_log(L"OK\n");
	}
}

bool Utils::get_jpeg_name(const wxFileName& file_name, wxFileName & result)
{
	wxString name = file_name.GetName();
	result.Clear();

	int pos = name.Find(L'_');
	if (pos != wxNOT_FOUND)
	{
		wxString number = name.SubString(pos+1, name.Len()-1);
		wxString path = file_name.GetPath();

		wxFileName new_name;
		result.SetPath(path);

		result.SetName(L"IMG_"+number+L".JPG");
        if (result.FileExists()) return true;

		result.SetName(L"img_"+number+L".JPG");
		if (result.FileExists()) return true;

		result.SetName(L"IMG_"+number+L".jpg");
		if (result.FileExists()) return true;

		result.SetName(L"img_"+number+L".jpg");
		if (result.FileExists()) return true;
    }

	result = file_name;
	result.SetExt(L"JPG");
	return (result.FileExists() && (file_name != result));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Utils::grb_to_xyz
void Utils::grb_to_xyz(double r, double g, double b, double &x, double &y, double &z)
{
	x = r * 0.4124 + g * 0.3576 + b * 0.1805;
	y = r * 0.2126 + g * 0.7152 + b * 0.0722;
	z = r * 0.0193 + g * 0.1192 + b * 0.9505;
}

// Utils::calc_errors
void Utils::calc_errors(const std::vector<Area> &areas, const double m[3][3], std::vector<double> &errors)
{
	double jX, jY, jZ, cX, cY, cZ, eX, eY, eZ, err;
	double min_divider = 0.00001;

	errors.clear();
	for(const Area &area : areas)
	{
		grb_to_xyz(area.jpeg.r, area.jpeg.g, area.jpeg.b, jX, jY, jZ);

		cX = area.camera.r*m[0][0] + area.camera.g*m[1][0] + area.camera.b*m[2][0];
		cY = area.camera.r*m[0][1] + area.camera.g*m[1][1] + area.camera.b*m[2][1];
		cZ = area.camera.r*m[0][2] + area.camera.g*m[1][2] + area.camera.b*m[2][2];

		err = 0;
		if (fabs(jX) > min_divider)
		{
			eX = fabs((cX-jX)/jX);
			if (eX > err) err = eX;
		}

		if (fabs(jY) > min_divider)
		{
			eY = fabs((cY-jY)/jY);
			if (eY > err) err = eY;
		}

		if (fabs(jZ) > min_divider)
		{
			eZ = fabs((cZ-jZ)/jZ);
			if (eZ > err) err = eZ;
		}

		errors.push_back(err);
	}
}

// Utils::find_white_area
int Utils::find_white_area(const std::vector<Area> &areas)
{
	int result = -1;
	double min_diff = 10000;
	for (size_t i = 0; i < areas.size(); i++)
	{
		const Area &area = areas[i];
		double average = (area.jpeg.r+area.jpeg.g+area.jpeg.b)/3.0;
		double diff_1 = fabs(area.jpeg.r-area.jpeg.g)/average;
		double diff_2 = fabs(area.jpeg.g-area.jpeg.b)/average;
		double diff_3 = fabs(area.jpeg.b-area.jpeg.r)/average;
		double diff = std::max(diff_1, std::max(diff_2, diff_3));
		if (diff < min_diff)
		{
			min_diff = diff;
			result = i;
		}
	}
	return result;
}

/* Utils::calc_camera_profile */
void Utils::calc_camera_profile
(
    double m[3][3],
    const std::vector<Area> &areas,
    int iterations,
    std::function<void (const wxString&)> on_log,
    double *result_error_max,
    double *result_aver_error
)
{
	double X, Y, Z;
	int white_area;
	wxString buffer;
	double diff_matrix[3][3];

	std::vector<double> errors;

	for (int x = 0; x < 3; x++) for (int y = 0; y < 3; y++) m[x][y] = (x==y) ? 1 : 0;

	white_area = find_white_area(areas);

	if (on_log)
	{
		buffer.Printf(L"White area: %i", white_area+1);
		on_log(buffer);
	}

	for (size_t i = 0; i < iterations; i++)
	{
		// Which zone is more wrong?
		calc_errors(areas, m, errors);
		size_t zone = std::max_element(errors.begin(), errors.end()) - errors.begin();

		double camera_r = areas[zone].camera.r;
		double camera_g = areas[zone].camera.g;
		double camera_b = areas[zone].camera.b;
		double new_m1, new_m2, new_m3;
		int col;

		for (int x = 0; x < 3; x++) for (int y = 0; y < 3; y++) diff_matrix[x][y] = m[x][y];

		grb_to_xyz(areas[zone].jpeg.r, areas[zone].jpeg.g, areas[zone].jpeg.b, X, Y, Z);
		static const double min_value = 0.1;

		bool calc_by_red = camera_r > min_value;
		bool calc_by_green = camera_g > min_value;
		bool calc_by_blue = camera_b > min_value;

		if (!calc_by_blue && !calc_by_green && !calc_by_red)
		{
			calc_by_red = calc_by_green = calc_by_blue = false;
			if ((camera_r > camera_g) && (camera_r > camera_b)) calc_by_red = true;
			else if ((camera_b > camera_g) && (camera_b > camera_r)) calc_by_blue = true;
			else calc_by_green = true;
		}

		if (calc_by_red)
		{
			col = 0;
			diff_matrix[col][0] = (X - m[1][0]*camera_g - m[2][0]*camera_b)/camera_r;
			diff_matrix[col][1] = (Y - m[1][1]*camera_g - m[2][1]*camera_b)/camera_r;
			diff_matrix[col][2] = (Z - m[1][2]*camera_g - m[2][2]*camera_b)/camera_r;
		}

		if (calc_by_green)
		{
			col = 1;
			diff_matrix[col][0] = (X - m[0][0]*camera_r - m[2][0]*camera_b)/camera_g;
			diff_matrix[col][1] = (Y - m[0][1]*camera_r - m[2][1]*camera_b)/camera_g;
			diff_matrix[col][2] = (Z - m[0][2]*camera_r - m[2][2]*camera_b)/camera_g;
		}

		if (calc_by_blue)
		{
			col = 2;
			diff_matrix[col][0] = (X - m[0][0]*camera_r - m[1][0]*camera_g)/camera_b;
			diff_matrix[col][1] = (Y - m[0][1]*camera_r - m[1][1]*camera_g)/camera_b;
			diff_matrix[col][2] = (Z - m[0][2]*camera_r - m[1][2]*camera_g)/camera_b;
		}

		int modify_coeff = (zone == white_area) ? areas.size()/10 : areas.size();
		if (modify_coeff < 2) modify_coeff = 2;

		for (int x = 0; x < 3; x++) for (int y = 0; y < 3; y++) m[x][y] =
				(diff_matrix[x][y]+(modify_coeff-1)*m[x][y])/modify_coeff;
	}


	calc_errors(areas, m, errors);

	double max_error = 0;
	double aver_error_total = 0;

	if (on_log) on_log(L"Errors:");
	int zone = 1;
	for (double err : errors)
	{
		aver_error_total += err;
		if (err > max_error) max_error = err;
		if (on_log)
		{
			buffer.Printf(L"Zone %i: %.1f%%", zone++, err*100.0);
			on_log(buffer);
		}
	}

	if (on_log)
	{
		static const double max_error_border = 0.1;
		if (max_error > max_error_border)
		{
			buffer.Printf(L"### Max error is greater then %.0f%%, RESULTS ARE POOR ###", max_error_border*100);
			on_log(buffer);
		}
	}

	if (result_error_max) *result_error_max = max_error;
	if (result_aver_error) *result_aver_error = aver_error_total/(double)errors.size();
}

/* Utils::inverse_matrix */
void Utils::inverse_matrix(const double m[3][3], double result[3][3])
{
	double A[3][3];

	double d = m[0][0]*m[1][1]*m[2][2] + m[1][0]*m[2][1]*m[0][2] + m[2][0]*m[0][1]*m[1][2]
	           - m[0][2]*m[1][1]*m[2][0] - m[0][1]*m[1][0]*m[2][2] - m[0][0]*m[1][2]*m[2][1];

	static const int cols1[] = {1, 0, 0};
	static const int cols2[] = {2, 2, 1};

	for (int x = 0; x < 3; x++) for (int y = 0; y < 3; y++)
		{
			double M = m[cols1[x]][cols1[y]]*m[cols2[x]][cols2[y]] - m[cols2[x]][cols1[y]] * m[cols1[x]][cols2[y]];
			A[x][y] = (((x+y)%2) == 0) ? M : -M;

		}

	for (int x = 0; x < 3; x++) for (int y = 0; y < 3; y++) result[x][y] = A[y][x]/d;
}

/* Utils::fill_3x3_matrix */
void Utils::fill_3x3_matrix
(
    double m[3][3],
    double m00, double m10, double m20,
    double m01, double m11, double m21,
    double m02, double m12, double m22
)
{
	m[0][0] = m00;
	m[1][0] = m10;
	m[2][0] = m20;
	m[0][1] = m01;
	m[1][1] = m11;
	m[2][1] = m21;
	m[0][2] = m02;
	m[1][2] = m12;
	m[2][2] = m22;
}

/* Utils::create_raw_image */
void Utils::create_raw_image(const CameraData* camera, const wxString& raw_file_name, const double m[3][3], wxImage &result, bool correct_gamma, double correction, double gamma)
{
	unsigned int r, g, b, offset_x, offset_y;
	double X, Y, Z, R, G, B;

	unsigned int active_area_width = (camera->width - camera->active_origin_right - camera->active_origin_x);
	unsigned int active_area_height = (camera->height - camera->active_origin_bottom - camera->active_origin_y);

	offset_x = camera->active_origin_x + (active_area_width - camera->cropped_width)/2;
	offset_y = camera->active_origin_y + (active_area_height - camera->cropped_height)/2;

	std::vector<unsigned short> raw_data;
	load_raw_file(raw_file_name, CameraOpts::get_file_size(*camera), raw_data, (int)camera->bits_per_unit);
	remove_bad_pixels(raw_data, camera->width, camera->height, camera->black_level, camera->mosaic);

	wxBitmap bitmap(camera->cropped_width, camera->cropped_height, 24);

	result = bitmap.ConvertToImage();

	for (unsigned int x = 0; x < camera->width; x += 2)
	{
		for (unsigned int y = 0; y < camera->height; y += 2)
		{
			r = g = b = 0;

			switch (camera->mosaic)
			{
			case 1:
				r += Utils::pixel(raw_data, x, y, camera->width, camera->height)-camera->black_level;
				g += (Utils::pixel(raw_data, x, y+1, camera->width, camera->height)-camera->black_level)/2;
				g += (Utils::pixel(raw_data, x+1, y, camera->width, camera->height)-camera->black_level)/2;
				b += Utils::pixel(raw_data, x+1, y+1, camera->width, camera->height)-camera->black_level;
				break;

			case 3:
				r += Utils::pixel(raw_data, x, y+1, camera->width, camera->height)-camera->black_level;
				g += (Utils::pixel(raw_data, x, y, camera->width, camera->height)-camera->black_level)/2;
				g += (Utils::pixel(raw_data, x+1, y+1, camera->width, camera->height)-camera->black_level)/2;
				b += Utils::pixel(raw_data, x+1, y, camera->width, camera->height)-camera->black_level;
				break;
			}

			R = (double)r/(camera->white_level-camera->black_level);
			G = (double)g/(camera->white_level-camera->black_level);
			B = (double)b/(camera->white_level-camera->black_level);


			if (m)
			{
				X = R*m[0][0] + G*m[1][0] + B*m[2][0];
				Y = R*m[0][1] + G*m[1][1] + B*m[2][1];
				Z = R*m[0][2] + G*m[1][2] + B*m[2][2];

				R = X *  3.2406 + Y * -1.5372 + Z * -0.4986;
				G = X * -0.9689 + Y *  1.8758 + Z *  0.0415;
				B = X *  0.0557 + Y * -0.2040 + Z *  1.0570;
			}

			if (correct_gamma)
			{
				if (R > 0.0031308) R = 1.055 * ( pow(R,( 1 / gamma )) ) - 0.055;
				else R = 12.92 * R;
				if (G > 0.0031308) G = 1.055 * ( pow(G,( 1 / gamma )) ) - 0.055;
				else G = 12.92 * G;
				if (B > 0.0031308) B = 1.055 * ( pow(B,( 1 / gamma )) ) - 0.055;
				else B = 12.92 * B;
			}

			R *= correction;
			G *= correction;
			B *= correction;

			if (R < 0) R = 0;
			if (R > 1) R = 1;
			if (G < 0) G = 0;
			if (G > 1) G = 1;
			if (B < 0) B = 0;
			if (B > 1) B = 1;

			r = R*255+0.5;
			g = G*255+0.5;
			b = B*255+0.5;

			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					int real_x = x-offset_x+i;
					int real_y = y-offset_y+j;
					if ((real_x >= 0) && (real_y >= 0) && (real_x < camera->cropped_width) && (real_y < camera->cropped_height))
						result.SetRGB(real_x, real_y, r, g, b);
				}
		}
	}
}

/* Utils::sRGB_to_RGB */
double Utils::sRGB_to_RGB(double color, double gamma)
{
	if (color > 0.04045) return pow(( ( color + 0.055 ) / 1.055 ), gamma);
	return color / 12.92;
}
