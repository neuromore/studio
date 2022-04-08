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
	welcomeAction->setDescription(QString("Let us give you a quick tour to show you how to create a simple neurofeedback application in neuromore Studio."));
	welcomeAction->setWindowPosition(QRect(563, 281 - titleBarHeight, 729, 222));
	welcomeAction->setTitlePosition(QRect(38, 35, 545, 47));
	welcomeAction->setDescriptionPosition(QRect(38, 118, 636, 50));
	mOnboardingActions.push_back(welcomeAction);

	OnboardingAction* fileHandlingAction = new OnboardingAction(mainWindow);
	fileHandlingAction->setTitle(QString("Handling files"));
	fileHandlingAction->setDescription((QString("Every neurofeedback application consists at "
		"least of a classifier in which you define the signal processing pipeline and usually also "
		"of a state machine for your application logic.\nFor the intro we've already loaded the "
		"classifier and state machine of out getting started guide.\nAll files in the examples folder"
		"are read-only. To use them as a basis for your own projects you can copy them to your "
		"personal folder which makes them editable.")));
	fileHandlingAction->setActivePlugin("Back-End File System");
	auto backendFileSystemWidget = fileHandlingAction->getDockWidget();

	if (nullptr == backendFileSystemWidget) {
		return false;
	}

	fileHandlingAction->setWindowPosition(QRect(backendFileSystemWidget->x()+backendFileSystemWidget->width() + 38,
												backendFileSystemWidget->y()+backendFileSystemWidget->height() / 4 - titleBarHeight, 700, 396));
	fileHandlingAction->setTitlePosition(QRect(38, 44, 255, 50));
	fileHandlingAction->setDescriptionPosition(QRect(38, 129, 620, 203));
	fileHandlingAction->setPrevOnboardingAction(welcomeAction);
	fileHandlingAction->setArrowPosition(OnboardingAction::ARROWTYPE::LEFTARROW, QRect(backendFileSystemWidget->x()+backendFileSystemWidget->width() + 10,
																					   backendFileSystemWidget->y()+backendFileSystemWidget->height() / 3 + 25 - titleBarHeight,
																					   30, 51));
	welcomeAction->setNextOnboardingAction(fileHandlingAction);
	mOnboardingActions.push_back(fileHandlingAction);

	OnboardingAction* editorAction = new OnboardingAction(mainWindow);
	editorAction->setTitle(QString("The editor window"));
	editorAction->setDescription(QString("This is the main editor window in which you can access all 3 parts "
		"of a neurofeedback application: the classifier, the state machine and the experience window.\n"
		"Right now you're seeing the classifier tab in which you define the signal processing pipeline "
		"by dragging nodes from the toolbox on the right into the graph.\nIn this example we average "
		"the amplitude of the Alpha band over all channels and over 3s and steam it into a custom"
		"feedback node to set the image brightness of a video."));
	editorAction->setActivePlugin("Classifier");

	auto classifierWidget = editorAction->getDockWidget();

	if (nullptr == classifierWidget) {
		return false;
	}

	auto classifierTabRect = editorAction->getTabRect(classifierWidget);

	if (classifierTabRect.isEmpty()) {
		return false;
	}

	editorAction->setWindowPosition(QRect(classifierWidget->x() - backendFileSystemWidget->width() / 2,
										  classifierWidget->y() + 3 * classifierTabRect.height() / 2 + 29 - titleBarHeight, 765, 330));
	editorAction->setTitlePosition(QRect(38, 11, 368, 47));
	editorAction->setDescriptionPosition(QRect(38, 72, 673, 201));
	editorAction->setArrowPosition(OnboardingAction::ARROWTYPE::TOPARROW, QRect(classifierWidget->x(),
																				classifierWidget->y() + 3 * classifierTabRect.height() / 2 - titleBarHeight,
																				51, 30));
	editorAction->setPrevOnboardingAction(fileHandlingAction);
	fileHandlingAction->setNextOnboardingAction(editorAction);
	mOnboardingActions.push_back(editorAction);
	connect(editorAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* stateMachineAction = new OnboardingAction(mainWindow);
	stateMachineAction->setTitle(QString("Defining the user flow in the state machine"));
	stateMachineAction->setDescription(QString("The state machine contains the interaction logic of your application.\n"
		"In this example we prompt the user to select for how long they want to do the focus training before a video "
		"starts to play. You can set conditions on transitions, for example to stop playing the video once the timer "
		"is up. The brightness and the session duration are streamed in real time through the custom feedback nodes "
		"from the classifier."));
	stateMachineAction->setActivePlugin("State Machine");;

	auto stateMachineWidget = stateMachineAction->getDockWidget();

	if (nullptr == stateMachineWidget) {
		return false;
	}

	auto stateTabRect = stateMachineAction->getTabRect(stateMachineWidget);

	if (stateTabRect.isEmpty()) {
		return false;
	}

	stateMachineAction->setWindowPosition(QRect(classifierWidget->x() - backendFileSystemWidget->width() / 4,
												classifierWidget->y() + 3 * stateTabRect.height() / 2 + 29 - titleBarHeight, 791, 313));
	stateMachineAction->setTitlePosition(QRect(38, 11, 630, 80));
	stateMachineAction->setDescriptionPosition(QRect(38, 113, 690, 203));
	stateMachineAction->setArrowPosition(OnboardingAction::ARROWTYPE::TOPARROW, QRect(classifierWidget->x() + stateTabRect.x(),
																					  classifierWidget->y() + 3 * stateTabRect.height() / 2 - titleBarHeight,
																					  51, 30));
	stateMachineAction->setPrevOnboardingAction(editorAction);
	editorAction->setNextOnboardingAction(stateMachineAction);
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

	experienceWindowAction->setWindowPosition(QRect(classifierWidget->x() - backendFileSystemWidget->width() / 6,
													classifierWidget->y() + 3 * experienceTabRect.height() / 2 + 29 - titleBarHeight, 791, 290));
	experienceWindowAction->setTitlePosition(QRect(38, 11, 689, 38));
	experienceWindowAction->setDescriptionPosition(QRect(38, 72, 680, 180));
	experienceWindowAction->setArrowPosition(OnboardingAction::ARROWTYPE::TOPARROW, QRect(classifierWidget->x() + experienceTabRect.x(),
																						  classifierWidget->y() + 3 * experienceTabRect.height() / 2 - titleBarHeight,
																						  51, 30));
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

	debugWindowAction->setWindowPosition(QRect(debugWindowWidget->x() - backendFileSystemWidget->width() / 6, debugWindowWidget->y() - 226 - titleBarHeight, 791, 196));
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

	auto feedbackViewTabRect = feedbackViewAction->getTabRect(feedbackViewWidget);

	if (feedbackViewTabRect.isEmpty()) {
		return false;
	}

	feedbackViewAction->setWindowPosition(QRect(debugWindowWidget->x() - backendFileSystemWidget->width() / 4, debugWindowWidget->y() - 215 - titleBarHeight, 795, 185));
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

	auto customParametersTabRect = customParametersAction->getTabRect(customParametersWidget);

	if (customParametersTabRect.isEmpty()) {
		return false;
	}

	OnboardingAction* sessionControlAction = new OnboardingAction(mainWindow);
	sessionControlAction->setActivePlugin("Session Control");
	auto sessionControlWidget = sessionControlAction->getDockWidget();

	if (nullptr == sessionControlWidget) {
		return false;
	}

	customParametersAction->setWindowPosition(QRect(0, sessionControlWidget->y() - 267 - titleBarHeight, 791, 237));
	customParametersAction->setTitlePosition(QRect(37, 20, 689, 45));
	customParametersAction->setDescriptionPosition(QRect(38, 80, 673, 99));
	customParametersAction->setArrowPosition(OnboardingAction::ARROWTYPE::DOWNARROW, QRect(customParametersTabRect.x(),
																						   sessionControlWidget->y() - 30 - titleBarHeight, 51, 30));
	customParametersAction->setPrevOnboardingAction(feedbackViewAction);
	feedbackViewAction->setNextOnboardingAction(customParametersAction);
	mOnboardingActions.push_back(customParametersAction);
	connect(customParametersAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	sessionControlAction->setTitle(QString("Starting a session"));
	sessionControlAction->setDescription(QString("Now it's time to run the experience. Click on the \"Go\" button "
		"to start a session. You can also use one of our existing neurofeedback  visualizations by clicking on "
		"the button with the eye icon. "));
	sessionControlAction->setWindowPosition(QRect(0, sessionControlWidget->y() - 267 - titleBarHeight, 791, 237));
	sessionControlAction->setTitlePosition(QRect(38, 11, 689, 45));
	sessionControlAction->setDescriptionPosition(QRect(38, 80, 680, 99));
	sessionControlAction->setArrowPosition(OnboardingAction::ARROWTYPE::DOWNARROW, QRect(20, sessionControlWidget->y() - 30 - titleBarHeight, 51, 30));
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

void TourManager::OnWindowClosed()
{
	TourManager::CurrentOnboardingAction->OnCloseAction();
}

void TourManager::startTour()
{
	TourManager::CurrentOnboardingAction = mOnboardingActions[0];
	TourManager::CurrentOnboardingAction->Invoke();
}
