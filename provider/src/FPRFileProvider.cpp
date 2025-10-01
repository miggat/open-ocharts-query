// #include <wx/wx.h>
// #include <wx/wfstream.h>
// #include <wx/filename.h>
// #include <wx/cmdline.h>
// #include <wx/mstream.h>
// #include <wx/string.h>
// #include <wx/dir.h>
// #include <wx/fileconf.h>
// #include <wx/stdpaths.h>


// class FPRFileProviderImpl : public FPRFileProvider{
//     const wxString server=wxT("oeserverd");
//     const wxString server2=wxT("oexserverd");
// private:
//     wxString GetExePath(wxString fn){
//         wxFileName fn_exe=wxFileName::FileName(GetOCPN_ExePath());
//         wxFileName daemon(fn_exe.GetPath(),fn);
//         return daemon.GetFullPath();
//     }
// public:
    
//     FPRFileProviderImpl(){            
//     }        
    
//     virtual Result createFile(bool forDongle) override{
//         LOG_INFO(wxT("creating fingerprint, forDongle=%s"),(forDongle?"true":"false"));
//         Result rt;
//         rt.status=true;
//         //code copied from the plugin
       
//         wxString fpr_file;
//         wxString fpr_dir = *GetpPrivateApplicationDataLocation(); //GetWritableDocumentsDir();
//         wxString cmd=GetExePath(server);
//         if (!wxFileExists(cmd)){
//             cmd=GetExePath(server2);
//                 if (!wxFileExists(cmd)){
//                 rt.hasError=true;
//                 rt.error=wxString::Format(wxT("server %s or %s not found"),server,server2);
//                 return rt;
//             }
//         }
//         if( fpr_dir.Last() != wxFileName::GetPathSeparator() )
//             fpr_dir += wxFileName::GetPathSeparator();
        
//         if(forDongle)
//             cmd.Append(_T(" -k "));                  // Make SGLock fingerprint
//         else
//             cmd.Append(_T(" -g "));                  // Make fingerprint
        
//         cmd.Append(_T("\""));
//         cmd.Append(fpr_dir);            
//         cmd.Append(_T("\""));       
//         LOG_INFO(_T("Create FPR command: ") + cmd);
//         // Set environment variable to find the required sglock dongle library
//         wxFileName libraryPath(GetOCPN_ExePath());
//         libraryPath.RemoveLastDir();
//         wxString libDir = libraryPath.GetPath( wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + _T("lib/opencpn");
//         wxSetEnv(_T("LD_LIBRARY_PATH"), libDir ); //"/usr/local/lib/opencpn");
//         wxArrayString ret_array;      
//         wxExecute(cmd, ret_array, ret_array );
        
//         bool berr = false;
//         for(unsigned int i=0 ; i < ret_array.GetCount() ; i++){
//             wxString line = ret_array[i];
//             wxLogMessage(line);
//             if(line.Upper().Find(_T("ERROR")) != wxNOT_FOUND){
//                 LOG_ERROR(wxT("getFPRFile: %s"),line);
//                 berr = true;
//                 rt.hasError=true;
//                 rt.error=line;
//                 return rt;
//             }
//             if(line.Upper().Find(_T("FPR")) != wxNOT_FOUND){
//                 fpr_file = line.AfterFirst(':');
//             }
            
//         }
//         if(fpr_file.IsEmpty()){                 // Probably dongle not present
//             rt.hasError=true;
//             if (forDongle){
//                 LOG_ERROR(wxT("getFPRFile: DONGLE not present"));
//                 rt.error=_T("DONGLE_NOT_PRESENT");
//             }
//             else{
//                 rt.error=wxT("no file created by daemon");
//             }
//             return rt;
//         }
//         wxFileName outFile=wxFileName::FileName(fpr_file);
//         rt.fileName=outFile.GetFullName();
//         return rt;
//     }

// };
