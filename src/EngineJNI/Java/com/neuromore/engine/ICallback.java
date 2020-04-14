/****************************************************************************
**
** Copyright 2019 neuromore co
** Contact: https://neuromore.com/contact
**
** Commercial License Usage
** Licensees holding valid commercial neuromore licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and neuromore. For licensing terms
** and conditions see https://neuromore.com/licensing. For further
** information use the contact form at https://neuromore.com/contact.
**
** neuromore Public License Usage
** Alternatively, this file may be used under the terms of the neuromore
** Public License version 1 as published by neuromore co with exceptions as 
** appearing in the file neuromore-class-exception.md included in the 
** packaging of this file. Please review the following information to 
** ensure the neuromore Public License requirements will be met: 
** https://neuromore.com/npl
**
****************************************************************************/

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
