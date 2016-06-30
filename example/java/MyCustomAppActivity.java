package org.qtproject.example;

import org.qtproject.example.R;
import org.qtproject.qt5.android.bindings.QtActivity;
import org.gmaxera.qtfacebook.QFacebookBinding;
import android.content.Intent;
import android.os.Bundle;

public class MyCustomAppActivity extends QtActivity {
    @Override
    public void onCreate(Bundle bundle) {
        super.onCreate(bundle);
        QFacebookBinding.onCreate(this, bundle);
        QFacebookBinding.setApplicationName(getString(R.string.app_name));
    }
    @Override
    protected void onResume() {
        super.onResume();
        QFacebookBinding.onResume();
    }
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        QFacebookBinding.onSaveInstanceState(outState);
    }
    @Override
    public void onPause() {
        super.onPause();
        QFacebookBinding.onPause();
    }
    @Override
    public void onDestroy() {
        QFacebookBinding.onDestroy();
        super.onDestroy();
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        QFacebookBinding.onActivityResult(requestCode, resultCode, data);
    }
}
