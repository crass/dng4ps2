#ifndef PROGRAM_OBJECTS_H_INCLUDED
#define PROGRAM_OBJECTS_H_INCLUDED

class Options;
class CameraOpts;

class ProgramObjects
{
public:
    std::auto_ptr<Options> options;
    std::auto_ptr<CameraOpts> cameras;
    wxString langs_path;

    void init_language();

private:
    friend ProgramObjects& sys();

    ProgramObjects();

    wxLocale * locale_;
};

ProgramObjects& sys();

#endif // PROGRAM_OBJECTS_H_INCLUDED
