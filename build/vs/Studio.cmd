setlocal
set MOC=%MOC%
set RCC=%RCC%
set DIR=../../src/Studio
set INC=-I../../src/Engine -I../../src/Studio -I../../deps/include
set VAR=-DWIN32 -D_WINDOWS -DNEUROMORE_PLATFORM_WINDOWS -DHAVE_CONFIG_H -D_UNICODE -DUNICODE

%RCC% --name StudioResources --output ../../src/Studio/Resources/StudioResources.rcc.cpp ../../src/Studio/Resources/StudioResources.qrc

%MOC% %INC% %VAR% %DIR%/AppManager.h -o %DIR%/AppManager.moc.cpp
%MOC% %INC% %VAR% %DIR%/AuthenticationCenter.h -o %DIR%/AuthenticationCenter.moc.cpp
%MOC% %INC% %VAR% %DIR%/MainWindow.h -o %DIR%/MainWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/VisualizationManager.h -o %DIR%/VisualizationManager.moc.cpp
%MOC% %INC% %VAR% %DIR%/VideoPlayer.h -o %DIR%/VideoPlayer.moc.cpp

%MOC% %INC% %VAR% %DIR%/Devices/Bluetooth/BluetoothDevice.h -o %DIR%/Devices/Bluetooth/BluetoothDevice.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Bluetooth/BluetoothDriver.h -o %DIR%/Devices/Bluetooth/BluetoothDriver.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Bluetooth/BluetoothService.h -o %DIR%/Devices/Bluetooth/BluetoothService.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Brainquiry/BrainquiryDriver.h -o %DIR%/Devices/Brainquiry/BrainquiryDriver.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Brainquiry/BrainquirySerialHandler.h -o %DIR%/Devices/Brainquiry/BrainquirySerialHandler.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/NeuroSky/NeuroSkyDriver.h -o %DIR%/Devices/NeuroSky/NeuroSkyDriver.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/NeuroSky/NeuroSkySerialHandler.h -o %DIR%/Devices/NeuroSky/NeuroSkySerialHandler.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/OpenBCI/OpenBCIDriver.h -o %DIR%/Devices/OpenBCI/OpenBCIDriver.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/OpenBCI/OpenBCISerialHandler.h -o %DIR%/Devices/OpenBCI/OpenBCISerialHandler.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Versus/VersusDriver.h -o %DIR%/Devices/Versus/VersusDriver.moc.cpp
%MOC% %INC% %VAR% %DIR%/Devices/Versus/VersusSerialHandler.h -o %DIR%/Devices/Versus/VersusSerialHandler.moc.cpp

%MOC% %INC% %VAR% %DIR%/Plugins/ABM/ABMPlugin.h -o %DIR%/Plugins/ABM/ABMPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/ABM/ABMProgressWidget.h -o %DIR%/Plugins/ABM/ABMProgressWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/BackendFileSystem/BackendFileSystemPlugin.h -o %DIR%/Plugins/BackendFileSystem/BackendFileSystemPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/BackendFileSystem/BackendFileSystemWidget.h -o %DIR%/Plugins/BackendFileSystem/BackendFileSystemWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/BackendFileSystem/CreateFileWindow.h -o %DIR%/Plugins/BackendFileSystem/CreateFileWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/BackendFileSystem/CreateFolderWindow.h -o %DIR%/Plugins/BackendFileSystem/CreateFolderWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/EEGElectrodePlacementPlugin/EEGElectrodePlacementPlugin.h -o %DIR%/Plugins/Development/EEGElectrodePlacementPlugin/EEGElectrodePlacementPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/HeatmapPlugin/HeatmapPlugin.h -o %DIR%/Plugins/Development/HeatmapPlugin/HeatmapPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/HeatmapPlugin/HeatmapWidget.h -o %DIR%/Plugins/Development/HeatmapPlugin/HeatmapWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/LatencyTestPlugin/LatencyTestPlugin.h -o %DIR%/Plugins/Development/LatencyTestPlugin/LatencyTestPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/LatencyTestPlugin/LatencyTestWidget.h -o %DIR%/Plugins/Development/LatencyTestPlugin/LatencyTestWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/Loreta/LoretaPlugin.h -o %DIR%/Plugins/Development/Loreta/LoretaPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Development/Loreta/LoretaWidget.h -o %DIR%/Plugins/Development/Loreta/LoretaWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Devices/BciDeviceWidget.h -o %DIR%/Plugins/Devices/BciDeviceWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Devices/DevicesPlugin.h -o %DIR%/Plugins/Devices/DevicesPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Devices/DevicesWidget.h -o %DIR%/Plugins/Devices/DevicesWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Devices/DeviceWidget.h -o %DIR%/Plugins/Devices/DeviceWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Devices/NoDeviceWidget.h -o %DIR%/Plugins/Devices/NoDeviceWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/EngineStatus/EngineStatusPlugin.h -o %DIR%/Plugins/EngineStatus/EngineStatusPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLPlugin.h -o %DIR%/Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLWidget.h -o %DIR%/Plugins/Examples/SimpleOpenGLPlugin/ExampleOpenGLWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Experience/ExperiencePlugin.h -o %DIR%/Plugins/Experience/ExperiencePlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Experience/ExperienceWidget.h -o %DIR%/Plugins/Experience/ExperienceWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/ExperienceSelection/ExperienceSelectionPlugin.h -o %DIR%/Plugins/ExperienceSelection/ExperienceSelectionPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/ExperienceSelection/ExperienceSelectionWidget.h -o %DIR%/Plugins/ExperienceSelection/ExperienceSelectionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Feedback/FeedbackPlugin.h -o %DIR%/Plugins/Feedback/FeedbackPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Feedback/FeedbackHistoryWidget.h -o %DIR%/Plugins/Feedback/FeedbackHistoryWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/ClassifierPlugin.h -o %DIR%/Plugins/Graph/ClassifierPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphAttributesWidget.h -o %DIR%/Plugins/Graph/GraphAttributesWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphInfoWidget.h -o %DIR%/Plugins/Graph/GraphInfoWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphHelpers.h -o %DIR%/Plugins/Graph/GraphHelpers.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphRenderer.h -o %DIR%/Plugins/Graph/GraphRenderer.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphRendererState.h -o %DIR%/Plugins/Graph/GraphRendererState.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphPaletteWidget.h -o %DIR%/Plugins/Graph/GraphPaletteWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphPlugin.h -o %DIR%/Plugins/Graph/GraphPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphShared.h -o %DIR%/Plugins/Graph/GraphShared.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphTextPixmapCache.h -o %DIR%/Plugins/Graph/GraphTextPixmapCache.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/GraphWidget.h -o %DIR%/Plugins/Graph/GraphWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Graph/StateMachinePlugin.h -o %DIR%/Plugins/Graph/StateMachinePlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Networking/NetworkTestClientPlugin.h -o %DIR%/Plugins/Networking/NetworkTestClientPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Parameter/ParameterControlPlugin.h -o %DIR%/Plugins/Parameter/ParameterControlPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Parameter/ParametersWidget.h -o %DIR%/Plugins/Parameter/ParametersWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Parameter/ParameterWidgets.h -o %DIR%/Plugins/Parameter/ParameterWidgets.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/RawWaveform/RawWaveformPlugin.h -o %DIR%/Plugins/RawWaveform/RawWaveformPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/RawWaveform/RawWaveformWidget.h -o %DIR%/Plugins/RawWaveform/RawWaveformWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/ClientInfoWidget.h -o %DIR%/Plugins/SessionControl/ClientInfoWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/PreSessionWidget.h -o %DIR%/Plugins/SessionControl/PreSessionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/SessionControlPlugin.h -o %DIR%/Plugins/SessionControl/SessionControlPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/SessionInfoWidget.h -o %DIR%/Plugins/SessionControl/SessionInfoWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/StageControlWidget.h -o %DIR%/Plugins/SessionControl/StageControlWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SessionControl/WhileSessionWidget.h -o %DIR%/Plugins/SessionControl/WhileSessionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram/SpectrogramBandsPlugin.h -o %DIR%/Plugins/Spectrogram/SpectrogramBandsPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram/SpectrogramPlugin.h -o %DIR%/Plugins/Spectrogram/SpectrogramPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram/SpectrogramPluginCollection.h -o %DIR%/Plugins/Spectrogram/SpectrogramPluginCollection.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram/SpectrogramSurfacePlugin.h -o %DIR%/Plugins/Spectrogram/SpectrogramSurfacePlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram2D/Spectrogram2DPlugin.h -o %DIR%/Plugins/Spectrogram2D/Spectrogram2DPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/Spectrogram2D/Spectrogram2DWidget.h -o %DIR%/Plugins/Spectrogram2D/Spectrogram2DWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsPlugin.h -o %DIR%/Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsWidget.h -o %DIR%/Plugins/SpectrumAnalyzerSettings/SpectrumAnalyzerSettingsWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/View/ViewPlugin.h -o %DIR%/Plugins/View/ViewPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/View/ViewSpectrumPlugin.h -o %DIR%/Plugins/View/ViewSpectrumPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/View/ViewSpectrumWidget.h -o %DIR%/Plugins/View/ViewSpectrumWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/View/ViewWidget.h -o %DIR%/Plugins/View/ViewWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/DataVisualizationPlugin.h -o %DIR%/Plugins/DataVisualizationPlugin.moc.cpp
%MOC% %INC% %VAR% %DIR%/Plugins/DataVisualizationSettingsWidget.h -o %DIR%/Plugins/DataVisualizationSettingsWidget.moc.cpp

%MOC% %INC% %VAR% %DIR%/Rendering/OpenGLManager.h -o %DIR%/Rendering/OpenGLManager.moc.cpp
%MOC% %INC% %VAR% %DIR%/Rendering/OpenGLWidget.h -o %DIR%/Rendering/OpenGLWidget.moc.cpp

%MOC% %INC% %VAR% %DIR%/Widgets/BatteryStatusWidget.h -o %DIR%/Widgets/BatteryStatusWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/BreathingRateWidget.h -o %DIR%/Widgets/BreathingRateWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/ChannelMultiSelectionWidget.h -o %DIR%/Widgets/ChannelMultiSelectionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/ChannelSelectionWidget.h -o %DIR%/Widgets/ChannelSelectionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/DeviceSelectionWidget.h -o %DIR%/Widgets/DeviceSelectionWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/EEGElectrodesWidget.h -o %DIR%/Widgets/EEGElectrodesWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/FrequencyBandCheckboxWidget.h -o %DIR%/Widgets/FrequencyBandCheckboxWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/HeartRateWidget.h -o %DIR%/Widgets/HeartRateWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/HMultiCheckboxWidget.h -o %DIR%/Widgets/HMultiCheckboxWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/ImpedanceTestWidget.h -o %DIR%/Widgets/ImpedanceTestWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/SensorCheckboxWidget.h -o %DIR%/Widgets/SensorCheckboxWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/SignalQualityWidget.h -o %DIR%/Widgets/SignalQualityWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/SkinTemperatureWidget.h -o %DIR%/Widgets/SkinTemperatureWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/StopwatchWidget.h -o %DIR%/Widgets/StopwatchWidget.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/SwitchingImage.h -o %DIR%/Widgets/SwitchingImage.moc.cpp
%MOC% %INC% %VAR% %DIR%/Widgets/ThermometerWidget.h -o %DIR%/Widgets/ThermometerWidget.moc.cpp

%MOC% %INC% %VAR% %DIR%/Windows/AboutWindow.h -o %DIR%/Windows/AboutWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/CreateUserWindow.h -o %DIR%/Windows/CreateUserWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/EmailValidator.h -o %DIR%/Windows/EmailValidator.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/ExperienceWizardWindow.h -o %DIR%/Windows/ExperienceWizardWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/InviteUserWindow.h -o %DIR%/Windows/InviteUserWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/LicenseAgreement.h -o %DIR%/Windows/LicenseAgreement.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/LicenseAgreementWindow.h -o %DIR%/Windows/LicenseAgreementWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/LoginWindow.h -o %DIR%/Windows/LoginWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/ReportWindow.h -o %DIR%/Windows/ReportWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/SelectUserWindow.h -o %DIR%/Windows/SelectUserWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/SettingsWindow.h -o %DIR%/Windows/SettingsWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/UpgradeWindow.h -o %DIR%/Windows/UpgradeWindow.moc.cpp
%MOC% %INC% %VAR% %DIR%/Windows/VisualizationSelectWindow.h -o %DIR%/Windows/VisualizationSelectWindow.moc.cpp
