/**
* @brief LLCEFLib - Wrapper for CEF SDK for use in LL Web Media Plugin
*                   Test app (as a DLL for compatibility with media system)
* $LicenseInfo:firstyear=2001&license=viewerlgpl$
* Second Life Viewer Source Code
* Copyright (C) 2015, Linden Research, Inc.
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation;
* version 2.1 of the License only.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*
* Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
* $/LicenseInfo$
*/

#ifndef _UBROWSERDLL
#define _UBROWSERDLL

#include <vector>

#include <GL/glui.h>
#include "GL/freeglut.h"

static void gluiCallbackWrapper(int controlIdIn);

class uBrowser
{
    public:
        uBrowser();

        void init(int glutWindow);

        // glut impls
        void reshape(int widthIn, int heightIn);
        void display();
        void idle();
        void keyboard(int keyIn, int xIn, int yIn);
        void specialKeyboard(int keyIn, int xIn, int yIn);
        void passiveMouse(int xIn, int yIn);
        void mouseMove(int xIn, int yIn);
        void mouseButton(int buttonIn, int stateIn, int xIn, int yIn);

        // GLUI to my app callback
        void gluiCallback(int controlIdIn);

        // callbacks triggered by LLCEFLib
        void onPageChangedCallback(unsigned char* pixels, int width, int height);
        void onCustomSchemeURLCallback(std::string url);
        void onConsoleMessageCallback(std::string message, std::string source, int line);
        void onStatusMessageCallback(std::string value);
		void onTitleChangeCallback(std::string title);
		void onRequestExitCallback();

    private:
        void makeChrome();
        bool loadSettings(std::string filename);
        bool saveSettings(std::string filename);
        void drawGeometry(int geomTypeIn, int updateTypeIn);
        void windowPosToTexturePos(int winXIn, int winYIn, int& texXOut, int& texYOut);
        void setSize(int widthIn, int heightIn);
        void resetView();

        // CEF Browser
        LLCEFLib* mLLCEFLib;

        // GLUT window handle (consumed by GLUI)
        int mAppWindow;

        // window, browser and app textures params
        int mWindowWidth;
        int mWindowHeight;
        int mTextureWidth;
        int mTextureHeight;
        float mTextureScaleX;
        float mTextureScaleY;
        float mViewportAspect;

        // GLUI controls & IDs
        GLUI* mTopGLUIWindow;
        GLUI_Button* mNavBackButton;
        GLUI_Button* mNavStopButton;
        GLUI_Button* mNavForwardButton;
        GLUI* mTop2GLUIWindow;
        GLUI_EditText* mUrlEdit;
        GLUI_String mNavUrl;
        GLUI* mRightGLUIWindow;
        GLUI_Rotation* mViewRotationCtrl;
        GLUI_Translation* mViewScaleCtrl;
        GLUI_Translation* mViewTranslationCtrl;
        GLUI* mBottomGLUIWindow;
        GLUI_StaticText* mStatusText;
        GLUI_StaticText* mProgressText;
        const int mIdReset;
        const int mIdExit;
        const int mIdClearCookies;
        const int mIdEnableCookies;
        const int mIdEnableJavaScript;
        const int mIdEnablePlugins;
        const int mIdBookmarks;
        const int mIdGeomTypeFlat;
        const int mIdGeomTypeSphere;
        const int mIdUrlEdit;
        const int mIdNavBack;
        const int mIdNavStop;
        const int mIdNavHome;
        const int mIdNavForward;
        const int mIdNavReload;
        const int mIdBrowserWidth;
        const int mIdBrowserHeight;
        const int mIdUpdateTypeRB;
        const int mIdUpdateTypeG;
        const int mIdUpdateTypeApp;
        const int mIdSaveSettings;
        int mSelBookmark;
        int mBrowserWidth;
        int mBrowserHeight;
        int mCurObjType;
        int mEnableCookies;
        int mEnableJavaScript;
        int mEnablePlugins;
        std::vector< std::pair< std::string, std::string > > mBookmarks;
        const std::string mHomePageURL = "http://google.com";
        const std::string mSettingsFilename = ".\\settings.txt";

        // picking textures and current mouse pos on geometry
        GLuint mRedBlueTexture;
        unsigned char mRedBlueTexturePixels[256 * 256 * 3];
        GLuint mGreenTexture;
        unsigned char mGreenTexturePixels[16 * 16 * 3];
        GLuint mAppTexture;
        unsigned char* mAppTexturePixels;
        unsigned char mPixelColorRB[3];
        unsigned char mPixelColorG[3];
        int mCurMouseX;
        int mCurMouseY;

        // used to place geometry - updated by GLUI
        float mViewPos[3];
        float mViewRotation[16];
        GLfloat mRotX;
        GLfloat mRotY;
        GLfloat mRotZ;

        // debugging switch - more spam as it gets bigger - 0 for nothing
        const unsigned int mVerboseLevel = 1;
};

#endif // _UBROWSERDLL