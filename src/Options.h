#ifndef DNG4PSOPTIONS_H
#define DNG4PSOPTIONS_H

enum PreviewType
{
    pt_None,
    pt_Medium,
    pt_Full
};

enum DateTypes
{
    dt_YYYY_MM_DD,
    dt_DD_MM_YYYY,
    dt_YY_MM_DD,
    dt_DD_MM_YY
};

class Options
{
public:
    wxString path_to_files;
    wxString output_path;
    wxString artist;
    bool use_artist;
    bool recursive_search;
    bool dont_overwrite;
    bool add_metadata;
    bool compress_dng;
    bool use_date_for_path;
    PreviewType preview_type;
    DateTypes date_type;
    wxString lang;

    wxString last_camera_id;

    Options();

    void load();
    void save() const;

private:
};

#endif

