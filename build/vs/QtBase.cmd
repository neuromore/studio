setlocal
set MOC=%MOC%
set RCC=%RCC%
set DIR=../../src/QtBase
set PCH=QtBase/Precompiled.h

%RCC% --name Assets --output %DIR%/Resources/Assets.rcc.cpp %DIR%/Resources/Assets.qrc
%RCC% --name DeviceResources --output %DIR%/Resources/DeviceResources.rcc.cpp %DIR%/Resources/DeviceResources.qrc
%RCC% --name GraphResources --output %DIR%/Resources/GraphResources.rcc.cpp %DIR%/Resources/GraphResources.qrc
%RCC% --name LayoutResources --output %DIR%/Resources/LayoutResources.rcc.cpp %DIR%/Resources/LayoutResources.qrc
%RCC% --name QtBaseResources --output %DIR%/Resources/QtBaseResources.rcc.cpp %DIR%/Resources/QtBaseResources.qrc

%MOC% %DIR%/ColorLabel.h -b %PCH% -o %DIR%/ColorLabel.moc.cpp
%MOC% %DIR%/DockHeader.h -b %PCH% -o %DIR%/DockHeader.moc.cpp
%MOC% %DIR%/DockWidget.h -b %PCH% -o %DIR%/DockWidget.moc.cpp
%MOC% %DIR%/ExperienceAssetCache.h -b %PCH% -o %DIR%/ExperienceAssetCache.moc.cpp
%MOC% %DIR%/FileManager.h -b %PCH% -o %DIR%/FileManager.moc.cpp
%MOC% %DIR%/Gamepad.h -b %PCH% -o %DIR%/Gamepad.moc.cpp
%MOC% %DIR%/GamepadManager.h -b %PCH% -o %DIR%/GamepadManager.moc.cpp
%MOC% %DIR%/ImageButton.h -b %PCH% -o %DIR%/ImageButton.moc.cpp
%MOC% %DIR%/Layout.h -b %PCH% -o %DIR%/Layout.moc.cpp
%MOC% %DIR%/LayoutComboBox.h -b %PCH% -o %DIR%/LayoutComboBox.moc.cpp
%MOC% %DIR%/LayoutManager.h -b %PCH% -o %DIR%/LayoutManager.moc.cpp
%MOC% %DIR%/LayoutMenu.h -b %PCH% -o %DIR%/LayoutMenu.moc.cpp
%MOC% %DIR%/LinkWidget.h -b %PCH% -o %DIR%/LinkWidget.moc.cpp
%MOC% %DIR%/MainWindowBase.h -b %PCH% -o %DIR%/MainWindowBase.moc.cpp
%MOC% %DIR%/PainterStaticTextCache.h -b %PCH% -o %DIR%/PainterStaticTextCache.moc.cpp
%MOC% %DIR%/QtBaseManager.h -b %PCH% -o %DIR%/QtBaseManager.moc.cpp
%MOC% %DIR%/Slider.h -b %PCH% -o %DIR%/Slider.moc.cpp
%MOC% %DIR%/Spinbox.h -b %PCH% -o %DIR%/Spinbox.moc.cpp

%MOC% %DIR%/AttributeWidgets/AttributeSetGridWidget.h -b %PCH% -o %DIR%/AttributeWidgets/AttributeSetGridWidget.moc.cpp
%MOC% %DIR%/AttributeWidgets/AttributeWidgets.h -b %PCH% -o %DIR%/AttributeWidgets/AttributeWidgets.moc.cpp
%MOC% %DIR%/AttributeWidgets/PropertyManager.h -b %PCH% -o %DIR%/AttributeWidgets/PropertyManager.moc.cpp
%MOC% %DIR%/AttributeWidgets/PropertyTreeWidget.h -b %PCH% -o %DIR%/AttributeWidgets/PropertyTreeWidget.moc.cpp

%MOC% %DIR%/Audio/MediaContent.h -b %PCH% -o %DIR%/Audio/MediaContent.moc.cpp

%MOC% %DIR%/Backend/BackendFileSystem.h -b %PCH% -o %DIR%/Backend/BackendFileSystem.moc.cpp
%MOC% %DIR%/Backend/BackendInterface.h -b %PCH% -o %DIR%/Backend/BackendInterface.moc.cpp
%MOC% %DIR%/Backend/BackendParameters.h -b %PCH% -o %DIR%/Backend/BackendParameters.moc.cpp
%MOC% %DIR%/Backend/BackendUploader.h -b %PCH% -o %DIR%/Backend/BackendUploader.moc.cpp
%MOC% %DIR%/Backend/FileDownloader.h -b %PCH% -o %DIR%/Backend/FileDownloader.moc.cpp
%MOC% %DIR%/Backend/NetworkAccessManager.h -b %PCH% -o %DIR%/Backend/NetworkAccessManager.moc.cpp
%MOC% %DIR%/Backend/WebDataCache.h -b %PCH% -o %DIR%/Backend/WebDataCache.moc.cpp

%MOC% %DIR%/Networking/NetworkClient.h -b %PCH% -o %DIR%/Networking/NetworkClient.moc.cpp
%MOC% %DIR%/Networking/NetworkServer.h -b %PCH% -o %DIR%/Networking/NetworkServer.moc.cpp
%MOC% %DIR%/Networking/NetworkServerClient.h -b %PCH% -o %DIR%/Networking/NetworkServerClient.moc.cpp
%MOC% %DIR%/Networking/OscServer.h -b %PCH% -o %DIR%/Networking/OscServer.moc.cpp

%MOC% %DIR%/PluginSystem/Plugin.h -b %PCH% -o %DIR%/PluginSystem/Plugin.moc.cpp
%MOC% %DIR%/PluginSystem/PluginManager.h -b %PCH% -o %DIR%/PluginSystem/PluginManager.moc.cpp
%MOC% %DIR%/PluginSystem/PluginMenu.h -b %PCH% -o %DIR%/PluginSystem/PluginMenu.moc.cpp

%MOC% %DIR%/System/SerialPort.h -b %PCH% -o %DIR%/System/SerialPort.moc.cpp

%MOC% %DIR%/Widgets/ColorMappingWidget.h -b %PCH% -o %DIR%/Widgets/ColorMappingWidget.moc.cpp
%MOC% %DIR%/Widgets/GraphObjectViewWidget.h -b %PCH% -o %DIR%/Widgets/GraphObjectViewWidget.moc.cpp
%MOC% %DIR%/Widgets/PlotWidget.h -b %PCH% -o %DIR%/Widgets/PlotWidget.moc.cpp
%MOC% %DIR%/Widgets/SearchBoxWidget.h -b %PCH% -o %DIR%/Widgets/SearchBoxWidget.moc.cpp
%MOC% %DIR%/Widgets/WindowFunctionWidget.h -b %PCH% -o %DIR%/Widgets/WindowFunctionWidget.moc.cpp

%MOC% %DIR%/Windows/EnterLabelWindow.h -b %PCH% -o %DIR%/Windows/EnterLabelWindow.moc.cpp
%MOC% %DIR%/Windows/ProgressWindow.h -b %PCH% -o %DIR%/Windows/ProgressWindow.moc.cpp
%MOC% %DIR%/Windows/ProgressWindowManager.h -b %PCH% -o %DIR%/Windows/ProgressWindowManager.moc.cpp
%MOC% %DIR%/Windows/StatusPopupManager.h -b %PCH% -o %DIR%/Windows/StatusPopupManager.moc.cpp
%MOC% %DIR%/Windows/StatusPopupWindow.h -b %PCH% -o %DIR%/Windows/StatusPopupWindow.moc.cpp
