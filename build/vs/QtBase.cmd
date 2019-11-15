setlocal
set MOC=%MOC%
set RCC=%RCC%
set DIR=../../src/QtBase

%RCC% --name Assets --output %DIR%/Resources/Assets.rcc.cpp %DIR%/Resources/Assets.qrc
%RCC% --name DeviceResources --output %DIR%/Resources/DeviceResources.rcc.cpp %DIR%/Resources/DeviceResources.qrc
%RCC% --name GraphResources --output %DIR%/Resources/GraphResources.rcc.cpp %DIR%/Resources/GraphResources.qrc
%RCC% --name LayoutResources --output %DIR%/Resources/LayoutResources.rcc.cpp %DIR%/Resources/LayoutResources.qrc
%RCC% --name QtBaseResources --output %DIR%/Resources/QtBaseResources.rcc.cpp %DIR%/Resources/QtBaseResources.qrc

%MOC% %DIR%/ColorLabel.h -o %DIR%/ColorLabel.moc.cpp
%MOC% %DIR%/DockHeader.h -o %DIR%/DockHeader.moc.cpp
%MOC% %DIR%/DockWidget.h -o %DIR%/DockWidget.moc.cpp
%MOC% %DIR%/ExperienceAssetCache.h -o %DIR%/ExperienceAssetCache.moc.cpp
%MOC% %DIR%/FileManager.h -o %DIR%/FileManager.moc.cpp
%MOC% %DIR%/Gamepad.h -o %DIR%/Gamepad.moc.cpp
%MOC% %DIR%/GamepadManager.h -o %DIR%/GamepadManager.moc.cpp
%MOC% %DIR%/ImageButton.h -o %DIR%/ImageButton.moc.cpp
%MOC% %DIR%/ImageManipulation.h -o %DIR%/ImageManipulation.moc.cpp
%MOC% %DIR%/Layout.h -o %DIR%/Layout.moc.cpp
%MOC% %DIR%/LayoutComboBox.h -o %DIR%/LayoutComboBox.moc.cpp
%MOC% %DIR%/LayoutManager.h -o %DIR%/LayoutManager.moc.cpp
%MOC% %DIR%/LayoutMenu.h -o %DIR%/LayoutMenu.moc.cpp
%MOC% %DIR%/LinkWidget.h -o %DIR%/LinkWidget.moc.cpp
%MOC% %DIR%/MainWindowBase.h -o %DIR%/MainWindowBase.moc.cpp
%MOC% %DIR%/PainterStaticTextCache.h -o %DIR%/PainterStaticTextCache.moc.cpp
%MOC% %DIR%/QtBaseManager.h -o %DIR%/QtBaseManager.moc.cpp
%MOC% %DIR%/singleapplication.h -o %DIR%/singleapplication.moc.cpp
%MOC% %DIR%/singleapplication_p.h -o %DIR%/singleapplication_p.moc.cpp
%MOC% %DIR%/Slider.h -o %DIR%/Slider.moc.cpp
%MOC% %DIR%/Spinbox.h -o %DIR%/Spinbox.moc.cpp
%MOC% %DIR%/SystemInfo.h -o %DIR%/SystemInfo.moc.cpp
%MOC% %DIR%/QtBaseConfig.h -o %DIR%/QtBaseConfig.moc.cpp

%MOC% %DIR%/AttributeWidgets/AttributeSetGridWidget.h -o %DIR%/AttributeWidgets/AttributeSetGridWidget.moc.cpp
%MOC% %DIR%/AttributeWidgets/AttributeWidgets.h -o %DIR%/AttributeWidgets/AttributeWidgets.moc.cpp
%MOC% %DIR%/AttributeWidgets/AttributeWidgetCreators.h -o %DIR%/AttributeWidgets/AttributeWidgetCreators.moc.cpp
%MOC% %DIR%/AttributeWidgets/AttributeWidgetFactory.h -o %DIR%/AttributeWidgets/AttributeWidgetFactory.moc.cpp
%MOC% %DIR%/AttributeWidgets/Property.h -o %DIR%/AttributeWidgets/Property.moc.cpp
%MOC% %DIR%/AttributeWidgets/PropertyManager.h -o %DIR%/AttributeWidgets/PropertyManager.moc.cpp
%MOC% %DIR%/AttributeWidgets/PropertyTreeWidget.h -o %DIR%/AttributeWidgets/PropertyTreeWidget.moc.cpp

%MOC% %DIR%/Audio/MediaContent.h -o %DIR%/Audio/MediaContent.moc.cpp

%MOC% %DIR%/AutoUpdate/AutoUpdate.h -o %DIR%/AutoUpdate/AutoUpdate.moc.cpp

%MOC% %DIR%/Backend/BackendFileSystem.h -o %DIR%/Backend/BackendFileSystem.moc.cpp
%MOC% %DIR%/Backend/BackendInterface.h -o %DIR%/Backend/BackendInterface.moc.cpp
%MOC% %DIR%/Backend/BackendParameters.h -o %DIR%/Backend/BackendParameters.moc.cpp
%MOC% %DIR%/Backend/BackendUploader.h -o %DIR%/Backend/BackendUploader.moc.cpp
%MOC% %DIR%/Backend/FileDownloader.h -o %DIR%/Backend/FileDownloader.moc.cpp
%MOC% %DIR%/Backend/NetworkAccessManager.h -o %DIR%/Backend/NetworkAccessManager.moc.cpp
%MOC% %DIR%/Backend/WebDataCache.h -o %DIR%/Backend/WebDataCache.moc.cpp

%MOC% %DIR%/Networking/NetworkClient.h -o %DIR%/Networking/NetworkClient.moc.cpp
%MOC% %DIR%/Networking/NetworkServer.h -o %DIR%/Networking/NetworkServer.moc.cpp
%MOC% %DIR%/Networking/NetworkServerClient.h -o %DIR%/Networking/NetworkServerClient.moc.cpp
%MOC% %DIR%/Networking/OscServer.h -o %DIR%/Networking/OscServer.moc.cpp

%MOC% %DIR%/PluginSystem/Plugin.h -o %DIR%/PluginSystem/Plugin.moc.cpp
%MOC% %DIR%/PluginSystem/PluginManager.h -o %DIR%/PluginSystem/PluginManager.moc.cpp
%MOC% %DIR%/PluginSystem/PluginMenu.h -o %DIR%/PluginSystem/PluginMenu.moc.cpp

%MOC% %DIR%/System/SerialPort.h -o %DIR%/System/SerialPort.moc.cpp

%MOC% %DIR%/Widgets/ColorMappingWidget.h -o %DIR%/Widgets/ColorMappingWidget.moc.cpp
%MOC% %DIR%/Widgets/GraphObjectViewWidget.h -o %DIR%/Widgets/GraphObjectViewWidget.moc.cpp
%MOC% %DIR%/Widgets/PlotWidget.h -o %DIR%/Widgets/PlotWidget.moc.cpp
%MOC% %DIR%/Widgets/SearchBoxWidget.h -o %DIR%/Widgets/SearchBoxWidget.moc.cpp
%MOC% %DIR%/Widgets/WindowFunctionWidget.h -o %DIR%/Widgets/WindowFunctionWidget.moc.cpp

%MOC% %DIR%/Windows/EnterLabelWindow.h -o %DIR%/Windows/EnterLabelWindow.moc.cpp
%MOC% %DIR%/Windows/ProgressWindow.h -o %DIR%/Windows/ProgressWindow.moc.cpp
%MOC% %DIR%/Windows/ProgressWindowManager.h -o %DIR%/Windows/ProgressWindowManager.moc.cpp
%MOC% %DIR%/Windows/StatusPopupManager.h -o %DIR%/Windows/StatusPopupManager.moc.cpp
%MOC% %DIR%/Windows/StatusPopupWindow.h -o %DIR%/Windows/StatusPopupWindow.moc.cpp
