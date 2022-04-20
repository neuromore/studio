#include "TourManager.h"
#include "Plugins/BackendFileSystem/BackendFileSystemPlugin.h"
#include "MainWindow.h"


OnboardingAction* TourManager::CurrentOnboardingAction = nullptr;

TourManager::TourManager()
{
};

TourManager::~TourManager()
{
	foreach(auto action, mOnboardingActions) 
	{
		action->deleteLater();
	}
}

bool TourManager::InitOnboardingActions()
{
	auto appManager = GetManager();
	auto mainWindow = GetMainWindow();

	int titleBarHeight = 0;
	#if defined(NEUROMORE_PLATFORM_OSX)
		QWidget * menuWidget = GetMainWindow()->menuWidget();
		if (nullptr != menuWidget) {
			titleBarHeight = menuWidget->height() + 20;
		}
	#endif

	OnboardingAction* welcomeAction = new OnboardingAction(mainWindow);
	welcomeAction->setTitle(QString("Welcome to neuromore Studio"));
	welcomeAction->setDescription(QString("neuromore Studio is your all-in-one platform to create any kind of bio-tech application, "
		"from state-of-the art neuro- and bio-feedback experiences to rich, interactive EEG analysis dashboards.<br/>"
		"Let us give you a quick tour to show you how to create a simple neurofeedback application in neuromore Studio."));
	welcomeAction->setTitlePosition(QRect(38, 35, 545, 47));
	welcomeAction->setDescriptionPosition(QRect(38, 120, 660, 121));
	welcomeAction->getWindowPosition = [](QDockWidget*, int titleBarHeight, OnboardingAction*)
	{ 
		return QRect(563 - titleBarHeight, 281 - 0, 729, 280);
	};

	mOnboardingActions.push_back(welcomeAction);

	OnboardingAction* fileHandlingAction = new OnboardingAction(mainWindow);
	fileHandlingAction->setTitle(QString("Handling files"));
	fileHandlingAction->setDescription((QString("Every neuromore experience consists of two files:<br/>"
			" 1. a <font color='#4EBCEB'>classifier</font> in which you define the signal processing pipeline and <br/>"
			" 2. a <font color='#4EBCEB'>state machine</font> for the application logic and the UI.<br/> For this intro we've already loaded the"
			" classifier and state machine of our <i>getting started</i> tutorial.<br/> All files in the <i> examples </i>-folder"
			" are read-only. To edit them you can copy them to your personal folder"
			" by right-clicking on them. ")));
	fileHandlingAction->setActivePlugin("Back-End File System");
	fileHandlingAction->setInstructionsTitle("Do this now");
	fileHandlingAction->setInstructionsDescription("<ul> <li> Copy the <i> GettingStartedClassifier </i> and the"
		" <i> GettingStartedStateMachine </i> from the <i> examples GetingStarted </i> folder to your"
		" personal folder </li> <li> Open both files by double clicking them </li> </ul>");
	auto backendFileSystemWidget = fileHandlingAction->getDockWidget();

	if (nullptr == backendFileSystemWidget) {
		return false;
	}

	fileHandlingAction->getWindowPosition = [](QDockWidget * backendFileSystemWidget, int titleBarHeight, OnboardingAction*)
	{ return  QRect(backendFileSystemWidget->x() + backendFileSystemWidget->width() + 38,
		backendFileSystemWidget->y() + backendFileSystemWidget->height() / 4 - titleBarHeight, 711, 529);
	};
	fileHandlingAction->getArrowPosition = [](QDockWidget* backendFileSystemWidget, int titleBarHeight, OnboardingAction*)
	{
		return QRect(backendFileSystemWidget->x() + backendFileSystemWidget->width() + 10,
			backendFileSystemWidget->y() + backendFileSystemWidget->height() / 3 + 25 - titleBarHeight,
			30, 51);
	};
	fileHandlingAction->setArrowType(OnboardingAction::ARROWTYPE::LEFTARROW);
	fileHandlingAction->setTitlePosition(QRect(38, 44, 255, 50));
	fileHandlingAction->setDescriptionPosition(QRect(38, 112, 620, 285));
	fileHandlingAction->setInstructionsTitlePosition(QRect(38, 336, 249, 47));
	fileHandlingAction->setInstructionsPosition(QRect(38, 365, 620, 150));
	fileHandlingAction->setPrevOnboardingAction(welcomeAction);
	welcomeAction->setNextOnboardingAction(fileHandlingAction);
	mOnboardingActions.push_back(fileHandlingAction);

	OnboardingAction* classifierAction = new OnboardingAction(mainWindow);
	classifierAction->setTitle(QString("The classifier"));
	classifierAction->setDescription(QString("This is the <font color='#4EBCEB'>classifier</font> tab in which"
						"you define how you want to process the signals from your bio-sensors. <br/> <br/> neuromore Studio is"
						" <font color='#4EBCEB'>hardware agnostic</font> and supports a wide variety of EEG devices as well as "
						"heartrate and GSR sensors. Simply drag a node into the graph and link it to any other node. "
						"<br/><br/>In this example we average the amplitude of the Alpha band over all channels and stream it into a"
						"custom feedback node to set the image brightness of a video.<br/><br/>"
						"You can edit the classifier by simply dragging nodes from the right into the graph."));
	classifierAction->setActivePlugin("Classifier");
	classifierAction->setInstructionsTitle("Do this now");
	classifierAction->setInstructionsDescription("<ul> <li> Drag a View node from the <i> Output </i> section"
		"of the toolbox on the right into the classifier and connect it the the <i> Alpha Band </i> node </li>"
		"<li> Optional: click on the <i> Alpha Band </i> node and choose a different band </li> </ul>");

	auto classifierWidget = classifierAction->getDockWidget();

	if (nullptr == classifierWidget) {
		return false;
	}

	auto classifierTabRect = classifierAction->getTabRect(classifierWidget);
	
	if (classifierTabRect.isEmpty()) {
		return false;
	}

	classifierAction->getWindowPosition = [](QDockWidget* classifierWidget, int titleBarHeight, OnboardingAction* classifierAction)
	{	auto classifierTabRect = classifierAction->getTabRect(classifierWidget);
		return  QRect(classifierWidget->x() - classifierWidget->width() / 11,
			classifierWidget->y() + 3 * classifierTabRect.height() / 2 + 29 - titleBarHeight,
			770, 590); 
	};
	classifierAction->getArrowPosition = [](QDockWidget* classifierWidget, int titleBarHeight, OnboardingAction* classifierAction)
	{
		auto classifierTabRect = classifierAction->getTabRect(classifierWidget);
		return QRect(classifierWidget->x(),
			classifierWidget->y() + 3 * classifierTabRect.height() / 2 - titleBarHeight,
			51, 30);
	};
	classifierAction->setArrowType(OnboardingAction::ARROWTYPE::TOPARROW);
	classifierAction->setTitlePosition(QRect(38, 11, 368, 47));
	classifierAction->setDescriptionPosition(QRect(38, 72, 710, 300));
	classifierAction->setInstructionsTitlePosition(QRect(38, 400, 243, 47));
	classifierAction->setInstructionsPosition(QRect(38, 435, 620, 150));
	classifierAction->setPrevOnboardingAction(fileHandlingAction);
	fileHandlingAction->setNextOnboardingAction(classifierAction);
	mOnboardingActions.push_back(classifierAction);
	connect(classifierAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* stateMachineAction = new OnboardingAction(mainWindow);
	stateMachineAction->setTitle(QString("The state machine"));
	stateMachineAction->setDescription(QString("In the state machine you can define the user"
		" interaction of your exprience. In this example we prompt the user to select for how"
		" long they want to do the focus training before a video starts to play. <br/> "
		"Every state machine contains a start state, an end state and a couple of action states"
		" in between.When you start your experience it will jump in at the start state.To transition"
		" between states <br/><br/>You can set conditions on transitions, for example to stop playing "
		"the video once the timer is up.<br/>The brightness and the session duration are streamed in"
		"real time through the custom feedback nodes from the classifier."));
	stateMachineAction->setActivePlugin("State Machine");;
	stateMachineAction->setInstructionsTitle("Do this now");
	stateMachineAction->setInstructionsDescription("Let's add another option to do the focus training "
		" for 5 minutes <ul> <li> Add another Action state to the graph </li> <li> Optional: click on the <i> Alpha Band </i> "
		" node and choose a different band </ul> </li>");

	auto stateMachineWidget = stateMachineAction->getDockWidget();

	if (nullptr == stateMachineWidget) {
		return false;
	}

	auto stateTabRect = stateMachineAction->getTabRect(stateMachineWidget);

	if (stateTabRect.isEmpty()) {
		return false;
	}

	stateMachineAction->getWindowPosition = [](QDockWidget* stateMachineWidget, int titleBarHeight, OnboardingAction* stateMachineAction)
	{	auto stateTabRect = stateMachineAction->getTabRect(stateMachineWidget);
	    return QRect(stateMachineWidget->x() - stateMachineWidget->width() / 22,
			stateMachineWidget->y() + 3 * stateTabRect.height() / 2 + 29 - titleBarHeight, 791, 595);
	};
	stateMachineAction->getArrowPosition = [](QDockWidget* stateMachineWidget, int titleBarHeight, OnboardingAction* stateMachineAction)
	{
		auto stateTabRect = stateMachineAction->getTabRect(stateMachineWidget);
		return QRect(stateMachineWidget->x() + stateTabRect.x(),
			stateMachineWidget->y() + 3 * stateTabRect.height() / 2 - titleBarHeight,
			51, 30);
	};
	stateMachineAction->setArrowType(OnboardingAction::ARROWTYPE::TOPARROW);
	stateMachineAction->setTitlePosition(QRect(38, 11, 630, 80));
	stateMachineAction->setDescriptionPosition(QRect(38, 85, 730, 285));
	stateMachineAction->setInstructionsTitlePosition(QRect(38, 370, 243, 47));
	stateMachineAction->setInstructionsPosition(QRect(38, 405, 613, 150));
	stateMachineAction->setPrevOnboardingAction(classifierAction);
	classifierAction->setNextOnboardingAction(stateMachineAction);
	mOnboardingActions.push_back(stateMachineAction);
	connect(stateMachineAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* experienceWindowAction = new OnboardingAction(mainWindow);
	experienceWindowAction->setTitle(QString("The experience window"));
	experienceWindowAction->setDescription(QString("In the experience window you can find the user - facing "
		"part of the application. Using this window you can show simple UIs using text buttons, images or play "
		"audio or video from a file or an URL.\nTo create more customizable experiences (e.g.a Unity game or a "
		"sound generator) you can also stream custom feedback variables to other applications using the OSC "
		"interface. You can find more information about OSC and other integrations in the docs."));
	experienceWindowAction->setActivePlugin("Experience");

	auto experienceWindowWidget = experienceWindowAction->getDockWidget();

	if (nullptr == experienceWindowWidget) {
		return false;
	}

	auto experienceTabRect = experienceWindowAction->getTabRect(experienceWindowWidget);

	if (experienceTabRect.isEmpty()) {
		return false;
	}

	experienceWindowAction->getWindowPosition = [](QDockWidget* experienceWindowWidget, int titleBarHeight, OnboardingAction* experienceWindowAction)
	{	auto experienceTabRect = experienceWindowAction->getTabRect(experienceWindowWidget);
		return QRect(experienceWindowWidget->x() - experienceWindowWidget->width() / 33,
		experienceWindowWidget->y() + 3 * experienceTabRect.height() / 2 + 29 - titleBarHeight,
			791, 290);
	};
	experienceWindowAction->getArrowPosition = [](QDockWidget* experienceWindowWidget, int titleBarHeight, OnboardingAction* experienceWindowAction)
	{
		auto experienceTabRect = experienceWindowAction->getTabRect(experienceWindowWidget);
		return QRect(experienceWindowWidget->x() + experienceTabRect.x(),
			experienceWindowWidget->y() + 3 * experienceTabRect.height() / 2 - titleBarHeight,
			51, 30);
	};

	experienceWindowAction->setArrowType(OnboardingAction::ARROWTYPE::TOPARROW);
	experienceWindowAction->setTitlePosition(QRect(38, 11, 689, 38));
	experienceWindowAction->setDescriptionPosition(QRect(38, 72, 680, 180));
	experienceWindowAction->setPrevOnboardingAction(stateMachineAction);
	stateMachineAction->setNextOnboardingAction(experienceWindowAction);
	mOnboardingActions.push_back(experienceWindowAction);
	connect(experienceWindowAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* debugWindowAction = new OnboardingAction(mainWindow);
	debugWindowAction->setTitle(QString("The debug window"));
	debugWindowAction->setDescription(QString("On the bottom you can find different windows "
		"to debug the current application output."));
	debugWindowAction->setActivePlugin("Raw EEG");

	auto debugWindowWidget = debugWindowAction->getDockWidget();

	if (nullptr == debugWindowWidget) {
		return false;
	}

	debugWindowAction->getWindowPosition = [](QDockWidget* debugWindowWidget, int titleBarHeight, OnboardingAction*)
	{
		return  QRect(debugWindowWidget->x() - debugWindowWidget->width() / 34,
			debugWindowWidget->y() - 226 - titleBarHeight, 791, 196);
	};
	debugWindowAction->getArrowPosition = [](QDockWidget* debugWindowWidget, int titleBarHeight, OnboardingAction* )
	{
		return QRect(debugWindowWidget->x() + 131, debugWindowWidget->y() - 30 - titleBarHeight,
			51, 30);
	};
	debugWindowAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);
	debugWindowAction->setTitlePosition(QRect(34, 27, 689, 45));
	debugWindowAction->setDescriptionPosition(QRect(34, 88, 680, 99));
	debugWindowAction->setArrowPosition(OnboardingAction::ARROWTYPE::DOWNARROW, QRect(debugWindowWidget->x() + 131, debugWindowWidget->y() - 30 - titleBarHeight, 51, 30));
	debugWindowAction->setPrevOnboardingAction(experienceWindowAction);
	experienceWindowAction->setNextOnboardingAction(debugWindowAction);
	mOnboardingActions.push_back(debugWindowAction);
	connect(debugWindowAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* feedbackViewAction = new OnboardingAction(mainWindow);
	feedbackViewAction->setTitle(QString("The feedback view"));
	feedbackViewAction->setDescription(QString("In the feedback view you can see the live feedback values of the custom "
		"variables defined in the classifier."));
	feedbackViewAction->setActivePlugin("Feedback");

	auto feedbackViewWidget = feedbackViewAction->getDockWidget();

	if (nullptr == feedbackViewWidget) {
		return false;
	}

	feedbackViewAction->getWindowPosition = [](QDockWidget* feedbackViewWidget, int titleBarHeight, OnboardingAction *)
	{
		return  QRect(feedbackViewWidget->x() - feedbackViewWidget->width() / 9,
			feedbackViewWidget->y() -215 - titleBarHeight, 795, 185);
	};

	auto feedbackViewTabRect = feedbackViewAction->getTabRect(feedbackViewWidget);
	if (feedbackViewTabRect.isEmpty()) {
		return false;
	}
	feedbackViewAction->getArrowPosition = [](QDockWidget* feedbackViewWidget, int titleBarHeight, OnboardingAction* feedbackViewAction)
	{
		auto feedbackViewTabRect = feedbackViewAction->getTabRect(feedbackViewWidget);
		return QRect(feedbackViewWidget->x()  + feedbackViewTabRect.x(),
			feedbackViewWidget->y() - 30 - titleBarHeight, 51, 30);
	};
	feedbackViewAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);
	feedbackViewAction->setTitlePosition(QRect(37, 20, 689, 40));
	feedbackViewAction->setDescriptionPosition(QRect(37, 81, 680, 99));
	feedbackViewAction->setArrowPosition(OnboardingAction::ARROWTYPE::DOWNARROW, QRect(debugWindowWidget->x() + feedbackViewTabRect.x(),
																					   debugWindowWidget->y() - 30 - titleBarHeight, 51, 30));
	feedbackViewAction->setPrevOnboardingAction(debugWindowAction);
	debugWindowAction->setNextOnboardingAction(feedbackViewAction);
	mOnboardingActions.push_back(feedbackViewAction);
	connect(feedbackViewAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* customParametersAction = new OnboardingAction(mainWindow);
	customParametersAction->setTitle(QString("Using custom parameters"));
	customParametersAction->setDescription(QString("You can parameterize the experience and adjust those parameters "
		"either before or during a session."));
	customParametersAction->setActivePlugin("Parameters");

	auto customParametersWidget = customParametersAction->getDockWidget();

	if (nullptr == customParametersWidget) {
		return false;
	}

	customParametersAction->getWindowPosition = [](QDockWidget* customParametersWidget, int titleBarHeight, OnboardingAction*)
	{
		return  QRect(0, customParametersWidget->y() - 267 - titleBarHeight, 791, 237);
	};

	auto customParametersTabRect = customParametersAction->getTabRect(customParametersWidget);

	if (customParametersTabRect.isEmpty()) {
		return false;
	}

	customParametersAction->getArrowPosition = [](QDockWidget* customParametersWidget, int titleBarHeight, OnboardingAction* customParametersAction)
	{
		auto customParametersTabRect = customParametersAction->getTabRect(customParametersWidget);
		return QRect(customParametersTabRect.x(),
			customParametersWidget->y() - 30 - titleBarHeight, 51, 30);
	};
	customParametersAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);

	OnboardingAction* sessionControlAction = new OnboardingAction(mainWindow);
	sessionControlAction->setActivePlugin("Session Control");
	auto sessionControlWidget = sessionControlAction->getDockWidget();

	if (nullptr == sessionControlWidget) {
		return false;
	}

	customParametersAction->setWindowPosition(QRect(0, sessionControlWidget->y() - 267 - titleBarHeight, 791, 237));
	customParametersAction->setTitlePosition(QRect(37, 20, 689, 45));
	customParametersAction->setDescriptionPosition(QRect(38, 80, 673, 99));
	customParametersAction->setPrevOnboardingAction(feedbackViewAction);
	feedbackViewAction->setNextOnboardingAction(customParametersAction);
	mOnboardingActions.push_back(customParametersAction);
	connect(customParametersAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	sessionControlAction->setTitle(QString("Starting a session"));
	sessionControlAction->setDescription(QString("Now it's time to run the experience. Click on the \"Go\" button "
		"to start a session. You can also use one of our existing neurofeedback  visualizations by clicking on "
		"the button with the eye icon. "));
	sessionControlAction->getWindowPosition = [](QDockWidget* sessionControlWidget, int titleBarHeight, OnboardingAction* sessionControlAction)
	{
		return QRect(0, sessionControlWidget->y() - 267 - titleBarHeight, 791, 237);
	};
	sessionControlAction->getArrowPosition = [](QDockWidget* sessionControlWidget, int titleBarHeight, OnboardingAction* )
	{
		return QRect(20, sessionControlWidget->y() - 30 - titleBarHeight,
			51, 30);
	};
	sessionControlAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);
	sessionControlAction->setTitlePosition(QRect(38, 11, 689, 45));
	sessionControlAction->setDescriptionPosition(QRect(38, 80, 680, 99));
	sessionControlAction->setPrevOnboardingAction(customParametersAction);
	customParametersAction->setNextOnboardingAction(sessionControlAction);
	mOnboardingActions.push_back(sessionControlAction);
	connect(sessionControlAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* endTutorialAction = new OnboardingAction(mainWindow);
	endTutorialAction->setTitle(QString("Running the experience"));
	endTutorialAction->setDescription(QString("It's time to use the neurofeedback application in action. "
		"Click on one of the 3 buttons in the experience window and see how the image brightness of the "
		"video changes according to the average Alpha band amplitude."));
	endTutorialAction->setActivePlugin("Experience");
	endTutorialAction->getWindowPosition = [](QDockWidget*, int titleBarHeight, OnboardingAction*)
	{
		return QRect(27, 60 - titleBarHeight, 670, 279);
	};
	endTutorialAction->setWindowPosition(QRect(27, 60 - titleBarHeight, 670, 279));
	endTutorialAction->setTitlePosition(QRect(38, 11, 689, 45));
	endTutorialAction->setDescriptionPosition(QRect(38, 80, 590, 95));
	endTutorialAction->setPrevOnboardingAction(sessionControlAction);
	sessionControlAction->setNextOnboardingAction(endTutorialAction);
	mOnboardingActions.push_back(endTutorialAction);
	connect(endTutorialAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	const uint32 numRegisteredPlugins = GetQtBaseManager()->GetPluginManager()
		->GetNumActivePlugins();
	BackendFileSystemPlugin* backendFileSystemPlugin = nullptr;
	for (uint32 i = 0; i < numRegisteredPlugins; ++i)
	{
		Plugin* plugin = GetPluginManager()->GetActivePlugin(i);
		if ("Back-End File System" == std::string(plugin->GetName()))
		{
			backendFileSystemPlugin = dynamic_cast<BackendFileSystemPlugin*>(plugin);
			break;
		}
	}
	if (nullptr == backendFileSystemPlugin)
	{
		return false;
	}
	QVector<QString> itemPath = { "examples", "GettingStarted", "FocusTrainer"};
	backendFileSystemPlugin->ExpandByPath(itemPath);

	// loads the "FirstFocusTrainerClassifierTestSystem" classifier and "FirstFocusTrainerStateMachine" state machine for the tour.
	GetFileManager()->OpenClassifier(FileManager::LOCATION_BACKEND, "ef5cff5a-d569-4f5d-8e00-03049540ff42", "FirstFocusTrainerClassifierTestSystem", -1);
	GetFileManager()->OpenStateMachine(FileManager::LOCATION_BACKEND, "33d18723-73a4-42f5-9869-7bb3da0530c1", "FirstFocusTrainerStateMachine", -1);

	return true;
}

QRect TourManager::getTabRect(const QDockWidget* DWidget) const
{
	auto mainWindow = GetMainWindow();
	Q_FOREACH(QTabBar * tabBar, mainWindow->findChildren<QTabBar*>())
	{
		for (int i = 0; i < tabBar->count(); ++i)
		{
			if (DWidget == (QDockWidget*)tabBar->tabData(i).toULongLong())
			{
				return tabBar->tabRect(i);
			}
		}
	}

	return QRect();
}

void TourManager::OnWindowClosed()
{
	TourManager::CurrentOnboardingAction->OnCloseAction();
}

void TourManager::startTour()
{
	TourManager::CurrentOnboardingAction = mOnboardingActions[0];
	TourManager::CurrentOnboardingAction->Invoke();
}
