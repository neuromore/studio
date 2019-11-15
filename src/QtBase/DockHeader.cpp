/*
 * Qt Base
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

// include the required headers
#include "DockHeader.h"
#include "QtBaseManager.h"
#include <QHBoxLayout>
#include <QPainter>


void DockHeaderFillWidget::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	int width = QWidget::width();
	int height = QWidget::height();

	painter.setPen( QColor(74,74,73) );

	int delta = 5;
	int padding = 3;
	for (int i=0; i<width; i+=delta)
	{
		painter.drawLine( i, padding, i, height-padding );
	}
}


// the constructor
DockHeader::DockHeader(QDockWidget* dockWidget) : QWidget()
{
	setObjectName("WindowHeaderWidget");

	mDockWidget = dockWidget;
	mDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);


	QHBoxLayout* mainLayout = new QHBoxLayout();
	QWidget* innerWidget = new QWidget();
	innerWidget->setObjectName("WindowHeaderWidget");
	mainLayout->addWidget( innerWidget );
	mainLayout->setMargin( 0 );
	mainLayout->setSpacing( 0 );
	mainLayout->setSizeConstraint( QLayout::SetNoConstraint );
	setLayout( mainLayout );

	QHBoxLayout* layout = new QHBoxLayout();
	layout->setMargin( 3 );
	layout->setSpacing( 0 );
	layout->setSizeConstraint( QLayout::SetNoConstraint );
	innerWidget->setLayout( layout );

	int spacing = 3;

    // spacer widget
	QWidget* begSpacingWidget = new QWidget(this);
	begSpacingWidget->setMinimumWidth(spacing);
	begSpacingWidget->setMaximumWidth(spacing);
	layout->addWidget( begSpacingWidget );
    
    mTitleLabel = new QLabel();
    mTitleLabel->setObjectName("WindowTitle");
    mTitleLabel->setText( mDockWidget->windowTitle() );
    mTitleLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Minimum );
    layout->addWidget( mTitleLabel );

    // spacer widget
	begSpacingWidget = new QWidget(this);
	begSpacingWidget->setMinimumWidth(spacing);
	begSpacingWidget->setMaximumWidth(spacing);
	layout->addWidget( begSpacingWidget );

    // spacer widget
	DockHeaderFillWidget* spacerWidget = new DockHeaderFillWidget(this);
	//spacerWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);
	layout->addWidget( spacerWidget );

	// add the minimize button
	mMinimizeButton = new QPushButton();
	mMinimizeButton->setObjectName("WindowMinimizeIcon");
	mMinimizeButton->setToolTip( "Minimize" );
	mMinimizeButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	mMinimizeButton->setMaximumWidth(22);
	connect(mMinimizeButton, SIGNAL(clicked()), this, SLOT(OnMaximizeButton()));
	layout->addWidget( mMinimizeButton );

	// add the maximize button
	mMaximizeButton = new QPushButton();
	mMaximizeButton->setObjectName("WindowMaximizeIcon");
	mMaximizeButton->setToolTip( "Maximize" );
	mMaximizeButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	mMaximizeButton->setMaximumWidth(22);
	connect(mMaximizeButton, SIGNAL(clicked()), this, SLOT(OnMaximizeButton()));
	layout->addWidget( mMaximizeButton );

	// add the dock button
	mDockButton = new QPushButton();
	mDockButton->setObjectName("WindowDockIcon");
	mDockButton->setToolTip( "Dock window and put it back into the interface" );
	mDockButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	mDockButton->setMaximumWidth(22);
	connect(mDockButton, SIGNAL(clicked()), this, SLOT(OnDockButton()));
	layout->addWidget( mDockButton );

	// add the undock button
	mUndockButton = new QPushButton();
	mUndockButton->setObjectName("WindowUndockIcon");
	mUndockButton->setToolTip( "Undock window" );
	mUndockButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	mUndockButton->setMaximumWidth(22);
	connect(mUndockButton, SIGNAL(clicked()), this, SLOT(OnDockButton()));
	layout->addWidget( mUndockButton );

	mCloseButton = new QPushButton();
	mCloseButton->setObjectName("WindowCloseIcon");
	mCloseButton->setToolTip( "Close" );
	mCloseButton->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
	mCloseButton->setMaximumWidth(22);
	connect(mCloseButton, SIGNAL(clicked()), this, SLOT(OnCloseButton()));
	layout->addWidget( mCloseButton );
    
	connect(mDockWidget, SIGNAL(topLevelChanged(bool)), this, SLOT(OnTopLevelChanged(bool)));
	OnUpdate();
}


// destructor
DockHeader::~DockHeader()
{
}


void DockHeader::OnDockButton()
{
	mDockWidget->setFloating( !mDockWidget->isFloating() );
	OnUpdate();
}


void DockHeader::OnCloseButton()
{
	mDockWidget->close();
	emit RemovePlugin();
}


void DockHeader::OnMaximizeButton()
{
	if (mDockWidget->isFloating() == true)
	{
#ifdef NEUROMORE_PLATFORM_OSX
		if (mDockWidget->windowState() & Qt::WindowFullScreen)
            mDockWidget->setWindowState( Qt::WindowNoState );
		else
			mDockWidget->setWindowState( Qt::WindowFullScreen );
#endif
#ifdef NEUROMORE_PLATFORM_WINDOWS
        if (mDockWidget->windowState() & Qt::WindowMaximized)
            mDockWidget->setWindowState( Qt::WindowNoState );
        else
            mDockWidget->setWindowState( Qt::WindowMaximized );
#endif
	}
	else
	{
		OnDockButton();
	}

	OnUpdate();
}


void DockHeader::OnUpdate()
{
	mCloseButton->setHidden( (mDockWidget->features() & QDockWidget::DockWidgetClosable) == false );

	if (mDockWidget->isFloating() == true)
	{
#ifdef NEUROMORE_PLATFORM_OSX
		if (mDockWidget->windowState() & Qt::WindowFullScreen)
        {
            //mMaximizeButton->setHidden( !mDockWidget->isFloating() );
            mMinimizeButton->setVisible(true);
            mMaximizeButton->setVisible(false);
        }
        else
        {
            mMaximizeButton->setVisible(true);
            mMinimizeButton->setVisible(false);
        }
#endif
#ifdef NEUROMORE_PLATFORM_WINDOWS
        if (mDockWidget->windowState() & Qt::WindowMaximized)
        {
            //mMaximizeButton->setHidden( !mDockWidget->isFloating() );
            mMinimizeButton->setVisible(true);
            mMaximizeButton->setVisible(false);
        }
        else
        {
            mMaximizeButton->setVisible(true);
            mMinimizeButton->setVisible(false);
        }
#endif

		mDockButton->setVisible( true );
		mUndockButton->setVisible( false );
	}
	else
	{
		mDockButton->setVisible(false);
		mMaximizeButton->setVisible(false);
		mMinimizeButton->setVisible(false);

		mUndockButton->setVisible( true );
	}
}


void DockHeader::OnTopLevelChanged(bool isFloating)
{
	if (isFloating)
	{		
		mDockWidget->raise();
		mDockWidget->setFocus();
		//mDockWidget->grabKeyboard();

		// setWindowFlags calls setParent() when changing the flags for a window, causing the widget to be hidden.
		// you must call show() to make the widget visible again
		//mDockWidget->setWindowFlags(Qt::Window);
		//mDockWidget->show();
	}
	else
	{
		mDockWidget->raise();
		mDockWidget->setFocus();
	}

	OnUpdate();
}