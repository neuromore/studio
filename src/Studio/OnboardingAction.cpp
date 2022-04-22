#include "OnboardingAction.h"
#include "MainWindow.h"
#include <QTextDocument>
#include <QTextOption>
#include <QAbstractTextDocumentLayout>

OnboardingAction::OnboardingAction(QWidget* parent) :
	QWidget(parent)
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_MacAlwaysShowToolWindow, true);
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
}

OnboardingAction::~OnboardingAction()
{
	delete mCloseBtn;
	delete mPreviousBtn;
	delete mNextBtn;
	delete mEndBtn;
}

void OnboardingAction::setNextOnboardingAction(OnboardingAction* nextOnboardingAction)
{
	mNextOnboardingAction = nextOnboardingAction;
}

void OnboardingAction::setPrevOnboardingAction(OnboardingAction* prevOnboardingAction)
{
	mPrevOnboardingAction = prevOnboardingAction;
}

void OnboardingAction::setTitle(const QString& title)
{
	mTitle = title;
}

void OnboardingAction::setDescription(const QString& description)
{
	mDescription = description;
}

void OnboardingAction::setInstructionsTitle(const QString& instructionsTitle)
{
	mInstructionsTitle = instructionsTitle;
}

void OnboardingAction::setInstructionsDescription(const QString& instructionsDescription)
{
	mInstructionsDescription = instructionsDescription;
}

void OnboardingAction::setActiveRegion(const QRect& activeRegion)
{
	mActiveRegion = activeRegion;
}

void OnboardingAction::setActivePlugin(std::string pluginName)
{
	const uint32 numRegisteredPlugins = GetQtBaseManager()->GetPluginManager()
		->GetNumActivePlugins();
	for (uint32 i = 0; i < numRegisteredPlugins; ++i)
	{
		Plugin* plugin = GetPluginManager()->GetActivePlugin(i);
		if (std::string(plugin->GetName()) == pluginName)
		{
			mActivePluginIdx = i;
		}
	}
}

void OnboardingAction::setMainRegion(const QRect& mainRegion)
{
	mMainRegion = mainRegion;
}

void OnboardingAction::setArrowPosition(ARROWTYPE arrowType, const QRect& arrowPosition)
{
	mArrowType = arrowType;
	mArrowPosition = arrowPosition;
}

void OnboardingAction::setArrowType(ARROWTYPE arrowType)
{
	mArrowType = arrowType;
}

void OnboardingAction::setDescriptionPosition(const QRect& rect)
{
	mDescriptionRect = rect;
}

void OnboardingAction::setTitlePosition(const QRect& rect)
{
	mTitleRect = rect;
}

void OnboardingAction::setInstructionsTitlePosition(const QRect& rect)
{
	mInstructionsTitleRect = rect;
}

void OnboardingAction::setInstructionsPosition(const QRect& rect)
{
	mInstructionsRect = rect;
}

void OnboardingAction::CreateButtons()
{
	mCloseBtn = new QToolButton(this);
	mCloseBtn->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/Close.png"));
	mCloseBtn->setStyleSheet(QString("background: transparent;"
		"border: 0px;"));
	mCloseBtn->setIconSize(QSize(29, 29));

	mPreviousBtn = new QToolButton(this);
	mPreviousBtn->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/LeftArrowBlue.png"));
	mPreviousBtn->setStyleSheet(QString("background: transparent;"
		"border: 0px;"));
	mPreviousBtn->setIconSize(QSize(29, 29));

	mNextBtn = new QToolButton(this);
	mNextBtn->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/RightArrowBlue.png"));
	mNextBtn->setStyleSheet(QString("background: transparent;"
		"border: 0px;"));
	mNextBtn->setIconSize(QSize(29, 29));

	mEndBtn = new QToolButton(this);
	mEndBtn->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/EndTutorial.png"));
	mEndBtn->setStyleSheet(QString("background: transparent;"
		"border: 0px;"));
	mEndBtn->setIconSize(QSize(124, 35));
	mEndBtn->setVisible(false);

	auto appManager = GetManager();
	connect(mCloseBtn, &QToolButton::pressed, appManager, &AppManager::CloseTour);
	connect(mCloseBtn, SIGNAL(pressed()), this, SLOT(OnCloseAction()));

	connect(mEndBtn, &QToolButton::pressed, appManager, &AppManager::CloseTour);
	connect(mNextBtn, SIGNAL(pressed()), this, SLOT(OnGoToNextAction()));
	connect(mPreviousBtn, SIGNAL(pressed()), this, SLOT(OnGoToPreviousAction()));
}

void OnboardingAction::ShowButtons()
{
	mCloseBtn->show();
	if (mPrevOnboardingAction == nullptr)
	{
		mPreviousBtn->setVisible(false);
	}
	if (mNextOnboardingAction == nullptr)
	{
		mNextBtn->setVisible(false);
		mEndBtn->setVisible(true);
	}
}

void OnboardingAction::SetButtonsGeometry()
{
	mCloseBtn->setGeometry(mWindowPosition.x() + mWindowPosition.width() - 44,
		mWindowPosition.y() + 15, 29, 29);

	mPreviousBtn->setGeometry(mWindowPosition.x() + 15,
		mWindowPosition.y() + mWindowPosition.height() - 44,
		29, 29);

	mNextBtn->setGeometry(mWindowPosition.x() + mWindowPosition.width() - 44,
		mWindowPosition.y() + mWindowPosition.height() - 44,
		29, 29);

	mEndBtn->setGeometry(mWindowPosition.x() + mWindowPosition.width() - 139,
		mWindowPosition.y() + mWindowPosition.height() - 50,
		124, 35);
}

void OnboardingAction::Invoke()
{
	emit ActivePluginChanged(mActivePluginIdx);

	SetWindowGeometry();

	auto mainWindow = GetQtBaseManager()->GetMainWindow();

	if (mActivePluginIdx >= 0)
	{
		auto activePlugin = GetQtBaseManager()->GetPluginManager()
		->GetActivePlugin(mActivePluginIdx);
		activePlugin->SetLocked(true);
	}

	if (mCloseBtn == nullptr)
	{
		CreateButtons();
	}

	SetButtonsGeometry();

	show();
}

void OnboardingAction::SetWindowGeometry()
{
	auto mainWindow = GetMainWindow();
	int frameHeight = mainWindow->geometry().y() - mainWindow->y();

#if defined(NEUROMORE_PLATFORM_OSX)

		if (mainWindow->windowState() != Qt::WindowFullScreen) {
			setGeometry(QRect(mainWindow->x(), mainWindow->y() + frameHeight, mainWindow->geometry().width(),
				mainWindow->geometry().height()));
		} else {
			setGeometry(QRect(mainWindow->x(), mainWindow->y(), mainWindow->geometry().width(),
				mainWindow->geometry().height()));
		}
		setMainRegion(QRect(0, 0, mainWindow->geometry().width(),
			mainWindow->geometry().height()));
#else
	setMainRegion(QRect(0, frameHeight, mainWindow->frameGeometry().width(),
		mainWindow->frameGeometry().height()));
	setGeometry(QRect(mainWindow->x(), mainWindow->y(), mainWindow->frameGeometry().width(),
		mainWindow->frameGeometry().height()));
#endif

	if (mActivePluginIdx == 0)
	{
		QDockWidget* debugWindowWidget = getDockWidget();

		if (nullptr == debugWindowWidget)
		{
			return;
		}

#if defined(NEUROMORE_PLATFORM_OSX)
		setActiveRegion(QRect(debugWindowWidget->x(), debugWindowWidget->y() - 25,
			debugWindowWidget->width(), 2 * debugWindowWidget->height()));
#else
		setActiveRegion(QRect(debugWindowWidget->x(), debugWindowWidget->y(),
			debugWindowWidget->width(), 2 * debugWindowWidget->height()));
#endif
	}

	else if (mActivePluginIdx > 0)
	{
		auto activePlugin = GetQtBaseManager()->GetPluginManager()
			->GetActivePlugin(mActivePluginIdx);
		setActiveRegion(activePlugin->GetGeometry());
	}

	int titleBarHeight = 0;
#if defined(NEUROMORE_PLATFORM_OSX)
	QWidget* menuWidget = GetMainWindow()->menuWidget();
	if (nullptr != menuWidget) {
		titleBarHeight = menuWidget->height() + 20;
	}
#endif

	auto dockWidget = getDockWidget();

	mWindowPosition = getWindowPosition(dockWidget, titleBarHeight, this);

	if (nullptr == dockWidget)
	{
		return;
	}

	if (mArrowType != OnboardingAction::NOARROW)
	{
		mArrowPosition = getArrowPosition(dockWidget, titleBarHeight, this);
	}
}

void OnboardingAction::OnResized()
{
	this->raise();
	SetWindowGeometry();
	SetButtonsGeometry();
	repaint();
}

void OnboardingAction::setWindowPosition(const QRect& windowPosition)
{
	mWindowPosition = windowPosition;
}

void OnboardingAction::paintEvent(QPaintEvent*)
{
	auto mainWindow = GetMainWindow();

	QPainter painter(this);
	painter.setPen(QPen(QColor(0, 0, 0)));
	painter.fillRect(mMainRegion, QBrush(QColor(0, 0, 0, 128)));

	QRegion overlayRegion = QRegion(mMainRegion) - mActiveRegion
		+ mWindowPosition + mArrowPosition;
	setMask(overlayRegion);

	QPixmap activeRegionPixmap(mActiveRegion.width(), mActiveRegion.height());
	painter.setCompositionMode(QPainter::CompositionMode_Clear);
	painter.drawPixmap(mActiveRegion.x(), mActiveRegion.y(), activeRegionPixmap);
	painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

	QIcon arrowIcon;
	switch (mArrowType)
	{
	case OnboardingAction::LEFTARROW:
	{
		arrowIcon = GetQtBaseManager()->FindIcon("/Images/Icons/LeftRectArrow.png");
		break;
	}
	case OnboardingAction::RIGHTARROW:
	{
		arrowIcon = GetQtBaseManager()->FindIcon("/Images/Icons/RightRectArrow.png");
		break;
	}
	case OnboardingAction::TOPARROW:
	{
		arrowIcon = GetQtBaseManager()->FindIcon("/Images/Icons/TopRectArrow.png");
		break;
	}
	case OnboardingAction::DOWNARROW:
	{
		arrowIcon = GetQtBaseManager()->FindIcon("/Images/Icons/DownRectArrow.png");
		break;
	}
	default:
		break;
	}

	if (!arrowIcon.isNull()) {
		QPixmap pixmap = arrowIcon.pixmap(QSize(mArrowPosition.width(), mArrowPosition.height()));
		painter.drawPixmap(mArrowPosition.x(), mArrowPosition.y(), pixmap);
	}

	QPainterPath path;
	QRectF messageBox(mWindowPosition);
	path.addRoundedRect(mWindowPosition, 25, 25);
	painter.fillPath(path, QColor("#E5E5E5"));

	QFont titleFont;
	titleFont.setPixelSize(35);
#if defined(__linux__)
	titleFont.setStretch(85);
#endif
	titleFont.setWeight(QFont::Bold);
	titleFont.setFamily("Roboto");
	painter.setFont(titleFont);
	painter.drawText(QRectF(messageBox.x() + mTitleRect.x(),
		messageBox.y() + mTitleRect.y(),
		mTitleRect.width(),
		mTitleRect.height()),
		mTitle);

	QTextDocument descriptionText;
	QFont descriptionFont;
	descriptionFont.setPixelSize(19);
#if defined(__linux__)
	descriptionFont.setStretch(85);
#endif
	descriptionFont.setWeight(QFont::DemiBold);
	descriptionFont.setFamily("Roboto");
	descriptionText.setTextWidth(mDescriptionRect.width());
	descriptionText.setDefaultFont(descriptionFont);
	descriptionText.setHtml(mDescription);
	QRect descriptionRect(0, 0, mDescriptionRect.width(),
		mDescriptionRect.height());
	painter.translate(messageBox.x() + mDescriptionRect.x(),
		messageBox.y() + mDescriptionRect.y());

	QAbstractTextDocumentLayout::PaintContext ctx;
	ctx.palette.setColor(QPalette::Text, painter.pen().color());
	descriptionText.documentLayout()->draw(&painter, ctx);

	QFont instructionTitleFont;
	instructionTitleFont.setPixelSize(30);
#if defined(__linux__)
	instructionTitleFont.setStretch(85);
#endif
	instructionTitleFont.setWeight(QFont::Bold);
	instructionTitleFont.setFamily("Roboto");
	painter.setFont(instructionTitleFont);
	painter.translate(-messageBox.x() - mDescriptionRect.x(),
		-messageBox.y() - mDescriptionRect.y());
	painter.drawText(QRectF(messageBox.x() + mInstructionsTitleRect.x(),
		messageBox.y() + mInstructionsTitleRect.y(),
		mInstructionsTitleRect.width(),
		mInstructionsTitleRect.height()),
		mInstructionsTitle);

	QTextDocument instructionsText;
	QFont instructionFont;
	instructionFont.setPixelSize(19);
#if defined(__linux__)
	instructionFont.setStretch(85);
#endif
	instructionFont.setWeight(QFont::DemiBold);
	instructionFont.setFamily("Roboto");
	instructionsText.setTextWidth(mInstructionsRect.width());
	instructionsText.setDefaultFont(instructionFont);
	instructionsText.setHtml(mInstructionsDescription);
	QRect instructionsRect(0, 0, mInstructionsRect.width(),
		mInstructionsRect.height());
	painter.translate(messageBox.x() + mInstructionsRect.x(),
		messageBox.y() + mInstructionsRect.y());

	instructionsText.documentLayout()->draw(&painter, ctx);

	ShowButtons();
}

QDockWidget* OnboardingAction::getDockWidget() const
{
	if (mActivePluginIdx < 0)
	{
		return nullptr;
	}

	Plugin* plugin = GetQtBaseManager()->GetPluginManager()->GetActivePlugin(mActivePluginIdx);

	if (nullptr != plugin)
	{
		return plugin->GetDockWidget();
	}

	return nullptr;
}

QRect OnboardingAction::getTabRect(const QDockWidget* DWidget) const
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

void OnboardingAction::OnCloseAction()
{
	if (mActivePluginIdx >= 0)
	{
		auto activePlugin = GetQtBaseManager()->GetPluginManager()
			->GetActivePlugin(mActivePluginIdx);
		activePlugin->SetLocked(false);
	}
	this->hide();
}

void OnboardingAction::OnGoToPreviousAction()
{
	OnCloseAction();
	if (mPrevOnboardingAction)
	{
		mPrevOnboardingAction->Invoke();
		TourManager::CurrentOnboardingAction = mPrevOnboardingAction;
	}
}

void OnboardingAction::OnGoToNextAction()
{
	OnCloseAction();
	if (mNextOnboardingAction)
	{
		mNextOnboardingAction->Invoke();
		TourManager::CurrentOnboardingAction = mNextOnboardingAction;
	}
}