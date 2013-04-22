#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include "dng_date_time.h"

class dng_negative;
class dng_string;
class dng_urational;
class dng_srational;
class dng_host;
struct _ExifEntry; typedef _ExifEntry ExifEntry;
struct _ExifData; typedef _ExifData ExifData;
struct _ExifMnoteData; typedef _ExifMnoteData ExifMnoteData;
struct CameraData;
struct ExifDataAgr;

struct ColorHelper
{
	double r, g, b;
};

struct Area
{
	ColorHelper jpeg;
	ColorHelper camera;
};

struct CorrMNoteData
{
	const char *text;
	int value;
};

class Utils
{
private:
    static void load_raw_file(const wxString & raw_file_name, size_t file_size, std::vector<unsigned short> & data, int bits_per_unit);

    static unsigned short& pixel(std::vector<unsigned short> & data, int col, int row, int width, int height)
    {
        static unsigned short empty = 0;
        if ((col < 0) || (row < 0) || (col >= width) || (row >= height))
        {
            empty = 0;
            return empty;;
        }
#ifdef __WXDEBUG__
        return data.at(row*width+col);
#else
        return data[row*width+col];
#endif
    }

    static void get_dng_path_and_name(const wxString& raw_file_name, wxString& path, wxString& name);

    static void remove_bad_pixels(std::vector<unsigned short> & data, int width, int height, unsigned black_level, int mosaic_type);

    static void add_metadata_from_jpeg(dng_host & host, dng_negative* negative, const ExifDataAgr &jpeg);

    static ExifEntry* get_exif_entry(ExifData* ed, unsigned int tag);

	static wxString get_str_exif(ExifData* ed, unsigned int tag);
    static void copy_exif(ExifData* ed, unsigned int tag, dng_string & str, bool force = true);

    static void copy_exif(ExifData* ed, unsigned int tag, dng_urational & value);

    static void copy_exif(ExifData* ed, unsigned int tag, dng_srational & value);

    static int get_mnote_short(ExifMnoteData * n, const char * name, const CorrMNoteData *values = NULL, int def_val = 0);

    static char* get_mnote_value(ExifMnoteData *n, const char *name, char *buffer, int buf_len);

    static void copy_mnote(ExifMnoteData *n, const char *name, dng_string &str, bool force = true);

    static void copy_mnote(ExifMnoteData *n, const char *name, dng_urational &data, float coef = 1);

    static void copy_mnote(ExifMnoteData *n, const char *name, dng_srational &data, float coef = 1);

    static void copy_mnote(ExifMnoteData *n, const char *name, uint32 &data);

    static int get_short_exif(ExifData* ed, unsigned int tag);

    static bool copy_exif(ExifData* ed, unsigned int tag, dng_date_time & value);

    static bool copy_exif(ExifData* ed, unsigned int tag, dng_date_time_info & value);

    static void create_dng_file
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
    );

    static void grb_to_xyz(double r, double g, double b, double &x, double &y, double &z);

    static void calc_errors(const std::vector<Area> &areas, const double m[3][3], std::vector<double> &errors);
    static int find_white_area(const std::vector<Area> &areas);

public:
    static void process_file
    (
		const wxString & raw_file_name,
		const wxString & jpg_file_name,
		wxImage *resultImage,
		double matrix[3][3],
		const std::function<void(const wxString&)> &on_log
	);

	static void calc_camera_profile
	(
		double m[3][3],
		const std::vector<Area> &areas,
		int iterations,
		std::function<void (const wxString&)> on_log,
		double *result_max_error,
		double *result_aver_error
	);

	static void inverse_matrix(const double m[3][3], double result[3][3]);

	static void fill_3x3_matrix
	(
		double m[3][3],
		double m00, double m10, double m20,
		double m01, double m11, double m21,
		double m02, double m12, double m22
	);

	static bool get_jpeg_name(const wxFileName& name, wxFileName & result);

	static bool is_jpeg(const wxString& full_path);

	static void create_raw_image
	(
		const CameraData* camera,
		const wxString& raw_file_name,
		const double m[3][3],
		wxImage &result,
		bool correct_gamma,
		double correction,
		double gamma
	);

	static double sRGB_to_RGB(double color, double gamma);
};

#endif // UTILS_H_INCLUDED
