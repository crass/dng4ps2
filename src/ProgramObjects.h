#ifndef PROGRAM_OBJECTS_H_INCLUDED
#define PROGRAM_OBJECTS_H_INCLUDED

class Options;
class CameraOpts;

class ProgramObjects
{
public:
    std::unique_ptr<Options> options;
    std::unique_ptr<CameraOpts> cameras;
    wxString langs_path;

    void init_language();

private:
    friend ProgramObjects& _sys();

    ProgramObjects();

    wxLocale * locale_;
};

extern ProgramObjects& sys;

#endif // PROGRAM_OBJECTS_H_INCLUDED
