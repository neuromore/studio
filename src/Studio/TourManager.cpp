#include "TourManager.h"
#include "MainWindow.h"

OnboardingAction* TourManager::CurrentOnboardingAction = nullptr;

TourManager::TourManager()
{
};

TourManager::~TourManager()
{
	foreach(auto action, mOnboardingActions) 
	{
		delete action;
	}
}

bool TourManager::InitOnboardingActions()
{
	OnboardingAction* prevAction;
	OnboardingAction* nextAction;
	auto appManager = GetManager();

	OnboardingAction* welcomeAction = new OnboardingAction;
	welcomeAction->setTitle(QString("Welcome to neuromore Studio"));
	welcomeAction->setDescription(QString("Let us give you a quick tour to show you how to create a  simpleneurofeedback application in neuromore Studio"));
	welcomeAction->setWindowPosition(QRect(563, 281, 729, 222));
	welcomeAction->setTitlePosition(QRect(38, 35, 545, 47));
	welcomeAction->setDescriptionPosition(QRect(38, 118, 636, 50));
	welcomeAction->setImagePosition(QRect(670, 35, 30, 32));
	mOnboardingActions.push_back(welcomeAction);

	OnboardingAction* fileHandlingAction = new OnboardingAction;
	fileHandlingAction->setTitle(QString("Handling files"));
	fileHandlingAction->setDescription((QString("Every neurofeedback application consists at "
		"least of a classifier in which you define the signal processing pipeline and usually also "
		"of a state machine for your application logic.\n For the intro we've already loaded the "
		"classifier and state machine of out getting started guide.\n All files in the examples folder"
		"are read-only. To use them as a basis for your own projects you can copy them to your "
		"personal folder which makes them editable.")));
	auto backendFileSystemWidget = getDockWidget("Back-End File System");

	if (nullptr == backendFileSystemWidget) {
		return false;
	}

	fileHandlingAction->setWindowPosition(QRect(backendFileSystemWidget->x()+backendFileSystemWidget->width() + 40,
												backendFileSystemWidget->y()+backendFileSystemWidget->height() / 4, 820, 396));
	fileHandlingAction->setTitlePosition(QRect(38, 44, 249, 50));
	fileHandlingAction->setDescriptionPosition(QRect(38, 129, 620, 203));
	fileHandlingAction->setActivePlugin("Back-End File System");
	fileHandlingAction->setPrevOnboardingAction(welcomeAction);
	fileHandlingAction->setArrowPosition(OnboardingAction::ARROWTYPE::LEFTARROW, QRect(backendFileSystemWidget->x()+backendFileSystemWidget->width() + 23,
																					   backendFileSystemWidget->y()+backendFileSystemWidget->height() / 3 + 25,
																					   52, 36));
	welcomeAction->setNextOnboardingAction(fileHandlingAction);
	mOnboardingActions.push_back(fileHandlingAction);

	OnboardingAction* editorAction = new OnboardingAction;
	editorAction->setTitle(QString("The editor Window"));
	editorAction->setDescription(QString("This is the main editor window in which you can access all 3 parts "
		"of a neurofeedback application: the classifier, the state machine and the experience window.\n"
		"Right now you're seeing the classifier tab in which you define the signal processing pipeline "
		"by dragging nodes from the toolbox on the right into the graph.\n In this example we average "
		"the amplitude of the Alpha band over all channels and over 3s and steam it into a custom"
		"feedback node to set the image brightness of a video"));

	auto classifierWidget = getDockWidget("Classifier");

	if (nullptr == classifierWidget) {
		return false;
	}

	auto classifierTabRect = getTabRect(classifierWidget);
	editorAction->setWindowPosition(QRect(classifierWidget->x() - backendFileSystemWidget->width() / 2,
										  classifierWidget->y() + 5 * classifierTabRect.height() / 2, 765, 330));
	editorAction->setTitlePosition(QRect(38, 11, 368, 47));
	editorAction->setDescriptionPosition(QRect(38, 72, 700, 203));
	editorAction->setActivePlugin("Classifier");
	editorAction->setArrowPosition(OnboardingAction::ARROWTYPE::TOPARROW, QRect(classifierWidget->x(),
																				classifierWidget->y() + 3 * classifierTabRect.height() / 2,
																				53, 36));
	editorAction->setPrevOnboardingAction(fileHandlingAction);
	fileHandlingAction->setNextOnboardingAction(editorAction);
	mOnboardingActions.push_back(editorAction);
	connect(editorAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* stateMachineAction = new OnboardingAction;
	stateMachineAction->setTitle(QString("Defining the user flow in the state machine"));
	stateMachineAction->setDescription(QString("The state machine contains the interaction logic of your application.\n"
		"In this example we prompt the user to select for how long they want to do the focus training before a video "
		"starts to play.You can set conditions on transitions, for example to stop playing the video once the timer "
		"is up. The brightnessand the session duration are streamed in real time through the custom feedback nodes "
		" from the classifier. "));

	auto stateMachineWidget = getDockWidget("State Machine");

	if (nullptr == stateMachineWidget) {
		return false;
	}

	auto stateTabRect = getTabRect(stateMachineWidget);
	stateMachineAction->setWindowPosition(QRect(classifierWidget->x() - backendFileSystemWidget->width() / 4,
												classifierWidget->y() + 5 * stateTabRect.height() / 2, 791, 313));
	stateMachineAction->setTitlePosition(QRect(38, 11, 689, 80));
	stateMachineAction->setDescriptionPosition(QRect(38, 113, 690, 203));
	stateMachineAction->setActivePlugin("State Machine");;
	stateMachineAction->setArrowPosition(OnboardingAction::ARROWTYPE::TOPARROW, QRect(classifierWidget->x() + stateTabRect.x(),
																					  classifierWidget->y() + 3 * stateTabRect.height() / 2,
																					  53, 36));
	stateMachineAction->setPrevOnboardingAction(editorAction);
	editorAction->setNextOnboardingAction(stateMachineAction);
	mOnboardingActions.push_back(stateMachineAction);
	connect(stateMachineAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* experienceWindowAction = new OnboardingAction;
	experienceWindowAction->setTitle(QString("The experience window"));
	experienceWindowAction->setDescription(QString("In the experience window you can find the user - facing "
		"part of the application.Using this window you can show simple UIs using text buttons, images or play "
		"audio or video from a file or an URL.\nTo create more customizable experiences(e.g.a Unity game or a "
		"sound generator) you can also stream custom feedback variables to other applications using the OSC "
		"interface.You can find more information about OSCand other integrations in the docs."));

	auto experienceWindowWidget = getDockWidget("Experience");

	if (nullptr == experienceWindowWidget) {
		return false;
	}

	auto experienceTabRect = getTabRect(experienceWindowWidget);
	experienceWindowAction->setWindowPosition(QRect(classifierWidget->x() - backendFileSystemWidget->width() / 6,
													classifierWidget->y() + 5 * experienceTabRect.height() / 2, 791, 290));
	experienceWindowAction->setTitlePosition(QRect(38, 11, 689, 38));
	experienceWindowAction->setDescriptionPosition(QRect(38, 72, 680, 180));
	experienceWindowAction->setActivePlugin("Experience");
	experienceWindowAction->setArrowPosition(OnboardingAction::ARROWTYPE::TOPARROW, QRect(classifierWidget->x() + experienceTabRect.x(),
																						  classifierWidget->y() + 3 * experienceTabRect.height() / 2,
																						  53, 36));
	experienceWindowAction->setPrevOnboardingAction(stateMachineAction);
	stateMachineAction->setNextOnboardingAction(experienceWindowAction);
	mOnboardingActions.push_back(experienceWindowAction);
	connect(experienceWindowAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* debugWindowAction = new OnboardingAction;

	auto debugWindowWidget = getDockWidget("Raw EEG");

	if (nullptr == debugWindowWidget) {
		return false;
	}

	debugWindowAction->setTitle(QString("The debug window"));
	debugWindowAction->setDescription(QString("On the bottom you can find different windows"
		"to debug the current application output."));
	debugWindowAction->setWindowPosition(QRect(debugWindowWidget->x() - backendFileSystemWidget->width() / 6, debugWindowWidget->y() - 226, 791, 196));
	debugWindowAction->setTitlePosition(QRect(34, 27, 689, 45));
	debugWindowAction->setDescriptionPosition(QRect(34, 88, 680, 99));
	debugWindowAction->setActivePlugin("Raw EEG");
	debugWindowAction->setArrowPosition(OnboardingAction::ARROWTYPE::DOWNARROW, QRect(debugWindowWidget->x() + 131, debugWindowWidget->y() - 30, 53, 36));
	debugWindowAction->setPrevOnboardingAction(experienceWindowAction);
	experienceWindowAction->setNextOnboardingAction(debugWindowAction);
	mOnboardingActions.push_back(debugWindowAction);
	connect(debugWindowAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* feedbackViewAction = new OnboardingAction;

	auto feedbackViewWidget = getDockWidget("Feedback");

	if (nullptr == feedbackViewWidget) {
		return false;
	}

	auto feedbackViewTabRect = getTabRect(feedbackViewWidget);

	feedbackViewAction->setTitle(QString("The feedback view"));
	feedbackViewAction->setDescription(QString("On the bottom you can find different windows to debug "
		"the current application output."));
	feedbackViewAction->setWindowPosition(QRect(debugWindowWidget->x() - backendFileSystemWidget->width() / 4, debugWindowWidget->y() - 205, 795, 185));
	feedbackViewAction->setTitlePosition(QRect(37, 20, 689, 40));
	feedbackViewAction->setDescriptionPosition(QRect(37, 81, 680, 99));
	feedbackViewAction->setActivePlugin("Feedback");
	feedbackViewAction->setArrowPosition(OnboardingAction::ARROWTYPE::DOWNARROW, QRect(debugWindowWidget->x() + feedbackViewTabRect.x(),
																					   debugWindowWidget->y() - 20, 53, 36));
	feedbackViewAction->setPrevOnboardingAction(debugWindowAction);
	debugWindowAction->setNextOnboardingAction(feedbackViewAction);
	mOnboardingActions.push_back(feedbackViewAction);
	connect(feedbackViewAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* customParametersAction = new OnboardingAction;

	auto customParametersWidget = getDockWidget("Parameters");

	if (nullptr == customParametersWidget) {
		return false;
	}

	auto customParametersTabRect = getTabRect(customParametersWidget);

	auto sessionControlWidget = getDockWidget("Session Control");

	if (nullptr == sessionControlWidget) {
		return false;
	}

	customParametersAction->setTitle(QString("Using custom parameters"));
	customParametersAction->setDescription(QString("You can parameterize the experience and adjust those parameters "
		"either before or during a session."));
	customParametersAction->setWindowPosition(QRect(0, sessionControlWidget->y() - 257, 791, 237));
	customParametersAction->setTitlePosition(QRect(37, 20, 689, 40));
	customParametersAction->setDescriptionPosition(QRect(38, 72, 673, 99));
	customParametersAction->setActivePlugin("Parameters");
	customParametersAction->setArrowPosition(OnboardingAction::ARROWTYPE::DOWNARROW, QRect(customParametersTabRect.x(),
																						   sessionControlWidget->y() - 20, 53, 36));
	customParametersAction->setPrevOnboardingAction(feedbackViewAction);
	feedbackViewAction->setNextOnboardingAction(customParametersAction);
	mOnboardingActions.push_back(customParametersAction);
	connect(customParametersAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* sessionControlAction = new OnboardingAction;
	sessionControlAction->setTitle(QString("Starting a session"));
	sessionControlAction->setDescription(QString("Now it's time to run the experience. Click on the 'Go' button "
		"to start a session. You can also use one of our existing neurofeedback  visualizations by clicking on "
		"the button with the eye icon. "));
	sessionControlAction->setWindowPosition(QRect(0, sessionControlWidget->y() - 257, 791, 237));
	sessionControlAction->setTitlePosition(QRect(38, 11, 689, 40));
	sessionControlAction->setDescriptionPosition(QRect(38, 72, 680, 99));
	sessionControlAction->setActivePlugin("Session Control");
	sessionControlAction->setArrowPosition(OnboardingAction::ARROWTYPE::DOWNARROW, QRect(20, sessionControlWidget->y() - 20, 53, 36));
	sessionControlAction->setPrevOnboardingAction(customParametersAction);
	customParametersAction->setNextOnboardingAction(sessionControlAction);
	mOnboardingActions.push_back(sessionControlAction);
	connect(sessionControlAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* endTutorialAction = new OnboardingAction;
	endTutorialAction->setTitle(QString("Running the experience"));
	endTutorialAction->setDescription(QString("It's time to use the neurofeedback application in action. "
		"Click on one of the 3 buttons in the experience window and see how the image brightness of the "
		"video changes according to the average Alpha band amplitude."));
	endTutorialAction->setWindowPosition(QRect(27, 60, 670, 279));
	endTutorialAction->setTitlePosition(QRect(38, 11, 689, 40));
	endTutorialAction->setDescriptionPosition(QRect(38, 72, 590, 95));
	endTutorialAction->setActivePlugin("Experience");
	endTutorialAction->setPrevOnboardingAction(sessionControlAction);
	sessionControlAction->setNextOnboardingAction(endTutorialAction);
	mOnboardingActions.push_back(endTutorialAction);
	connect(endTutorialAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

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

QDockWidget* TourManager::getDockWidget(std::string pluginName) const
{
	const uint32 numRegisteredPlugins = GetQtBaseManager()->GetPluginManager()
		->GetNumActivePlugins();
	for (uint32 i = 0; i < numRegisteredPlugins; ++i)
	{
		Plugin* plugin = GetPluginManager()->GetActivePlugin(i);
		if (std::string(plugin->GetName()) == pluginName)
		{
			return plugin->GetDockWidget();
		}
	}
	return nullptr;
}

QRect TourManager::getTabRect(const QDockWidget* DWidget) const
{
	auto mainWindow = GetMainWindow();
	Q_FOREACH(QTabBar * tabBar, mainWindow->findChildren<QTabBar*>()) {
		for (int i = 0; i < tabBar->count(); ++i) {
			if (DWidget == (QDockWidget*)tabBar->tabData(i).toULongLong())
			{
				return tabBar->tabRect(i);
			}
		}
	}
}
