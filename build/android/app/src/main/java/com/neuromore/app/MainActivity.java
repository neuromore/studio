package com.neuromore.app;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Toast;

import com.neuromore.engine.Wrapper;

public class MainActivity extends AppCompatActivity {

    /**
     * Shows an toast with text.
     * @param text text.
     * @param longLength true for long
     */
    public void showToast(final String text, boolean longLength)
    {
        final Toast toast = Toast.makeText(
                this, text, longLength ? Toast.LENGTH_LONG : Toast.LENGTH_SHORT);

        toast.show();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //////////////////////////////////////////////////////////////////////////////////////
        // 1) init engine
        Wrapper.Init(engineCallback);

        // check init
        if (Wrapper.IsInitialized()) {
            showToast("Init Success", false);
        }
        else {
            showToast("Init Fail", false);
        }

        //////////////////////////////////////////////////////////////////////////////////////
        // 2) load some hardcoded experience (classifier/statemachine)

        //Wrapper.LoadClassifier();
        //Wrapper.LoadStateMachine();

        //////////////////////////////////////////////////////////////////////////////////////
        // 3) try start engine

        if (Wrapper.IsReady()) {
            showToast("Ready: Yes", false);

            if (Wrapper.Start()) {
                showToast("Start Success", false);
            }
            else {
                showToast("Start Fail", false);
            }
        }
        else {
            showToast("Ready: No", false);
        }

        Wrapper.Stop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        // shutdown engine
        if (Wrapper.IsInitialized()) {
            Wrapper.Shutdown();
        }
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
