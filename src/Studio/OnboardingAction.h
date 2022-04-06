#ifndef __NEUROMORE_ONBOARDINGACTION_H
#define __NEUROMORE_ONBOARDINGACTION_H

#include "Config.h"
#include <QtBaseManager.h>
#include <PluginSystem/PluginManager.h>
#include "TourManager.h"
#include "MainWindow.h"

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

	void setNextOnboardingAction(OnboardingAction* nextOnboardingAction);
	void setPrevOnboardingAction(OnboardingAction* prevOnboardingAction);
	void setTitle(const QString& title);
	void setDescription(const QString& description);
	void setActiveRegion(const QRegion& activeRegion);
	void setActivePlugin(std::string pluginName);
	void setMainRegion(const QRect& mainRegion);
	void setWindowPosition(const QRect& windowPosition);
	void setArrowPosition(ARROWTYPE arrowtype, const QRect& arrowPosition = QRect());
	void setDescriptionPosition(const QRect& rect);
	void setTitlePosition(const QRect& rect);
	void ShowButtons();
	void CreateButtons();
	void Invoke();
	void paintEvent(QPaintEvent*) override;

	QDockWidget* getDockWidget() const;
	QRect getTabRect(const QDockWidget* dockWidget) const;

public slots:
	void OnCloseAction();
	void OnGoToPreviousAction();
	void OnGoToNextAction();

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
	QRegion mActiveRegion = QRect(0, 0, 0, 0);
	QRect mWindowPosition = QRect(800, 350, 350, 200);
	QRect mDescriptionRect;
	QRect mTitleRect;
	QRect mArrowPosition;
	ARROWTYPE mArrowType = ARROWTYPE::NOARROW;
	int mActivePluginIdx = -1;
	QString mTitle;
	QString mDescription;
};

#endif