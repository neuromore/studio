#ifndef __NEUROMORE_TOURMANAGER_H
#define __NEUROMORE_TOURMANAGER_H

// include Qt
#include <QObject>

#include <vector>

class OnboardingAction;
class TourManager : public QObject
{
	Q_OBJECT
public:
	TourManager();
	~TourManager();

	bool InitOnboardingActions();

	void startTour();

	bool setSignalViewPluginActive();

	static OnboardingAction* CurrentOnboardingAction;

public slots: 

	void OnWindowClosed();
	void OnResized();
	void OnMinimized();
	void OnMaximized();

private:

	std::vector<OnboardingAction*>	mOnboardingActions;
};

#endif
