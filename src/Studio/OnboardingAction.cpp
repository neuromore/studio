#include "OnboardingAction.h"

OnboardingAction::OnboardingAction(QWidget* parent) :
	QWidget(parent)
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
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

void OnboardingAction::setActiveRegion(const QRegion& activeRegion)
{
	mActiveRegion = activeRegion;
}

void OnboardingAction::setActivePlugin(std::string pluginName)
{
	const uint32 numRegisteredPlugins = GetQtBaseManager()->GetPluginManager()
		->GetNumActivePlugins();
	for (uint32 i = 1; i < numRegisteredPlugins; ++i)
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

void OnboardingAction::setWindowPosition(const QRect& windowPosition)
{
	mWindowPosition = windowPosition;
}

void OnboardingAction::setArrowPosition(ARROWTYPE arrowType, const QRect& arrowPosition)
{
	mArrowType = arrowType;
	mArrowPosition = arrowPosition;
}

void OnboardingAction::setDescriptionPosition(const QRect& rect)
{
	mDescriptionRect = rect;
}

void OnboardingAction::setImagePosition(const QRect& rect)
{
	mImagePosition = rect;
}

void OnboardingAction::setTitlePosition(const QRect& rect)
{
	mTitleRect = rect;
}

void OnboardingAction::CreateButtons()
{
	mCloseBtn = new QToolButton(this);
	mCloseBtn->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/Close.png"));
	mCloseBtn->setStyleSheet(QString("border: 0px"));
	mCloseBtn->setIconSize(QSize(29, 29));
	mCloseBtn->setGeometry(mWindowPosition.x() + mWindowPosition.width() - 44,
						   mWindowPosition.y() + 15,
						   29, 29);

	mPreviousBtn = new QToolButton(this);
	mPreviousBtn->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/LeftArrowGray.png"));
	mPreviousBtn->setStyleSheet(QString("border: 1px"));
	mPreviousBtn->setIconSize(QSize(29, 29));
	mPreviousBtn->setGeometry(mWindowPosition.x() + 15,
						      mWindowPosition.y() + mWindowPosition.height() - 44,
						      29, 29);

	mNextBtn = new QToolButton(this);
	mNextBtn->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/RightArrowGray.png"));
	mNextBtn->setStyleSheet(QString("border: none"));
	mNextBtn->setIconSize(QSize(29, 29));
	mNextBtn->setGeometry(mWindowPosition.x() + mWindowPosition.width() - 44,
						  mWindowPosition.y() + mWindowPosition.height() - 44,
					      29, 29);

	mEndBtn = new QToolButton(this);
	mEndBtn->setIcon(GetQtBaseManager()->FindIcon("/Images/Icons/EndTutorial.png"));
	mEndBtn->setStyleSheet(QString("border: none"));
	mEndBtn->setIconSize(QSize(124, 35));
	mEndBtn->setGeometry(mWindowPosition.x() + mWindowPosition.width() - 139,
						  mWindowPosition.y() + mWindowPosition.height() - 50,
					      124, 35);
	mEndBtn->setVisible(false);

	connect(mCloseBtn, SIGNAL(pressed()), this, SLOT(OnCloseAction()));
	connect(mEndBtn, SIGNAL(pressed()), this, SLOT(OnCloseAction()));
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

void OnboardingAction::Invoke()
{
	emit ActivePluginChanged(mActivePluginIdx);

	auto mainWindow = GetQtBaseManager()->GetMainWindow();
    setMainRegion(QRect(0, 0, mainWindow->frameGeometry().width(),
		mainWindow->frameGeometry().height()));

	setGeometry(QRect(mainWindow->x(), mainWindow->y(), mainWindow->frameGeometry().width(),
		mainWindow->frameGeometry().height()));

	const uint32 numRegisteredPlugins = GetQtBaseManager()->GetPluginManager()->GetNumActivePlugins();
	if (mActivePluginIdx)
	{
		auto activePlugin = GetQtBaseManager()->GetPluginManager()
			->GetActivePlugin(mActivePluginIdx);
		activePlugin->EnableTitleBarButtons(false);
		setActiveRegion(activePlugin->GetGeometry());
	}

	if (mCloseBtn == nullptr)
	{
		CreateButtons();
	}

	show();
}

void OnboardingAction::paintEvent(QPaintEvent*) 
{
	QPainter painter(this);
	painter.setPen(QPen(QColor(0, 0, 0)));
	painter.fillRect(mMainRegion, QBrush(QColor(0, 0, 0, 128)));

	QPainterPath path;
	QRectF messageBox(mWindowPosition);
	path.addRoundedRect(mWindowPosition, 25, 25);
	painter.fillPath(path, QColor("#E5E5E5"));

	QFont titleFont;
	titleFont.setPixelSize(35);
	titleFont.setStretch(85);
	titleFont.setWeight(QFont::Bold);
	titleFont.setFamily("Roboto");
	painter.setFont(titleFont);
	painter.drawText(QRectF(messageBox.x() + mTitleRect.x(),
							messageBox.y() + mTitleRect.y(),
							mTitleRect.width(),
							mTitleRect.height()),
							mTitle);

	QFont descriptionFont;
	descriptionFont.setPixelSize(19);
	descriptionFont.setStretch(85);
	descriptionFont.setWeight(QFont::DemiBold);
	descriptionFont.setFamily("Roboto");
	painter.setFont(descriptionFont);
	painter.drawText(QRect(messageBox.x() + mDescriptionRect.x(),
							messageBox.y() + mDescriptionRect.y(),
							mDescriptionRect.width(),
							mDescriptionRect.height()),
							mDescription);

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

	ShowButtons();

	QRegion overlayRegion = QRegion(mMainRegion) - mActiveRegion
		+ mWindowPosition + mArrowPosition;
	setMask(overlayRegion);
}

void OnboardingAction::OnCloseAction()
{
	auto activePlugin = GetQtBaseManager()->GetPluginManager()
		->GetActivePlugin(mActivePluginIdx);
	activePlugin->EnableTitleBarButtons(true);
	this->hide();
	mEndBtn->setVisible(false);
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