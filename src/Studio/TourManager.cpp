#include "TourManager.h"

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


void TourManager::InitOnboardingActions()
{
	OnboardingAction* prevAction;
	OnboardingAction* nextAction;

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
	fileHandlingAction->setWindowPosition(QRect(352, 118, 720, 396));
	fileHandlingAction->setTitlePosition(QRect(38, 44, 249, 50));
	fileHandlingAction->setDescriptionPosition(QRect(38, 129, 620, 203));
	fileHandlingAction->setActivePlugin("Back-End File System");
	fileHandlingAction->setPrevOnboardingAction(welcomeAction);
	fileHandlingAction->setArrowPosition(OnboardingAction::ARROWTYPE::LEFTARROW, QRect(335, 230, 52, 36));
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
	editorAction->setWindowPosition(QRect(342, 88, 765, 330));
	editorAction->setTitlePosition(QRect(38, 11, 368, 47));
	editorAction->setDescriptionPosition(QRect(38, 72, 700, 203));
	editorAction->setActivePlugin("Classifier");
	editorAction->setPrevOnboardingAction(fileHandlingAction);
	editorAction->setArrowPosition(OnboardingAction::ARROWTYPE::TOPARROW, QRect(426, 67, 53 , 36));
	fileHandlingAction->setNextOnboardingAction(editorAction);
	mOnboardingActions.push_back(editorAction);
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