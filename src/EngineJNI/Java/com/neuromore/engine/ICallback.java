/*
 * neuromore Android App
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

package com.neuromore.engine;

/**
 * This is the java equivalent to the neuromore Callback Interface.
 * Therefore a neuromore Callback controller must implement this.
 * Also these function signatures MUST match what is expected on the native side.
 */
public interface ICallback
{
    void OnStop(int status);    // use EStatus
    void OnPlayAudio(String url, int numLoops, double beginAt, double volume);
    void OnStopAudio(String url);
    void OnPauseAudio(String url, boolean unPause);
    void OnSetAudioVolume(String url, double volume);
    void OnSeekAudio(String url, int millisecs);
    void OnPlayVideo(String url, int numLoops, double beginAt, double volume);
    void OnStopVideo();
    void OnPauseVideo(String url, boolean unPause);
    void OnSetVideoVolume(String url, double volume);
    void OnSeekVideo(String url, int millisecs);
    void OnShowImage(String url);
    void OnHideImage();
    void OnShowText(String text, float colorR, float colorG, float colorB, float colorA);
    void OnHideText();
    void OnShowButton(String text, int buttonId);
    void OnClearButtons();
    void OnCommand(String url);

    void OnVolumeChanged(double volume);
    void OnProgressChanged(double progress);

    void OnExperienceLoaded(
        final boolean loaded,
        final String[] feedbacks,
        final String[] audios,
        final String[] videos,
        final String[] images,
        final String userId);
}
