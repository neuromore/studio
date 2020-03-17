/*
 * neuromore Android App
 * Copyright (c) 2012-2016 neuromore Inc.
 * All Rights Reserved.
 */

package com.neuromore.app;

import android.app.Application;
import android.widget.Toast;

import com.neuromore.engine.Wrapper;

/**
 * This class extends the default Application class.
 * It can be assumed the main controller in a MVC pattern.
 * It's also the backend response handler.
 */
public final class App extends Application
{
    private static App singleton;
    public static App getSingleton() { return singleton; }

    /**
     * Constructor
     */
    public App()
    {
        super();
        singleton = this;
    }

    @Override
    public void onCreate()
    {
        super.onCreate();
    }

    /**
     * Handles callbacks from the Engine
     */
    private final com.neuromore.engine.ICallback engineCallback = new com.neuromore.engine.ICallback()
    {
        @Override
        public final void OnStop(int status)
        {
        }

        @Override
        public final void OnPlayAudio(String url, int numLoops, double beginAt, double volume)
        {
        }

        @Override
        public final void OnStopAudio(String url)
        {
        }

        @Override
        public final void OnPauseAudio(String url, boolean unPause)
        {
        }

        @Override
        public final void OnSetAudioVolume(String url, double volume)
        {
        }

        @Override
        public final void OnSeekAudio(String url, int millisecs)
        {
        }

        @Override
        public final void OnPlayVideo(String url, int numLoops, double beginAt, double volume)
        {
        }

        @Override
        public final void OnStopVideo()
        {
        }

        @Override
        public final void OnPauseVideo(String url, boolean unPause)
        {
        }

        @Override
        public final void OnSetVideoVolume(String url, double volume)
        {
        }

        @Override
        public final void OnSeekVideo(String url, int millisecs)
        {
        }

        @Override
        public final void OnShowImage(String url)
        {
        }

        @Override
        public final void OnHideImage()
        {
        }

        @Override
        public final void OnShowText(String text, float colorR, float colorG, float colorB, float colorA)
        {
        }

        @Override
        public final void OnHideText()
        {
        }

        @Override
        public final void OnShowButton(String text, int buttonId)
        {
        }

        @Override
        public final void OnClearButtons()
        {
        }

        @Override
        public final void OnCommand(String command)
        {
        }

        @Override
        public final void OnVolumeChanged(double volume)
        {
        }

        @Override
        public final void OnProgressChanged(double progress)
        {
        }

        @Override
        public final void OnExperienceLoaded(
            final boolean loaded,
            final String[] feedbacks,
            final String[] audios,
            final String[] videos,
            final String[] images,
            final String userId)
        {
        }
    };
}
