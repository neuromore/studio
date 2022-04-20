#ifndef __NEUROMORE_ONBOARDINGACTION_H
#define __NEUROMORE_ONBOARDINGACTION_H

#include "Config.h"
#include <QtBaseManager.h>
#include <PluginSystem/PluginManager.h>
#include "TourManager.h"

#include <QObject>
#include <QString>
#include <QWidget>
#include <QPainter>
#include <QRegion>
#include <QLabel>

#include <iostream>
#include <vector>

class OnboardingAction : public QWidget
{
	Q_OBJECT
public:
	enum ARROWTYPE {
		NOARROW = 0,
		LEFTARROW = 1,
		RIGHTARROW = 2,
		TOPARROW = 3,
		DOWNARROW = 4
	};

	OnboardingAction(QWidget* parent = nullptr);
	~OnboardingAction();

	void setWindowPosition(const QRect& windowPosition);
	void setNextOnboardingAction(OnboardingAction* nextOnboardingAction);
	void setPrevOnboardingAction(OnboardingAction* prevOnboardingAction);
	void setTitle(const QString& title);
	void setDescription(const QString& description);
	void setInstructionsTitle(const QString& instructionsTitle);
	void setInstructionsDescription(const QString& instructionsDescription);
	void setActiveRegion(const QRect& activeRegion);
	void setActivePlugin(std::string pluginName);
	void setMainRegion(const QRect& mainRegion);
	void setArrowPosition(ARROWTYPE arrowtype, const QRect& arrowPosition = QRect());
	void setArrowType(ARROWTYPE arrowType);
	void setDescriptionPosition(const QRect& rect);
	void setTitlePosition(const QRect& rect);
	void setInstructionsTitlePosition(const QRect& rect);
	void setInstructionsPosition(const QRect& rect);
	void ShowButtons();
	void CreateButtons();
	void SetButtonsGeometry();
	void Invoke();
	void paintEvent(QPaintEvent*) override;

	QDockWidget* getDockWidget() const;
	QRect getTabRect(const QDockWidget* dockWidget) const;

	QRect (*getWindowPosition) (QDockWidget* dockWidget, int titleBarHeight, OnboardingAction* currentAction);
	QRect (*getArrowPosition) (QDockWidget* dockWidget, int titleBarHeight, OnboardingAction* currentAction);
public slots:
	void OnCloseAction();
	void OnGoToPreviousAction();
	void OnGoToNextAction();
	void SetWindowGeometry();
	void OnResized();
	void OnMinimized();
	void OnMaximized();

signals:
	void ActivePluginChanged(int activePluginIdx);

private:
	OnboardingAction* mNextOnboardingAction = nullptr;
	OnboardingAction* mPrevOnboardingAction = nullptr;
	QToolButton* mCloseBtn = nullptr;
	QToolButton* mPreviousBtn = nullptr;
	QToolButton* mNextBtn = nullptr;
	QToolButton* mEndBtn = nullptr;
	QRect mMainRegion;
	QRect mActiveRegion = QRect(0, 0, 0, 0);
	QRect mWindowPosition = QRect(800, 350, 350, 200);
	QRect mDescriptionRect;
	QRect mTitleRect;
	QRect mInstructionsRect;
	QRect mInstructionsTitleRect;
	QRect mArrowPosition;
	ARROWTYPE mArrowType = ARROWTYPE::NOARROW;
	int mActivePluginIdx = -1;
	QString mTitle;
	QString mDescription;
	QString mInstructionsTitle;
	QString mInstructionsDescription;
};

#endif