#ifndef __NEUROMORE_TOURMANAGER_H
#define __NEUROMORE_TOURMANAGER_H

// include required headers
#include <QtBaseManager.h>
#include <PluginSystem/PluginManager.h>
#include "AppManager.h"
#include "OnboardingAction.h"

// include Qt
#include <QObject>
#include <QWidget>

#include <iostream>
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

	static OnboardingAction* CurrentOnboardingAction;

	QRect getTabRect(const QDockWidget* DWidget) const;

public slots: 

	void OnWindowClosed();

private:

	std::vector<OnboardingAction*>	mOnboardingActions;
};

#endif
