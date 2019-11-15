/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

#if defined(NEUROMORE_PLATFORM_WINDOWS) || defined(NEUROMORE_PLATFORM_OSX)

#include "AutoUpdate.h"
#include <Core/LogManager.h>
#include "../QtBaseManager.h"
#include <QProcess>
#include <QDir>


#ifdef NEUROMORE_PLATFORM_WINDOWS
    #define MAINTENANCETOOL     "neuromoreMaintenanceTool.exe"
#elif NEUROMORE_PLATFORM_OSX
    #define MAINTENANCETOOL     "neuromoreMaintenanceTool.app"
#endif

using namespace Core;

//
bool AutoUpdate::IsUpdateAvailable(bool ignoreErrors)
{
	// get the maintenance tool path
	QDir installationDir(qApp->applicationDirPath());
	installationDir.cdUp();

    QProcess process;
    
#ifdef NEUROMORE_PLATFORM_WINDOWS
    QString maintenanceToolPath = installationDir.absolutePath() + QString("/") + MAINTENANCETOOL;
	QStringList args("--checkupdates");
	
    process.start( maintenanceToolPath, args );
#else
    installationDir.cdUp();
    installationDir.cdUp();
    installationDir.cdUp();
    
    QString maintenanceToolPath = installationDir.absolutePath() + QString("/") + MAINTENANCETOOL;
    
    //String cmd;
    //cmd.Format("open \"%s\"", maintenanceToolPath.toUtf8().data());
    //LogError("QProcess: cmd=%s", cmd.AsChar());

    QStringList args;
    args.append("--args");
    args.append("--checkupdates");

    process.start(maintenanceToolPath, args);
#endif

	// wait until the update tool is finished
	process.waitForFinished();

	if(process.error() != QProcess::UnknownError)
	{
		if (ignoreErrors == false)
        {
#ifdef NEUROMORE_PLATFORM_WINDOWS
			MessageBoxA( NULL, "Cannot find maintenance tool. Please contact the support team via support@neuromore.com", "Maintenance Tool Error",  MB_OK );
#else
            // TODO: show message box on macOS
#endif
        }
        
        QString output = process.readAllStandardOutput();
        QString error = process.readAllStandardError();
        LogError("AutoUpdate: Output=%s",  output.toUtf8().data());
        LogError("AutoUpdate: ErrorID=%i, ErrorMsg=%s", process.error(), error.toUtf8().data());

		return false;
	}

	// read the output
	QByteArray data = process.readAllStandardOutput();
	QString dataString(data);
    
    //LogInfo("AutoUpdate: %s",  dataString.toUtf8().data());

	// no output means no updates available
	// note that the exit code will also be 1, but we don't use that
	// also note that we should parse the output instead of just checking if it is empty if we want specific update info
	if (dataString.contains("<updates>") == false)
	{
		// IMPORTANT NOTE:
		// normally the return code is empty in case no updates are available
		// though, when running from C:/Program Files/neuromore/ where we don't have file write rights we get the following error when just checking for updates (while there is none)
		// "[0] Warning: Could not create lock file: 'C:/Program Files/neuromore/lockmyApp1502976.lock': Access is denied."
		// this works fine from another folder like out GitHub/Studio/Bin one, but doesn't work in Program Files
		// that is why we don't check for empty return string, except we check if the return string contains the <update> tag
		//if (data.isEmpty() == false)
		//	MessageBoxA( NULL, QString(data).toLatin1().data(), "No update", MB_OK | MB_ICONQUESTION|MB_TOPMOST);

		return false;
	}

	return true;
}


void AutoUpdate::StartUpdateTool()
{
	// get the maintenance tool path
	QDir installationDir(qApp->applicationDirPath());
	installationDir.cdUp();
    
#ifdef NEUROMORE_PLATFORM_WINDOWS
    QString maintenanceToolPath = installationDir.absolutePath() + QString("/") + MAINTENANCETOOL;

	// call the maintenance tool binary
	// note: we start it detached because this application need to close for the update
	QStringList args("--updater");
	/*bool success = */QProcess::startDetached( maintenanceToolPath, args );
#else
    installationDir.cdUp();
    installationDir.cdUp();
    installationDir.cdUp();
    
    QString maintenanceToolPath = installationDir.absolutePath() + QString("/") + MAINTENANCETOOL;
    LogError("AutoUpdate: maintenanceToolPath=%s\n",  maintenanceToolPath.toUtf8().data());
    
    String cmd;
    cmd.Format("open \"%s\" --args --updater", maintenanceToolPath.toUtf8().data());
    printf("cmd: %s", cmd.AsChar());
    
    system(cmd.AsChar());
#endif

	// close the application
	GetQtBaseManager()->ForceTerminateApplication();
}

#endif
