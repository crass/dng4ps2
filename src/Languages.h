#ifndef LANGUAGES_H_INCLUDED
#define LANGUAGES_H_INCLUDED


#define DEFAULT_LANG wxLANGUAGE_DEFAULT

#define LANGUAGES_SUBDIRECTORY "langs"


struct SupportedLanguage
{
    SupportedLanguage():iso_name(nullptr), langinfo(nullptr) {}
    SupportedLanguage(const char* _iso_name):iso_name(_iso_name)
    {
        langinfo = wxLocale::FindLanguageInfo(_iso_name);
        if ( langinfo == nullptr )
        {
            wxLogError(_("Locale \"%s\" is unknown."), _iso_name);
            wxLanguageInfo *linfo = new wxLanguageInfo();
            linfo->Language = wxLANGUAGE_UNKNOWN;
            linfo->Description = wxString(_iso_name) + wxString(wxT(" (Unsupported)"));
            linfo->CanonicalName = _iso_name;
            langinfo = linfo;
        }
    }

    bool is_unknown() { return langinfo->Language == wxLANGUAGE_UNKNOWN; }

    const char* iso_name;
    const wxLanguageInfo * langinfo;
};

class Languages : public std::vector<SupportedLanguage>
{
public:
    Languages(std::initializer_list<Languages::value_type> il):std::vector<Languages::value_type>(il.begin(), il.end()),
        default_lang(find_by_language_id(DEFAULT_LANG)) {}

    const Languages::value_type find_by_language_id (wxLanguage lang_id) const;

    const Languages::value_type find_by_language_iso_name (wxString lang_iso_name) const;

    const Languages::value_type default_lang;
};

extern const Languages langs;

#endif // LANGUAGES_H_INCLUDED
