setlocal
set MOC=%MOC%
set RCC=%RCC%
set PCH=Studio/Precompiled.h
set DIR=../../src/Studio
set INC=-I../../src/Engine -I../../src/Studio -I../../deps/include
set VAR=--no-notes -DWIN32 -D_WINDOWS -DNEUROMORE_PLATFORM_WINDOWS -DHAVE_CONFIG_H -D_UNICODE -DUNICODE

%RCC% --name StudioResources --output ../../src/Studio/Resources/StudioResources.rcc.cpp ../../src/Studio/Resources/StudioResources.qrc

%MOC% %INC% %VAR% %DIR%/AppManager.h -b %PCH% -o %DIR%/AppManager.moc.cpp
%MOC% %INC% %VAR% %DIR%/AuthenticationCenter.h -b %PCH% -o %DIR%/AuthenticationCenter.moc.cpp
%MOC% %INC% %VAR% %DIR%/MainWindow.h -b %PCH% -o %DIR%/MainWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/VisualizationManager.h -b %PCH% -o %DIR%/VisualizationManager.moc.cpp
%MOC% %INC% %VAR% %DIR%/VideoPlayer.h -b %PCH% -o %DIR%/VideoPlayer.moc.cpp
%MOC% %INC% %VAR% %DIR%/OnboardingAction.h -b %PCH% -o %DIR%/OnboardingAction.moc.cpp
%MOC% %INC% %VAR% %DIR%/TourManager.h -b %PCH% -o %DIR%/TourManager.moc.cpp

%MOC% %INC% %VAR% %DIR%/Devices/Bluetooth/BluetoothDevice.h -b %PCH% -o %DIR%/Devices/Bluetooth/BluetoothDevice.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Bluetooth/BluetoothDriver.h -b %PCH% -o %DIR%/Devices/Bluetooth/BluetoothDriver.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Bluetooth/BluetoothService.h -b %PCH% -o %DIR%/Devices/Bluetooth/BluetoothService.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Brainquiry/BrainquiryDriver.h -b %PCH% -o %DIR%/Devices/Brainquiry/BrainquiryDriver.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Brainquiry/BrainquirySerialHandler.h -b %PCH% -o %DIR%/Devices/Brainquiry/BrainquirySerialHandler.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/NeuroSky/NeuroSkyDriver.h -b %PCH% -o %DIR%/Devices/NeuroSky/NeuroSkyDriver.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/NeuroSky/NeuroSkySerialHandler.h -b %PCH% -o %DIR%/Devices/NeuroSky/NeuroSkySerialHandler.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/OpenBCI/OpenBCIDriver.h -b %PCH% -o %DIR%/Devices/OpenBCI/OpenBCIDriver.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/OpenBCI/OpenBCISerialHandler.h -b %PCH% -o %DIR%/Devices/OpenBCI/OpenBCISerialHandler.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Versus/VersusDriver.h -b %PCH% -o %DIR%/Devices/Versus/VersusDriver.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Versus/VersusSerialHandler.h -b %PCH% -o %DIR%/Devices/Versus/VersusSerialHandler.moc.cpp

%MOC% %INC% %VAR% %DIR%/Plugins/ABM/ABMPlugin.h -b %PCH% -o %DIR%/Plugins/ABM/ABMPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/ABM/ABMProgressWidget.h -b %PCH% -o %DIR%/Plugins/ABM/ABMProgressWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/BackendFileSystem/BackendFileSystemPlugin.h -b %PCH% -o %DIR%/Plugins/BackendFileSystem/BackendFileSystemPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/BackendFileSystem/BackendFileSystemWidget.h -b %PCH% -o %DIR%/Plugins/BackendFileSystem/BackendFileSystemWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/BackendFileSystem/CreateFileWindow.h -b %PCH% -o %DIR%/Plugins/BackendFileSystem/CreateFileWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/BackendFileSystem/CreateFolderWindow.h -b %PCH% -o %DIR%/Plugins/BackendFileSystem/CreateFolderWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/EEGElectrodePlacementPlugin/EEGElectrodePlacementPlugin.h -b %PCH% -o %DIR%/Plugins/Development/EEGElectrodePlacementPlugin/EEGElectrodePlacementPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/HeatmapPlugin/HeatmapPlugin.h -b %PCH% -o %DIR%/Plugins/Development/HeatmapPlugin/HeatmapPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/HeatmapPlugin/HeatmapWidget.h -b %PCH% -o %DIR%/Plugins/Development/HeatmapPlugin/HeatmapWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/LatencyTestPlugin/LatencyTestPlugin.h -b %PCH% -o %DIR%/Plugins/Development/LatencyTestPlugin/LatencyTestPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/LatencyTestPlugin/LatencyTestWidget.h -b %PCH% -o %DIR%/Plugins/Development/LatencyTestPlugin/LatencyTestWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Devices/BciDeviceWidget.h -b %PCH% -o %DIR%/Plugins/Devices/BciDeviceWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Devices/DevicesPlugin.h -b %PCH% -o %DIR%/Plugins/Devices/DevicesPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Devices/DevicesWidget.h -b %PCH% -o %DIR%/Plugins/Devices/DevicesWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Devices/DeviceWidget.h -b %PCH% -o %DIR%/Plugins/Devices/DeviceWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Devices/NoDeviceWidget.h -b %PCH% -o %DIR%/Plugins/Devices/NoDeviceWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/EngineStatus/EngineStatusPlugin.h -b %PCH% -o %DIR%/Plugins/EngineStatus/EngineStatusPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLPlugin.h -b %PCH% -o %DIR%/Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLWidget.h -b %PCH% -o %DIR%/Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Experience/ExperiencePlugin.h -b %PCH% -o %DIR%/Plugins/Experience/ExperiencePlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Experience/ExperienceWidget.h -b %PCH% -o %DIR%/Plugins/Experience/ExperienceWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/ExperienceSelection/ExperienceSelectionPlugin.h -b %PCH% -o %DIR%/Plugins/ExperienceSelection/ExperienceSelectionPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/ExperienceSelection/ExperienceSelectionWidget.h -b %PCH% -o %DIR%/Plugins/ExperienceSelection/ExperienceSelectionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/ExperienceSelection/CreateExperienceWindow.h -b %PCH% -o %DIR%/Plugins/ExperienceSelection/CreateExperienceWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Feedback/FeedbackPlugin.h -b %PCH% -o %DIR%/Plugins/Feedback/FeedbackPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Feedback/FeedbackHistoryWidget.h -b %PCH% -o %DIR%/Plugins/Feedback/FeedbackHistoryWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/ClassifierPlugin.h -b %PCH% -o %DIR%/Plugins/Graph/ClassifierPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphAttributesWidget.h -b %PCH% -o %DIR%/Plugins/Graph/GraphAttributesWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphInfoWidget.h -b %PCH% -o %DIR%/Plugins/Graph/GraphInfoWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphPaletteWidget.h -b %PCH% -o %DIR%/Plugins/Graph/GraphPaletteWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphPlugin.h -b %PCH% -o %DIR%/Plugins/Graph/GraphPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphShared.h -b %PCH% -o %DIR%/Plugins/Graph/GraphShared.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphTextPixmapCache.h -b %PCH% -o %DIR%/Plugins/Graph/GraphTextPixmapCache.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphWidget.h -b %PCH% -o %DIR%/Plugins/Graph/GraphWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/StateMachinePlugin.h -b %PCH% -o %DIR%/Plugins/Graph/StateMachinePlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Networking/NetworkTestClientPlugin.h -b %PCH% -o %DIR%/Plugins/Networking/NetworkTestClientPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Parameter/ParameterControlPlugin.h -b %PCH% -o %DIR%/Plugins/Parameter/ParameterControlPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Parameter/ParametersWidget.h -b %PCH% -o %DIR%/Plugins/Parameter/ParametersWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Parameter/ParameterWidgets.h -b %PCH% -o %DIR%/Plugins/Parameter/ParameterWidgets.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/RawWaveform/RawWaveformPlugin.h -b %PCH% -o %DIR%/Plugins/RawWaveform/RawWaveformPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/RawWaveform/RawWaveformWidget.h -b %PCH% -o %DIR%/Plugins/RawWaveform/RawWaveformWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/ClientInfoWidget.h -b %PCH% -o %DIR%/Plugins/SessionControl/ClientInfoWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/PreSessionWidget.h -b %PCH% -o %DIR%/Plugins/SessionControl/PreSessionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/SessionControlPlugin.h -b %PCH% -o %DIR%/Plugins/SessionControl/SessionControlPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/SessionInfoWidget.h -b %PCH% -o %DIR%/Plugins/SessionControl/SessionInfoWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/StageControlWidget.h -b %PCH% -o %DIR%/Plugins/SessionControl/StageControlWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/WhileSessionWidget.h -b %PCH% -o %DIR%/Plugins/SessionControl/WhileSessionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram/SpectrogramBandsPlugin.h -b %PCH% -o %DIR%/Plugins/Spectrogram/SpectrogramBandsPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram/SpectrogramPlugin.h -b %PCH% -o %DIR%/Plugins/Spectrogram/SpectrogramPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram/SpectrogramPluginCollection.h -b %PCH% -o %DIR%/Plugins/Spectrogram/SpectrogramPluginCollection.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram/SpectrogramSurfacePlugin.h -b %PCH% -o %DIR%/Plugins/Spectrogram/SpectrogramSurfacePlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram2D/Spectrogram2DPlugin.h -b %PCH% -o %DIR%/Plugins/Spectrogram2D/Spectrogram2DPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram2D/Spectrogram2DWidget.h -b %PCH% -o %DIR%/Plugins/Spectrogram2D/Spectrogram2DWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsPlugin.h -b %PCH% -o %DIR%/Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsWidget.h -b %PCH% -o %DIR%/Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/View/ViewPlugin.h -b %PCH% -o %DIR%/Plugins/View/ViewPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/View/ViewSpectrumPlugin.h -b %PCH% -o %DIR%/Plugins/View/ViewSpectrumPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/View/ViewSpectrumWidget.h -b %PCH% -o %DIR%/Plugins/View/ViewSpectrumWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/View/ViewWidget.h -b %PCH% -o %DIR%/Plugins/View/ViewWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/DataVisualizationPlugin.h -b %PCH% -o %DIR%/Plugins/DataVisualizationPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/DataVisualizationSettingsWidget.h -b %PCH% -o %DIR%/Plugins/DataVisualizationSettingsWidget.moc.cpp

%MOC% %INC% %VAR% %DIR%/Rendering/OpenGLManager.h -b %PCH% -o %DIR%/Rendering/OpenGLManager.moc.cpp
%MOC% %INC% %VAR% %DIR%/Rendering/OpenGLWidget.h -b %PCH% -o %DIR%/Rendering/OpenGLWidget.moc.cpp

%MOC% %INC% %VAR% %DIR%/Widgets/BatteryStatusWidget.h -b %PCH% -o %DIR%/Widgets/BatteryStatusWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/BreathingRateWidget.h -b %PCH% -o %DIR%/Widgets/BreathingRateWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/ChannelMultiSelectionWidget.h -b %PCH% -o %DIR%/Widgets/ChannelMultiSelectionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/ChannelSelectionWidget.h -b %PCH% -o %DIR%/Widgets/ChannelSelectionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/DeviceSelectionWidget.h -b %PCH% -o %DIR%/Widgets/DeviceSelectionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/EEGElectrodesWidget.h -b %PCH% -o %DIR%/Widgets/EEGElectrodesWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/FrequencyBandCheckboxWidget.h -b %PCH% -o %DIR%/Widgets/FrequencyBandCheckboxWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/HeartRateWidget.h -b %PCH% -o %DIR%/Widgets/HeartRateWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/HMultiCheckboxWidget.h -b %PCH% -o %DIR%/Widgets/HMultiCheckboxWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/ImpedanceTestWidget.h -b %PCH% -o %DIR%/Widgets/ImpedanceTestWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/SensorCheckboxWidget.h -b %PCH% -o %DIR%/Widgets/SensorCheckboxWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/SignalQualityWidget.h -b %PCH% -o %DIR%/Widgets/SignalQualityWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/SkinTemperatureWidget.h -b %PCH% -o %DIR%/Widgets/SkinTemperatureWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/StopwatchWidget.h -b %PCH% -o %DIR%/Widgets/StopwatchWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/SwitchingImage.h -b %PCH% -o %DIR%/Widgets/SwitchingImage.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/ThermometerWidget.h -b %PCH% -o %DIR%/Widgets/ThermometerWidget.moc.cpp

%MOC% %INC% %VAR% %DIR%/Windows/AboutWindow.h -b %PCH% -o %DIR%/Windows/AboutWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/CreateUserWindow.h -b %PCH% -o %DIR%/Windows/CreateUserWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/EmailValidator.h -b %PCH% -o %DIR%/Windows/EmailValidator.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/ExperienceWizardWindow.h -b %PCH% -o %DIR%/Windows/ExperienceWizardWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/InviteUserWindow.h -b %PCH% -o %DIR%/Windows/InviteUserWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/LicenseAgreement.h -b %PCH% -o %DIR%/Windows/LicenseAgreement.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/LicenseAgreementWindow.h -b %PCH% -o %DIR%/Windows/LicenseAgreementWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/LoginWindow.h -b %PCH% -o %DIR%/Windows/LoginWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/ReportWindow.h -b %PCH% -o %DIR%/Windows/ReportWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/SelectUserWindow.h -b %PCH% -o %DIR%/Windows/SelectUserWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/SettingsWindow.h -b %PCH% -o %DIR%/Windows/SettingsWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/UpgradeWindow.h -b %PCH% -o %DIR%/Windows/UpgradeWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/VisualizationSelectWindow.h -b %PCH% -o %DIR%/Windows/VisualizationSelectWindow.moc.cpp
