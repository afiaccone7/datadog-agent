#include "stdafx.h"

std::wstring datadog_path ;                             //IDS_DATADOG_PATH
std::wstring datadog_key_root_base;                     // IDS_DATADOG_KEY_ROOT_BASE
std::wstring datadog_acl_key_datadog_base;              // IDS_DATADOG_ACL_KEY_DATADOG_BASE

std::wstring datadog_service_name;                      //IDS_DATADOG_SERVICE_NAME

std::wstring ddAgentUserName;                           // IDS_DATADOG_AGENT_USER_NAME

std::wstring ddAgentUserDescription;                    // IDS_DATADOG_AGENT_USER_DESCRIPTION

std::wstring traceService;                              // IDS_TRACE_SERVICE_NAME
std::wstring processService;                            // IDS_PROCESS_SERVICE_NAME
std::wstring agentService;                              // IDS_AGENT_SERVICE_NAME

std::wstring propertyDDAgentUserName;                   // IDS_PROPERTY_DDAGENTUSER
std::wstring propertyDDAgentUserPassword;               // IDS_PROPERTY_DDAGENTUSER_PASSWORD
std::wstring propertyAppDataDir;                        // IDS_PROPERTY_PROGRAMDATA
std::wstring propertyProgramFilesDir;                   // IDS_PROPERTY_PROGRAMFILESDIR

//std::wstring propertyEnableServicesDeferredKey(L"enableservices");
//std::wstring propertyRollbackState(L"CustomActionData");
std::wstring logsSuffix;                        // IDS_LOGSSUFFIX
std::wstring authTokenSuffix;                   // IDS_AUTHTOKENSUFFIX
std::wstring datadogyaml;                       // IDS_DATADOGYAML
std::wstring confdsuffix;                       // IDS_CONFSDSUFFIX
std::wstring logsdirsuffix;                     // IDS_LOGSDIRSUFFIX


std::wstring programdataroot;
std::wstring logfilename;
std::wstring authtokenfilename;
std::wstring datadogyamlfile;
std::wstring confddir;
std::wstring logdir;

std::wstring propertyCustomActionData(L"CustomActionData");
std::wstring datadog_key_root;
std::wstring datadog_acl_key_datadog;

// installation steps
std::wstring strDdUserCreated(L"00-ddUserCreated");
std::wstring strDdUserPasswordChanged(L"01-ddUserPasswordChanged");
std::wstring strFilePermissionsChanged(L"02-ddUserFilePermsChanged");
std::wstring strAddDdUserToPerfmon(L"03-ddUserAddedToPerfmon");
std::wstring strChangedRegistryPermissions(L"04-ddRegPermsChanged");

std::wstring* loadStrings[] = {
    NULL,
    &datadog_path,
    &datadog_key_root_base,
    &datadog_acl_key_datadog_base,
    &datadog_service_name,
    &ddAgentUserName,
    &ddAgentUserDescription,
    &traceService,
    &processService,
    &agentService,
    &propertyDDAgentUserName,
    &propertyDDAgentUserPassword,
    &propertyAppDataDir,
    &propertyProgramFilesDir,
    &logsSuffix,
    &authTokenSuffix,
    &datadogyaml,
    &confdsuffix,
    &logsdirsuffix
};
void loadStringToWstring(int id, std::wstring *target)
{
#define DEFAULT_BUFFER_SIZE 512
    wchar_t defaultbuffer[DEFAULT_BUFFER_SIZE];
    memset(defaultbuffer, 0, DEFAULT_BUFFER_SIZE * sizeof(wchar_t));
    int nRc = LoadStringW(hDllModule, id, defaultbuffer, DEFAULT_BUFFER_SIZE);

    if (nRc == 0) {
        // string isn't present
        return;
    }
    if (nRc < DEFAULT_BUFFER_SIZE - 1) {
        // it fit in the buffer, just return it
        *target = defaultbuffer;
        return;
    }
    // ideally, we'll never get here.  The LoadString API is lame, and doesn't
    // tell you how big a buffer you need.  So, keep trying until we don't use
    // the whole buffer

    nRc = DEFAULT_BUFFER_SIZE * 2; // initialize to get past the initial comparison in the for
    for (int bufsz = DEFAULT_BUFFER_SIZE * 2; nRc >= (bufsz - 1); bufsz += DEFAULT_BUFFER_SIZE)
    {
        wchar_t * tgtbuffer = new wchar_t[bufsz];
        memset(tgtbuffer, 0, bufsz * sizeof(wchar_t));
        nRc = LoadStringW(hDllModule, id, tgtbuffer, bufsz);
        if (nRc < bufsz - 1) {
            *target = tgtbuffer;
        }
        delete[] tgtbuffer;
    }
}
void initializeStringsFromStringTable()
{
    for (int i = 1; i < sizeof(loadStrings) / sizeof(std::wstring*); i++)
    {
        loadStringToWstring(STRINGTABLE_BASE + i, loadStrings[i]);
    }
}

void buildStrings(CustomActionData& data)
{
    // build up all the path-based strings
    std::wstring programfiles;

    data.value(propertyAppDataDir, programdataroot);
    data.value(propertyProgramFilesDir, programfiles);

    logfilename = programdataroot + logsSuffix;
    authtokenfilename = programdataroot + authTokenSuffix;
    datadogyamlfile = programdataroot + datadogyaml;
    confddir = programdataroot + confdsuffix;
    logdir = programdataroot + logsdirsuffix;

    datadog_key_root = datadog_key_root_base + datadog_path;
    datadog_acl_key_datadog = datadog_acl_key_datadog_base + datadog_path;


}
void toMbcs(std::string& target, LPCWSTR src) {
    size_t len = wcslen(src);
    size_t narrowlen = (2 * len) + 1;
    char * tgt = new char[narrowlen];
    wcstombs(tgt, src, narrowlen);
    target = tgt;
    delete[] tgt;
    return;
}

bool loadPropertyString(MSIHANDLE hInstall, LPCWSTR propertyName, std::wstring& dststr)
{
    wchar_t *dst = NULL;
    DWORD len = 0;
    if(loadPropertyString(hInstall, propertyName, &dst, &len)) {
        dststr = dst;
        delete [] dst;
        return true;
    }
    return false;
}

bool loadPropertyString(MSIHANDLE hInstall, LPCWSTR propertyName, wchar_t **dst, DWORD *len)
{
    TCHAR* szValueBuf = NULL;
    DWORD cchValueBuf = 0;
    std::string propertyname;
    std::string propval;
    toMbcs(propertyname, propertyName);

    UINT uiStat =  MsiGetProperty(hInstall, propertyName, L"", &cchValueBuf);
    //cchValueBuf now contains the size of the property's string, without null termination
    if (ERROR_MORE_DATA == uiStat)
    {
        ++cchValueBuf; // add 1 for null termination
        szValueBuf = new wchar_t[cchValueBuf];
        if (szValueBuf)
        {
            uiStat = MsiGetProperty(hInstall, propertyName, szValueBuf, &cchValueBuf);
        }
    }
    if (ERROR_SUCCESS != uiStat)
    {
        if (szValueBuf != NULL) 
           delete[] szValueBuf;
        WcaLog(LOGMSG_STANDARD, "failed to get  property");
        return false;
    }
    if (wcslen(szValueBuf) == 0){
        WcaLog(LOGMSG_STANDARD, "Property %s is empty", propertyname.c_str());
        delete [] szValueBuf;
        return false;
    }
    *dst=szValueBuf;
    *len = cchValueBuf;
    toMbcs(propval, szValueBuf);
    WcaLog(LOGMSG_STANDARD, "loaded property %s = %s", propertyname.c_str(), propval.c_str());

    
    return true;
}


bool loadDdAgentPassword(MSIHANDLE hInstall, wchar_t **pass, DWORD *len) {
    return loadPropertyString(hInstall, propertyDDAgentUserPassword.c_str(), pass, len);
}