class pawrapper {
    public:
        pawrapper()
        {
            wxConfigBase::Get()->Read(wxT("Config/SystemNxDir"), &pactlcmd);
            pactlcmd << wxFileName::GetPathSeparator() << wxT("bin")
                << wxFileName::GetPathSeparator() << wxT("pactl.exe");
            m_bConnected=wxFileName::IsFileExecutable(pactlcmd);
            if (m_bConnected) {
                myLogTrace(MYTRACETAG, wxT("PAWrapper: executable \"%s\" found."),
                    pactlcmd.c_str());
                pactlcmd << wxT(" -s 127.0.0.1");
             } else
                myLogTrace(MYTRACETAG, wxT("PAWrapper: executable \"%s\" not found!"),
                    pactlcmd.c_str());
        }


        bool getdefaults(wxString &Sink, wxString &Source)
        {
            wxString cmd; wxArrayString outtxt; bool ret;
            Sink.Empty(); Source.Empty();
            cmd << pactlcmd << wxT(" info");
            int sterr = ::wxExecute(cmd, outtxt, wxEXEC_SYNC);
            ret = (sterr == 0);
            if (ret) {
                for (int i=0; i<outtxt.GetCount(); i++) {
                    wxArrayString tks = ::wxStringTokenize(outtxt[i],wxT(":"));
                    if (tks[0].IsSameAs(wxT("Default Sink")))
                        Sink = tks[1].Trim(false);
                    else if (tks[0].IsSameAs(wxT("Default Source")))
                        Source = tks[1].Trim(false);
                }
            } else
                myLogTrace(MYTRACETAG, wxT("PAWrapper: couldn't get defaults sterr = %d"),
                    sterr);
            return ret;
        }

        bool findmodules(const wxChar *name, wxArrayString &indexes, wxArrayString &args)
        {
            wxString cmd, ts; wxArrayString outtxt; bool ret;
            cmd << pactlcmd << wxT(" list short");
            int sterr = ::wxExecute(cmd, outtxt, wxEXEC_SYNC);
            ret = (sterr == 0);
            if (ret) {
                for (int i=0; i<outtxt.GetCount(); i++) {
                    wxArrayString tks = ::wxStringTokenize(outtxt[i]);
                    if (tks[1].IsSameAs(name)) {
                        indexes.Add(tks[0]);
                        ts.Empty();
                        for (int j=2; j<tks.GetCount(); j++) {
                            if (j!=2)
                                ts << wxT(" ");
                            ts << tks[j];
                        }
                        args.Add(ts);
                    }
                }
            } else
                myLogTrace(MYTRACETAG, wxT("PAWrapper: couldn't list modules sterr = %d"),
                    sterr);
            return ret;
        }


        bool loadmodule(const wxString name, const wxString args)
        {
            wxString cmd; wxArrayString outtxt; bool ret;
            cmd << pactlcmd << wxT(" load-module ")
                << name << wxT(" ") << args;
            int sterr = ::wxExecute(cmd, outtxt, wxEXEC_SYNC);
            ret = (sterr == 0);
            if (!ret)
                myLogTrace(MYTRACETAG, wxT("PAWrapper: couldn't load module %s sterr = %d"),
                    name.c_str(), sterr);
            return ret;
        }

        bool unloadmodule(int index)
        {
            wxString cmd; wxArrayString outtxt; bool ret;
            cmd << pactlcmd << wxT(" unload-module ")
                << wxString::Format(wxT("%u"),index);
            int sterr = ::wxExecute(cmd, outtxt, wxEXEC_SYNC);
            ret = (sterr == 0);
            if (!ret)
                myLogTrace(MYTRACETAG, wxT("PAWrapper: couldn't load module id %u; sterr = %d"),
                    index, sterr);
            return ret;
        }

        bool isConnected()
        {
            return m_bConnected;
        }

    private:

        bool m_bConnected;
        wxString pactlcmd;

};
