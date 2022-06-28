#include "TourManager.h"
#include "Plugins/BackendFileSystem/BackendFileSystemPlugin.h"
#include "MainWindow.h"
#include "OnboardingAction.h"
#include "AppManager.h"


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
		if (nullptr != menuWidget)
		{
			titleBarHeight = menuWidget->height() + 20;
		}
	#endif

	OnboardingAction* welcomeAction = new OnboardingAction(mainWindow);
	welcomeAction->setTitle("Welcome to neuromore Studio");
	welcomeAction->setDescription("your all-in-one platform to create any kind of bio-sensor application - "
		"from state-of-the art bio-feedback experiences over stress and health monitoring to diagnosis tools based on biodata.<br/>");
	welcomeAction->setTitlePosition(QRect(38, 35, 545, 47));
	welcomeAction->setDescriptionPosition(QRect(38, 120, 680, 121));
	welcomeAction->getWindowPosition = [](QDockWidget*, int titleBarHeight, OnboardingAction*)
	{ 
		return QRect(563 - titleBarHeight, 281, 729, 280);
	};

	mOnboardingActions.push_back(welcomeAction);

	OnboardingAction* sessionControlAction = new OnboardingAction(mainWindow);
	sessionControlAction->setActivePlugin("Session Control");

	auto sessionControlWidget = sessionControlAction->getDockWidget();

	if (nullptr == sessionControlWidget)
	{
		return false;
	}

	sessionControlAction->setTitle("A simple Attention Trainer");
	sessionControlAction->setDescription("Let's take a look at an example of a very simple biofeedback experience "
		"that uses a simulated EEG headset. The goal of the experience is to <font color='#4EBCEB'>train "
		"the user's attention</font> by raising their Alpha band brain activity: if the Alpha "
		"amplitude is relatively high then the video is clear, otherwise the screen "
		"brightness decreases.<br/>");
	sessionControlAction->setInstructionsTitle("Do this now");
	sessionControlAction->setInstructionsDescription("<ul> <li>Start the neurofeedback experience by clicking on the 'go' button in the <i>Session Control</i> window</li> </ul>");
	sessionControlAction->getWindowPosition = [](QDockWidget* sessionControlWidget, int titleBarHeight, OnboardingAction*)
	{
		return QRect(10, sessionControlWidget->y() - 350 - titleBarHeight, 791, 330);
	};
	sessionControlAction->getArrowPosition = [](QDockWidget* sessionControlWidget, int titleBarHeight, OnboardingAction*)
	{
		return QRect(30, sessionControlWidget->y() - 20 - titleBarHeight,
			51, 30);
	};

	sessionControlAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);
	sessionControlAction->setTitlePosition(QRect(38, 11, 689, 45));
	sessionControlAction->setDescriptionPosition(QRect(38, 80, 680, 130));
	sessionControlAction->setInstructionsTitlePosition(QRect(38, 210, 243, 47));
	sessionControlAction->setInstructionsPosition(QRect(38, 255, 680, 110));
	sessionControlAction->setPrevOnboardingAction(welcomeAction);
	welcomeAction->setNextOnboardingAction(sessionControlAction);
	mOnboardingActions.push_back(sessionControlAction);
	connect(sessionControlAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* experienceWindowAction = new OnboardingAction(mainWindow);
	experienceWindowAction->setActivePlugin("Experience");

	experienceWindowAction->setTitle("A simple Attention Trainer");
	experienceWindowAction->setDescription("This experience consists of multiple steps. Before the actual "
		"neurofeedback session starts the user is prompted to select how long they want to do the attention "
		"training. All of this is configured in neuromore Studio.");
	experienceWindowAction->setInstructionsTitle("Do this now");
	experienceWindowAction->setInstructionsDescription("<ul> <li>Click on one of the buttons in the Experience window to select the duration of the Session</li>"
		"<li> Observe the video brightness change. When you're ready continue to the next step </li></ul>");

	experienceWindowAction->getWindowPosition = [](QDockWidget* classifierWidget, int titleBarHeight, OnboardingAction*)
	{
		return QRect(10, classifierWidget->y() + classifierWidget->height() - 400 - titleBarHeight, 791, 370);
	};
	experienceWindowAction->getArrowPosition = [](QDockWidget* classifierWidget, int titleBarHeight, OnboardingAction*)
	{
		return QRect(580, classifierWidget->y() + classifierWidget->height() - 30 - titleBarHeight,
			51, 30);
	};

	experienceWindowAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);
	experienceWindowAction->setTitlePosition(QRect(38, 11, 689, 45));
	experienceWindowAction->setDescriptionPosition(QRect(38, 80, 680, 100));
	experienceWindowAction->setInstructionsTitlePosition(QRect(38, 190, 243, 47));
	experienceWindowAction->setInstructionsPosition(QRect(38, 240, 680, 100));
	experienceWindowAction->setPrevOnboardingAction(sessionControlAction);
	sessionControlAction->setNextOnboardingAction(experienceWindowAction);
	mOnboardingActions.push_back(experienceWindowAction);
	connect(experienceWindowAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* stopSessionControlAction = new OnboardingAction(mainWindow);
	stopSessionControlAction->setActivePlugin("Session Control");

	stopSessionControlAction->setTitle("Stopping the experience");
	stopSessionControlAction->setInstructionsTitle("Do this now");
	stopSessionControlAction->setInstructionsDescription("<ul> <li>Stop the session by clicking on the Stop button below before continuing.</li> </ul>");
	stopSessionControlAction->getWindowPosition = [](QDockWidget* sessionControlWidget, int titleBarHeight, OnboardingAction*)
	{
		return QRect(10, sessionControlWidget->y() - 220 - titleBarHeight, 791, 200);
	};
	stopSessionControlAction->getArrowPosition = [](QDockWidget* sessionControlWidget, int titleBarHeight, OnboardingAction*)
	{
		return QRect(30, sessionControlWidget->y() - 20 - titleBarHeight,
			51, 30);
	};

	stopSessionControlAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);
	stopSessionControlAction->setTitlePosition(QRect(38, 11, 689, 45));
	stopSessionControlAction->setInstructionsTitlePosition(QRect(38, 70, 243, 47));
	stopSessionControlAction->setInstructionsPosition(QRect(38, 120, 680, 110));
	stopSessionControlAction->setPrevOnboardingAction(experienceWindowAction);
	experienceWindowAction->setNextOnboardingAction(stopSessionControlAction);
	mOnboardingActions.push_back(stopSessionControlAction);
	connect(stopSessionControlAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* fileHandlingAction = new OnboardingAction(mainWindow);
	fileHandlingAction->setTitle("Handling files");
	fileHandlingAction->setDescription("A bio-feedback application in neuromore Studio consists of two files:<br/><br/>"
			"	1. a <font color='#4EBCEB'>classifier</font> in which you process the signals from your bio-sensors and<br/>"
			"	2. a <font color='#4EBCEB'>state machine</font> for the application logic and the UI.<br/><br/>"
			"All files in the <i> examples </i>-folder "
			"are read-only. To edit them you can copy them to your personal folder "
			"by right-clicking on them. ");
	fileHandlingAction->setActivePlugin("Back-End File System");
	auto backendFileSystemWidget = fileHandlingAction->getDockWidget();

	if (nullptr == backendFileSystemWidget)
	{
		return false;
	}

	fileHandlingAction->getWindowPosition = [](QDockWidget * backendFileSystemWidget, int titleBarHeight, OnboardingAction*)
	{ return  QRect(backendFileSystemWidget->x() + backendFileSystemWidget->width() + 38,
		backendFileSystemWidget->y() + backendFileSystemWidget->height() / 4 - titleBarHeight, 711, 410);
	};
	fileHandlingAction->getArrowPosition = [](QDockWidget* backendFileSystemWidget, int titleBarHeight, OnboardingAction*)
	{
		return QRect(backendFileSystemWidget->x() + backendFileSystemWidget->width() + 10,
			backendFileSystemWidget->y() + backendFileSystemWidget->height() / 3 + 25 - titleBarHeight,
			30, 51);
	};

	fileHandlingAction->setArrowType(OnboardingAction::ARROWTYPE::LEFTARROW);
	fileHandlingAction->setTitlePosition(QRect(38, 44, 255, 50));
	fileHandlingAction->setDescriptionPosition(QRect(38, 112, 620, 250));
	fileHandlingAction->setInstructionsTitlePosition(QRect(38, 370, 249, 47));
	fileHandlingAction->setInstructionsPosition(QRect(38, 420, 680, 120));
	fileHandlingAction->setPrevOnboardingAction(stopSessionControlAction);
	stopSessionControlAction->setNextOnboardingAction(fileHandlingAction);
	mOnboardingActions.push_back(fileHandlingAction);

	OnboardingAction* classifierAction = new OnboardingAction(mainWindow);
	classifierAction->setTitle("Process the data in the classifier");
	classifierAction->setDescription("To control the brightness of the video we average the amplitude "
		"of the Alpha band and stream it into a custom feedback node called ”ScreenBrightness”.<br/><br/>" 
		"You can also control other applications, for example a Unity game or a music production program "
		"like Ableton using the OSC protocol. <br/><br/>For more information have a look at doc.neuromore.com.");
	classifierAction->setActivePlugin("Classifier");
	classifierAction->setInstructionsTitle("Do this now");
	classifierAction->setInstructionsDescription("<ul><li> Explore the graph by right-clicking and dragging in the classifier window </li>"
	  	"<li>Zoom by pressing alt/option and turning the mouse wheel</li></ul>");

	auto classifierWidget = classifierAction->getDockWidget();

	if (nullptr == classifierWidget)
	{
		return false;
	}

	auto classifierTabRect = classifierAction->getTabRect(classifierWidget);

	if (classifierTabRect.isEmpty())
	{
		return false;
	}

	classifierAction->getWindowPosition = [](QDockWidget* classifierWidget, int titleBarHeight, OnboardingAction* classifierAction)
	{	
		auto classifierTabRect = classifierAction->getTabRect(classifierWidget);
		return  QRect(10, classifierWidget->y() + 3 * classifierTabRect.height() / 2 + 29 - titleBarHeight,
			520, 580);
	};
	classifierAction->getArrowPosition = [](QDockWidget* classifierWidget, int titleBarHeight, OnboardingAction* classifierAction)
	{
		auto classifierTabRect = classifierAction->getTabRect(classifierWidget);
		return QRect(classifierWidget->x(),
			classifierWidget->y() + 3 * classifierTabRect.height() / 2 - titleBarHeight,
			51, 30);
	};

	classifierAction->setArrowType(OnboardingAction::ARROWTYPE::TOPARROW);
	classifierAction->setTitlePosition(QRect(38, 11, 350, 90));
	classifierAction->setDescriptionPosition(QRect(38, 110, 450, 280));
	classifierAction->setInstructionsTitlePosition(QRect(38, 400, 243, 47));
	classifierAction->setInstructionsPosition(QRect(38, 450, 450, 300));
	classifierAction->setPrevOnboardingAction(fileHandlingAction);
	fileHandlingAction->setNextOnboardingAction(classifierAction);
	mOnboardingActions.push_back(classifierAction);
	connect(classifierAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* stateMachineAction = new OnboardingAction(mainWindow);
	stateMachineAction->setTitle("Design the UX in the State Machine");
	stateMachineAction->setDescription("In the <font color='#4EBCEB'>state machine</font> you define the user flow "
		"and design the <font color='#4EBCEB'>user interface</font> of your exprience.<br/><br/>"
		"Every state machine contains of a start state, an end state and a couple of action states "
		"in between. You can set conditions on transitions, for example to stop playing the video once the timer is up.<br/><br/>"
		"You can also access the state of the classifier through <i> CustomFeedback </i> "
		"variables. In this example we measure how long the session is running in the "
		"classifier and check in the <i> TimerRunning </i> state if the session duration of the training is finished.");
	stateMachineAction->setActivePlugin("State Machine");
	stateMachineAction->setInstructionsTitle("Do this now");
	stateMachineAction->setInstructionsDescription("<ul><li>Right-click and drag to explore the state machine</li> <li> Double click on the 'Select Duration' node and explore the properties "
		"on the right of the <i>State Machine</i> window.</li> </ul>");

	auto stateMachineWidget = stateMachineAction->getDockWidget();

	if (nullptr == stateMachineWidget)
	{
		return false;
	}

	auto stateTabRect = stateMachineAction->getTabRect(stateMachineWidget);

	if (stateTabRect.isEmpty())
	{
		return false;
	}

	stateMachineAction->getWindowPosition = [](QDockWidget* stateMachineWidget, int titleBarHeight, OnboardingAction* stateMachineAction)
	{
		auto stateTabRect = stateMachineAction->getTabRect(stateMachineWidget);
	    return QRect(70, stateMachineWidget->y() + 3 * stateTabRect.height() / 2 + 29 - titleBarHeight, 520, 680);
	};
	stateMachineAction->getArrowPosition = [](QDockWidget* stateMachineWidget, int titleBarHeight, OnboardingAction* stateMachineAction)
	{
		auto stateTabRect = stateMachineAction->getTabRect(stateMachineWidget);
		return QRect(stateMachineWidget->x() + stateTabRect.x(),
			stateMachineWidget->y() + 3 * stateTabRect.height() / 2 - titleBarHeight,
			51, 30);
	};

	stateMachineAction->setArrowType(OnboardingAction::ARROWTYPE::TOPARROW);
	stateMachineAction->setTitlePosition(QRect(38, 11, 450, 90));
	stateMachineAction->setDescriptionPosition(QRect(38, 110, 450, 380));
	stateMachineAction->setInstructionsTitlePosition(QRect(38, 500, 450, 47));
	stateMachineAction->setInstructionsPosition(QRect(38, 550, 450, 100));
	stateMachineAction->setPrevOnboardingAction(classifierAction);
	classifierAction->setNextOnboardingAction(stateMachineAction);
	mOnboardingActions.push_back(stateMachineAction);
	connect(stateMachineAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* debugWindowAction = new OnboardingAction(mainWindow);
	debugWindowAction->setTitle("Investigate the Raw Signals");
	debugWindowAction->setDescription("On the bottom you can find different windows "
		"to debug the current application output. This is the Raw EEG window where "
		"you can see the activity for all the channels of the simulated EEG device.");
	debugWindowAction->setActivePlugin("Raw EEG");

	auto debugWindowWidget = debugWindowAction->getDockWidget();

	if (nullptr == debugWindowWidget)
	{
		return false;
	}

	debugWindowAction->getWindowPosition = [](QDockWidget* debugWindowWidget, int titleBarHeight, OnboardingAction*)
	{
		return  QRect(debugWindowWidget->x() - debugWindowWidget->width() / 34,
			debugWindowWidget->y() - 245 - titleBarHeight, 791, 215);
	};
	auto rawEEGViewTabRect = debugWindowAction->getTabRect(debugWindowWidget);
	if (rawEEGViewTabRect.isEmpty())
	{
		return false;
	}
	debugWindowAction->getArrowPosition = [](QDockWidget* debugWindowWidget, int titleBarHeight, OnboardingAction* debugWindowAction)
	{
		auto rawEEGViewTabRect = debugWindowAction->getTabRect(debugWindowWidget);
		return QRect(debugWindowWidget->x() + rawEEGViewTabRect.x(), debugWindowWidget->y() - 30 - titleBarHeight,
			51, 30);
	};

	debugWindowAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);
	debugWindowAction->setTitlePosition(QRect(34, 27, 689, 45));
	debugWindowAction->setDescriptionPosition(QRect(34, 88, 680, 99));
	debugWindowAction->setArrowPosition(OnboardingAction::ARROWTYPE::DOWNARROW, QRect(debugWindowWidget->x() + 131, debugWindowWidget->y() - 30 - titleBarHeight, 51, 30));
	debugWindowAction->setPrevOnboardingAction(stateMachineAction);
	stateMachineAction->setNextOnboardingAction(debugWindowAction);
	mOnboardingActions.push_back(debugWindowAction);
	connect(debugWindowAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* feedbackViewAction = new OnboardingAction(mainWindow);
	feedbackViewAction->setTitle("The feedback view");
	feedbackViewAction->setDescription("In the feedback view you can see the live feedback values of the custom "
		"variables defined in the classifier. Here you see how the ScreenBrightness "
		"changes continuously and that you can also control other parameters, e.g. the weather in a game.");
	feedbackViewAction->setActivePlugin("Feedback");

	auto feedbackViewWidget = feedbackViewAction->getDockWidget();

	if (nullptr == feedbackViewWidget)
	{
		return false;
	}

	feedbackViewAction->getWindowPosition = [](QDockWidget* feedbackViewWidget, int titleBarHeight, OnboardingAction *)
	{
		return  QRect(feedbackViewWidget->x() - feedbackViewWidget->width() / 9,
			feedbackViewWidget->y() - 240 - titleBarHeight, 795, 220);
	};

	auto feedbackViewTabRect = feedbackViewAction->getTabRect(feedbackViewWidget);
	if (feedbackViewTabRect.isEmpty())
	{
		return false;
	}
	feedbackViewAction->getArrowPosition = [](QDockWidget* feedbackViewWidget, int titleBarHeight, OnboardingAction* feedbackViewAction)
	{
		auto feedbackViewTabRect = feedbackViewAction->getTabRect(feedbackViewWidget);
		return QRect(feedbackViewWidget->x()  + feedbackViewTabRect.x(),
			feedbackViewWidget->y() - 20 - titleBarHeight, 51, 30);
	};

	feedbackViewAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);
	feedbackViewAction->setTitlePosition(QRect(37, 20, 689, 40));
	feedbackViewAction->setDescriptionPosition(QRect(37, 81, 750, 99));
	feedbackViewAction->setArrowPosition(OnboardingAction::ARROWTYPE::DOWNARROW, QRect(debugWindowWidget->x() + feedbackViewTabRect.x(),
																					   debugWindowWidget->y() - 30 - titleBarHeight, 51, 30));
	feedbackViewAction->setPrevOnboardingAction(debugWindowAction);
	debugWindowAction->setNextOnboardingAction(feedbackViewAction);
	mOnboardingActions.push_back(feedbackViewAction);
	connect(feedbackViewAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);


	OnboardingAction* customParametersAction = new OnboardingAction(mainWindow);
	customParametersAction->setTitle("Using Parameters");
	customParametersAction->setDescription("You can parameterize the experience and adjust those parameters "
		"either before or during a session. Here you find the 'Duration' parameter again "
		"that we previously set in the experience window after starting a session.");
	customParametersAction->setActivePlugin("Parameters");

	auto customParametersWidget = customParametersAction->getDockWidget();

	if (nullptr == customParametersWidget)
	{
		return false;
	}

	customParametersAction->getWindowPosition = [](QDockWidget* customParametersWidget, int titleBarHeight, OnboardingAction*)
	{
		return  QRect(0, customParametersWidget->y() - 267 - titleBarHeight, 850, 237);
	};

	auto customParametersTabRect = customParametersAction->getTabRect(customParametersWidget);

	if (customParametersTabRect.isEmpty())
	{
		return false;
	}

	customParametersAction->getArrowPosition = [](QDockWidget* customParametersWidget, int titleBarHeight, OnboardingAction* customParametersAction)
	{
		auto customParametersTabRect = customParametersAction->getTabRect(customParametersWidget);
		return QRect(customParametersTabRect.x(),
			customParametersWidget->y() - 30 - titleBarHeight, 51, 30);
	};
	customParametersAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);

	customParametersAction->setWindowPosition(QRect(0, sessionControlWidget->y() - 267 - titleBarHeight, 791, 237));
	customParametersAction->setTitlePosition(QRect(37, 20, 689, 45));
	customParametersAction->setDescriptionPosition(QRect(38, 80, 800, 99));
	customParametersAction->setPrevOnboardingAction(feedbackViewAction);
	feedbackViewAction->setNextOnboardingAction(customParametersAction);
	mOnboardingActions.push_back(customParametersAction);
	connect(customParametersAction, &OnboardingAction::ActivePluginChanged, appManager, &AppManager::SetPluginTabVisible);

	OnboardingAction* endTutorialAction = new OnboardingAction(mainWindow);
	endTutorialAction->setTitle("Starting a Visualization");
	endTutorialAction->setDescription("You can also use one of the pre-built visualizations. Those are little "
		"games in which you can contol the speed of a car, the weather, the volume "
		"or special camera effects using biofeedback. Those parameters are controlled through the OSC interface in the classifier." );
	endTutorialAction->setActivePlugin("Session Control");
	endTutorialAction->setInstructionsTitle("Do this now");
	endTutorialAction->setInstructionsDescription("<ul> <li> Click on the eye icon and start one of the games. "
		"It will open in another window</li> "
		"<li>Click on the 'go' button and observe the game changing in real time</li> "
		"<li>Congratulations! You've made it through the tutorial and are now ready to "
		"create your own applications. For more information go to <font color='#4EBCEB'>doc.neuromore.com</font></li> </ul>");

	endTutorialAction->getWindowPosition = [](QDockWidget* sessionControlWidget, int titleBarHeight, OnboardingAction*)
	{
		return QRect(75, sessionControlWidget->y() - 310 - titleBarHeight, 900, 380);
	};

	endTutorialAction->getArrowPosition = [](QDockWidget* sessionControlWidget, int titleBarHeight, OnboardingAction* )
	{
		return QRect(200, sessionControlWidget->y() + 40 - titleBarHeight,
			51, 30);
	};
	endTutorialAction->setArrowType(OnboardingAction::ARROWTYPE::DOWNARROW);
	endTutorialAction->setTitlePosition(QRect(38, 11, 689, 45));
	endTutorialAction->setDescriptionPosition(QRect(38, 60, 730, 100));
	endTutorialAction->setInstructionsTitlePosition(QRect(38, 170, 243, 47));
	endTutorialAction->setInstructionsPosition(QRect(38, 220, 850, 110));
	endTutorialAction->setPrevOnboardingAction(customParametersAction);
	customParametersAction->setNextOnboardingAction(endTutorialAction);
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
	QStringList itemPath = { "examples", "GettingStarted", "FocusTrainer"};
	backendFileSystemPlugin->ExpandByPath(itemPath);

	// loads the "FirstFocusTrainerClassifierTestSystem" classifier and "FirstFocusTrainerStateMachine" state machine for the tour.
	GetFileManager()->OpenClassifier(FileManager::LOCATION_BACKEND, "ef5cff5a-d569-4f5d-8e00-03049540ff42", "FirstFocusTrainerClassifierTestSystem", -1);
	GetFileManager()->OpenStateMachine(FileManager::LOCATION_BACKEND, "33d18723-73a4-42f5-9869-7bb3da0530c1", "FirstFocusTrainerStateMachine", -1);

	connect(mainWindow, &MainWindow::resized, this, &TourManager::OnResized);
	connect(mainWindow, &MainWindow::minimized, this, &TourManager::OnMinimized);
	connect(mainWindow, &MainWindow::maximized, this, &TourManager::OnMaximized);

	if (!setSignalViewPluginActive()) {
		return false;
	}

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

void TourManager::OnMinimized()
{
	TourManager::CurrentOnboardingAction->hide();
}

void TourManager::OnMaximized()
{
	TourManager::CurrentOnboardingAction->setVisible(true);
}

void TourManager::OnResized()
{
	TourManager::CurrentOnboardingAction->OnResized();
}

bool TourManager::setSignalViewPluginActive()
{
	auto mainWindow = GetMainWindow();
	const uint32 numRegisteredPlugins = GetQtBaseManager()->GetPluginManager()
		->GetNumActivePlugins();
	for (uint32 i = 0; i < numRegisteredPlugins; ++i)
	{
		Plugin* plugin = GetPluginManager()->GetActivePlugin(i);
		if (std::string(plugin->GetName()) == "Signal View")
		{
			QDockWidget* dockWidget = plugin->GetDockWidget();
			Q_FOREACH(QTabBar * tabBar, mainWindow->findChildren<QTabBar*>()) {
				for (int i = 0; i < tabBar->count(); ++i) {
					if (dockWidget == (QDockWidget*)tabBar->tabData(i).toULongLong())
					{
						tabBar->setCurrentIndex(i);
						return true;
					}
				}
			}
		}
	}
	return false;
}