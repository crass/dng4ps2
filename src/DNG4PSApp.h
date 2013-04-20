#ifndef DNG4PSAPP_H
#define DNG4PSAPP_H

class DNG4PSApp : public wxApp
{
    bool commandLineMode;
    
    public:
        DNG4PSApp():wxApp(),commandLineMode(false) {};
        virtual bool OnInit();
        virtual int OnRun();
};

#endif // DNG4PSAPP_H
