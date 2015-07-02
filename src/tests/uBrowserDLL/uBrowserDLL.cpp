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

#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <windows.h>

#include "../../llCEFLib.h"

#include "uBrowserDLL.h"

// connects GLUI callbacks to the app class
uBrowser* gInstance = 0;

////////////////////////////////////////////////////////////////////////////////
//
uBrowser::uBrowser() :
    mRotX(180.0f),
    mRotY(0.0f),
    mRotZ(0.0f),
    mTextureWidth(0),
    mTextureHeight(0),
    mTextureScaleX(0.0f),
    mTextureScaleY(0.0f),
    mViewportAspect(0.0f),
    mIdReset(0x0010),
    mIdBookmarks(0x0011),
    mIdExit(0x0012),
    mIdClearCookies(0x0030),
    mIdEnableCookies(0x0031),
    mIdEnableJavaScript(0x0032),
    mIdEnablePlugins(0x0039),
    mIdUrlEdit(0x0013),
    mIdNavBack(0x0014),
    mIdNavStop(0x0015),
    mIdNavHome(0x0016),
    mIdNavForward(0x0017),
    mIdNavReload(0x001b),
    mIdBrowserWidth(0x0067),
    mIdBrowserHeight(0x0068),
    mIdUpdateTypeRB(0x0100),
    mIdUpdateTypeG(0x0101),
    mIdUpdateTypeApp(0x0102),
    mIdGeomTypeFlat(0x0201),
    mIdGeomTypeSphere(0x202),
    mIdSaveSettings(0x404),
    mStatusText(0),
    mBrowserWidth(900),
    mBrowserHeight(1024),
    mEnableCookies(1),
    mEnableJavaScript(1),
    mEnablePlugins(1),
    mProgressText(0)
{
    // use this to connect GLUI callbacks to this class
    gInstance = this;

    mLLCEFLib = new LLCEFLib();
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::init(int glutWindow)
{
    // save the GLUT window handle since we need to reset it sometimes.
    // (GLUT/GLUI dependency)
    mAppWindow = glutWindow;

    // sites to test
    mBookmarks.push_back(std::pair< std::string, std::string >("Select a bookmark", ""));
    mBookmarks.push_back(std::pair< std::string, std::string >("Locale", "http://jsfiddle.net/cajeamx9/"));
    mBookmarks.push_back(std::pair< std::string, std::string >("Chrome Version", "chrome://version"));
    mBookmarks.push_back(std::pair< std::string, std::string >("Google News", "http://www.news.google.com"));
    mBookmarks.push_back(std::pair< std::string, std::string >("Second Life Home Page", "http://www.secondlife.com"));
    mBookmarks.push_back(std::pair< std::string, std::string >("Linden Lab Home Page", "http://www.lindenlab.com"));
    mBookmarks.push_back(std::pair< std::string, std::string >("YouTube Home", "http://www.youtube.com"));
    mBookmarks.push_back(std::pair< std::string, std::string >("Flash game", "http://www.shooter-bubble.com/"));
    mBookmarks.push_back(std::pair< std::string, std::string >("Altered Qualia WebGL", "http://www.alteredqualia.com"));
    mBookmarks.push_back(std::pair< std::string, std::string >("Callum's WebGL", "http://www.callum.com"));
    mBookmarks.push_back(std::pair< std::string, std::string >("User Agent Info", "http://whatsmyuseragent.com"));
    mBookmarks.push_back(std::pair< std::string, std::string >("HTML5 Compliance test", "http://html5test.com"));
    mBookmarks.push_back(std::pair< std::string, std::string >("HTTP Auth test", "http://www.pagetutor.com/keeper/http_authentication/index.html"));
    mBookmarks.push_back(std::pair< std::string, std::string >("secondlife:Teleport to Ahern", "secondlife:///app/teleport/Ahern/128/128/0/"));
    mBookmarks.push_back(std::pair< std::string, std::string >("secondlife:Resident bio", "secondlife:///app/agent/69bfa8f2-ce10-46fe-b742-490e077d4c08/about"));
    mBookmarks.push_back(std::pair< std::string, std::string >("Non secondlife scheme", "flasm:///foo/bar/wibble"));

    // create the red/blue texture used in picking
    glGenTextures(1, &mRedBlueTexture);
    for(int y = 0; y < 256; ++y)
    {
        for(int x = 0; x < 256 * 3; x += 3)
        {
            mRedBlueTexturePixels[y * 256 * 3 + x + 0] = (x / 3);
            mRedBlueTexturePixels[y * 256 * 3 + x + 1] = 0;
            mRedBlueTexturePixels[y * 256 * 3 + x + 2] = y;
        };
    }
    glBindTexture(GL_TEXTURE_2D, mRedBlueTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, mRedBlueTexturePixels);

    // create the green texture used in picking
    glGenTextures(1, &mGreenTexture);
    for(int j = 0; j < 16 * 16 * 3; j += 3)
    {
        mGreenTexturePixels[j + 0] = 0;
        mGreenTexturePixels[j + 1] = (unsigned char)(j / 3);
        mGreenTexturePixels[j + 2] = 0;
    };
    glBindTexture(GL_TEXTURE_2D, mGreenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 16, 16, 0, GL_RGB, GL_UNSIGNED_BYTE, mGreenTexturePixels);

    // start with the flat rectangle so we don't freak people out :)
    mCurObjType = mIdGeomTypeFlat;

    // initial size of the browser window
    setSize(mBrowserWidth, mBrowserHeight);

    // load settings from disk
    loadSettings(mSettingsFilename);

    // generate the UI
    makeChrome();

    // reset rotation, translation, scale etc.
    resetView();

    // hook up callbacks for events we want to act on
    mLLCEFLib->setPageChangedCallback(std::bind(&uBrowser::pageChangedCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    mLLCEFLib->setOnCustomSchemeURLCallback(std::bind(&uBrowser::onCustomSchemeURLCallback, this, std::placeholders::_1));
    mLLCEFLib->setOnConsoleMessageCallback(std::bind(&uBrowser::onConsoleMessageCallback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    mLLCEFLib->setOnStatusMessageCallback(std::bind(&uBrowser::onStatusMessageCallback, this, std::placeholders::_1));
    mLLCEFLib->setOnTitleChangeCallback(std::bind(&uBrowser::onTitleChangeCallback, this, std::placeholders::_1));

    // initialize CEF lib and navigate to home page
    LLCEFLibSettings settings;
    settings.inital_width = mBrowserWidth;
    settings.inital_height = mBrowserHeight;
    settings.javascript_enabled = mEnableJavaScript ? true: false;
    settings.plugins_enabled = mEnablePlugins ? true : false;
    settings.cookies_enabled = mEnableCookies ? true : false;
    settings.user_agent_substring = "uBrowser Test";
    bool result = mLLCEFLib->init(settings);
    if(result)
    {
        mLLCEFLib->navigate(mHomePageURL);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::pageChangedCallback(unsigned char* pixels, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, mAppTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);

    // GLUI requires this
    if(glutGetWindow() != mAppWindow)
        glutSetWindow(mAppWindow);

    glutPostRedisplay();

    if(mVerboseLevel > 1)
    {
        std::cout << "LOG> page contents changed" << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onCustomSchemeURLCallback(std::string url)
{
    if(mVerboseLevel > 0)
    {
        std::cout << "Custom scheme URL triggered: " << url << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onConsoleMessageCallback(std::string message, std::string source, int line)
{
    if(mVerboseLevel > 3)
    {
        std::cout << "Console message " << message << " in file (" << source << ") at line " << line << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onStatusMessageCallback(std::string value)
{
    if(mVerboseLevel > 3)
    {
        std::cout << "Status text " << value << std::endl;
    }
    mStatusText->set_text(value.c_str());
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::onTitleChangeCallback(std::string title)
{
    if(mVerboseLevel > 2)
    {
        std::cout << "Page title changed to " << title << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::drawGeometry(int geomTypeIn, int updateTypeIn)
{
    // plain old flat rectangle
    if(geomTypeIn == mIdGeomTypeFlat)
    {
        glEnable(GL_TEXTURE_2D);   // TODO need ?
        glColor3f(1.0f, 1.0f, 1.0f);

        if(updateTypeIn == mIdUpdateTypeG)
            glBindTexture(GL_TEXTURE_2D, mGreenTexture);
        else if(updateTypeIn == mIdUpdateTypeApp)
            glBindTexture(GL_TEXTURE_2D, mAppTexture);

        GLfloat aspect_ratio = (GLfloat)mBrowserWidth / (GLfloat)mBrowserHeight;
        GLfloat x_size = 1.0f;
        GLfloat y_size = x_size / aspect_ratio;
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-x_size, -y_size, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(x_size, -y_size, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(x_size, y_size, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-x_size, y_size, 0.0f);
        glEnd();
    }
    if(geomTypeIn == mIdGeomTypeSphere)
    {
        if(updateTypeIn == mIdUpdateTypeG)
            glBindTexture(GL_TEXTURE_2D, mGreenTexture);
        else if(updateTypeIn == mIdUpdateTypeApp)
            glBindTexture(GL_TEXTURE_2D, mAppTexture);

        glEnable(GL_TEXTURE_2D);
        glColor3f(1.0f, 1.0f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        const int numElems = 80;
        const GLfloat  piDiv2 = 1.57079632679489661923f;
        const GLfloat piMul2 = 6.28318530717958647692f;

        for(int j = 0; j < numElems / 2; ++j)
        {
            GLfloat theta1 = j * (piMul2) / numElems - piDiv2;
            GLfloat theta2 = (j + 1) * (piMul2) / numElems - piDiv2;

            glBegin(GL_TRIANGLE_STRIP);

            for(int i = 0; i <= numElems; i++)
            {
                GLfloat theta3 = i * piMul2 / numElems;

                GLfloat pointX = cos(theta2) * cos(theta3);
                GLfloat pointY = sin(theta2);
                GLfloat pointZ = cos(theta2) * sin(theta3);

                glNormal3f(pointX, pointY, pointZ);
                glTexCoord2f(i / (GLfloat)numElems, 2 * (j + 1) / (GLfloat)numElems);
                glVertex3f(pointX, pointY, pointZ);

                
                pointX = cos(theta1) * cos(theta3);
                pointY = sin(theta1);
                pointZ = cos(theta1) * sin(theta3);

                
                glNormal3f(pointX, pointY, pointZ);
                glTexCoord2f(i / (GLfloat)numElems, 2 * j / (GLfloat)numElems);
                glVertex3f(pointX, pointY, pointZ);
            };
            glEnd();
        };
    }
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::makeChrome()
{
    // top UI bar
    mTopGLUIWindow = GLUI_Master.create_glui_subwindow(mAppWindow, GLUI_SUBWINDOW_TOP);

    mNavBackButton = mTopGLUIWindow->add_button("<<<", mIdNavBack, gluiCallbackWrapper);
    mNavBackButton->set_w(56);

    mTopGLUIWindow->add_column(false);
    GLUI_Button* button = mTopGLUIWindow->add_button("STOP", mIdNavStop, gluiCallbackWrapper);
    button->set_w(56);

    mTopGLUIWindow->add_column(false);
    button = mTopGLUIWindow->add_button("HOME", mIdNavHome, gluiCallbackWrapper);
    button->set_w(56);

    mTopGLUIWindow->add_column(false);
    mNavForwardButton = mTopGLUIWindow->add_button(">>>", mIdNavForward, gluiCallbackWrapper);
    mNavForwardButton->set_w(56);

    mTopGLUIWindow->add_column(false);
    GLUI_Button* mNavReloadButton = mTopGLUIWindow->add_button("Reload", mIdNavReload, gluiCallbackWrapper);
    mNavReloadButton->set_w(56);

    mTopGLUIWindow->add_column(false);

    mUrlEdit = mTopGLUIWindow->add_edittext("Url:", mNavUrl, mIdUrlEdit, gluiCallbackWrapper);

    mTopGLUIWindow->add_column(false);
    GLUI_Listbox* bookmarkList = mTopGLUIWindow->add_listbox("Bookmarks", &mSelBookmark, mIdBookmarks, gluiCallbackWrapper);
    for(unsigned int each = 0; each < mBookmarks.size(); ++each)
    {
        bookmarkList->add_item(each, const_cast< char* >(mBookmarks[each].first.c_str()));
    };

    mTopGLUIWindow->set_main_gfx_window(mAppWindow);
    
    // top UI bar (second part)
    mTop2GLUIWindow = GLUI_Master.create_glui_subwindow(mAppWindow, GLUI_SUBWINDOW_TOP);
    mTop2GLUIWindow->add_checkbox("Enable Cookies", &mEnableCookies, mIdEnableCookies, gluiCallbackWrapper);
    mTop2GLUIWindow->add_column(false);
    button = mTop2GLUIWindow->add_button("Clear Cookies", mIdClearCookies, gluiCallbackWrapper);
    button->set_w(64);
    mTop2GLUIWindow->add_column(false);
    mTop2GLUIWindow->add_checkbox("Enable JavaScript", &mEnableJavaScript, mIdEnableJavaScript, gluiCallbackWrapper);
    mTop2GLUIWindow->add_column(false);
    mTop2GLUIWindow->add_checkbox("Enable Plugins", &mEnablePlugins, mIdEnablePlugins, gluiCallbackWrapper);
    mTop2GLUIWindow->set_main_gfx_window(mAppWindow);
    mTop2GLUIWindow->add_column(false);
    mTop2GLUIWindow->add_spinner("Browser width", GLUI_SPINNER_INT, &mBrowserWidth, mIdBrowserWidth, gluiCallbackWrapper);
    mTop2GLUIWindow->add_column(false);
    mTop2GLUIWindow->add_spinner("Browser height", GLUI_SPINNER_INT, &mBrowserHeight, mIdBrowserHeight, gluiCallbackWrapper);
    mTop2GLUIWindow->add_column(false);
    button = mTop2GLUIWindow->add_button("Save settings", mIdSaveSettings, gluiCallbackWrapper);
    button->set_w(64);

    // bottom UI bar
    mBottomGLUIWindow = GLUI_Master.create_glui_subwindow(mAppWindow, GLUI_SUBWINDOW_BOTTOM);
    mStatusText = mBottomGLUIWindow->add_statictext("");
    mBottomGLUIWindow->add_column(false);
    mProgressText = mBottomGLUIWindow->add_statictext("");
    mProgressText->set_alignment(GLUI_ALIGN_RIGHT);
    mBottomGLUIWindow->set_main_gfx_window(mAppWindow);
    mRightGLUIWindow = GLUI_Master.create_glui_subwindow(mAppWindow, GLUI_SUBWINDOW_RIGHT);

    // object selector
    GLUI_Listbox* objTypelist = mRightGLUIWindow->add_listbox("Type ", &mCurObjType);
    objTypelist->add_item(mIdGeomTypeFlat, "Flat");
    objTypelist->add_item(mIdGeomTypeSphere, "Sphere");
    mRightGLUIWindow->add_statictext("");
    mViewRotationCtrl = mRightGLUIWindow->add_rotation("Rotation", mViewRotation);
    mViewTranslationCtrl = mRightGLUIWindow->add_translation("Translate", GLUI_TRANSLATION_XY, mViewPos);
    mViewTranslationCtrl->set_speed(0.01f);
    mViewScaleCtrl = mRightGLUIWindow->add_translation("Scale", GLUI_TRANSLATION_Z, &mViewPos[2]);
    mViewScaleCtrl->set_speed(0.05f);
    mRightGLUIWindow->add_statictext("");
    button = mRightGLUIWindow->add_button("Reset", mIdReset, gluiCallbackWrapper);
    button->set_w(64);
    mRightGLUIWindow->add_statictext("");
    button = mRightGLUIWindow->add_button("Exit", mIdExit, gluiCallbackWrapper);
    button->set_w(64);
    mRightGLUIWindow->set_main_gfx_window(mAppWindow);
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::setSize(int widthIn, int heightIn)
{
    // calculate the next power of 2 bigger than reqquested size for width and height
    for(mTextureWidth = 1; mTextureWidth < widthIn; mTextureWidth <<= 1)
    {
    };

    for(mTextureHeight = 1; mTextureHeight < heightIn; mTextureHeight <<= 1)
    {
    };

    // save the height and width
    mBrowserWidth = widthIn;
    mBrowserHeight = heightIn;

    // we scale all textures by this much so that they fit the geometry
    mTextureScaleX = (GLfloat)mBrowserWidth / (GLfloat)mTextureWidth;
    mTextureScaleY = (GLfloat)mBrowserHeight / (GLfloat)mTextureHeight;

    // delete the old texture handle and create a new one
    if(mAppTexture)
        glDeleteTextures(1, &mAppTexture);
    glGenTextures(1, &mAppTexture);
    glBindTexture(GL_TEXTURE_2D, mAppTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mTextureWidth, mTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    mLLCEFLib->setSize(mBrowserWidth, mBrowserHeight);
}

////////////////////////////////////////////////////////////////////////////////
//
bool uBrowser::saveSettings(std::string filename)
{
    std::ofstream file_handle(filename.c_str());
    if (!file_handle.is_open())
    {
        return false;
    };

    file_handle << "enable_plugins=" << mEnablePlugins << std::endl;
    file_handle << "enable_javascript=" << mEnableJavaScript << std::endl;
    file_handle << "enable_cookies=" << mEnableCookies << std::endl;

    file_handle.close();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
//
bool uBrowser::loadSettings(std::string filename)
{
    std::ifstream file_handle(filename.c_str());
    if (!file_handle.is_open())
    {
        return false;
    };

    std::string line;
    while (!file_handle.eof())
    {
        std::getline(file_handle, line);

        if (file_handle.eof())
        {
            break;
        };

        std::size_t split_pos = line.find("=");
        std::string name = line.substr(0, split_pos);
        std::string value = line.substr(split_pos + 1);
        std::istringstream codec(value);

        if (name == "enable_plugins") codec >> mEnablePlugins;
        if (name == "enable_javascript") codec >> mEnableJavaScript;
        if (name == "enable_cookies") codec >> mEnableCookies;
    };

    file_handle.close();
    return true;
}


////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::resetView()
{
    mViewRotationCtrl->reset();

    mRotX = 180.0f;
    mRotY = 0.0f;
    mRotZ = 0.0f;

    mViewScaleCtrl->set_x(0.0f);
    mViewScaleCtrl->set_y(0.0f);
    mViewScaleCtrl->set_z(0.0f);

    mViewTranslationCtrl->set_x(0.0f);
    mViewTranslationCtrl->set_y(0.0f);
    mViewTranslationCtrl->set_z(0.0f);
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::gluiCallback(int controlIdIn)
{
    if(controlIdIn == mIdExit)
    {
        mLLCEFLib->reset();

        // exit the main GLUT loop (freeglut feature)
        glutLeaveMainLoop();
    }
    if(controlIdIn == mIdBookmarks)
    {
        if(mVerboseLevel > 0)
        {
            std::cout << "Navigate to: " << mBookmarks[mSelBookmark].second << std::endl;
        }

        mLLCEFLib->navigate(mBookmarks[mSelBookmark].second);
    }
    else if (controlIdIn == mIdNavBack)
    {
        if (mVerboseLevel > 0)
        {
            std::cout << "Navigate back" << std::endl;
        }

        mLLCEFLib->goBack();
    }
    else if (controlIdIn == mIdNavStop)
    {
        if (mVerboseLevel > 0)
        {
            std::cout << "Navigate stop" << std::endl;
        }

        mLLCEFLib->stop();
    }
    else if (controlIdIn == mIdNavHome)
    {
        if (mVerboseLevel > 0)
        {
            std::cout << "Navigate home" << std::endl;
        }

        mLLCEFLib->navigate(mHomePageURL);
    }
    else if (controlIdIn == mIdNavForward)
    {
        if (mVerboseLevel > 0)
        {
            std::cout << "Navigate forward" << std::endl;
        }

        mLLCEFLib->goForward();
    }
    else if (controlIdIn == mIdNavReload)
    {
        if (mVerboseLevel > 0)
        {
            std::cout << "Navigate reload" << std::endl;
        }

        // always ignore cache (true) for this test
        mLLCEFLib->reload(true);
    }
    else if(controlIdIn == mIdBrowserWidth || controlIdIn == mIdBrowserHeight)
    {
        setSize(mBrowserWidth, mBrowserHeight);

        if(mVerboseLevel > 2)
        {
            std::cout << "Browser dimensions changed to: " << mBrowserWidth << " x " << mIdBrowserHeight << std::endl;
        }
    }
    else if(controlIdIn == mIdReset)
    {
        if(glutGetWindow() != mAppWindow)
            glutSetWindow(mAppWindow);
        resetView();
        glutPostRedisplay();
    }
    else if(controlIdIn == mIdClearCookies)
    {
        // clear cookie code will go here
        std::cout << "Cookies cleared" << std::endl;
    }
    else if(controlIdIn == mIdEnableCookies)
    {
        std::cout << "Cookie state set to " << mEnableCookies << std::endl;
        std::cout << "Settings saved - restart required" << std::endl;
        saveSettings(mSettingsFilename);
    }
    else if(controlIdIn == mIdEnableJavaScript)
    {
        std::cout << "JavaScript enabled state set to " << mEnableJavaScript << std::endl;
        std::cout << "Settings saved - restart required" << std::endl;
        saveSettings(mSettingsFilename);
    }
    else if (controlIdIn == mIdEnablePlugins)
    {
        std::cout << "Plugins enabled state set to " << mEnablePlugins << std::endl;
        std::cout << "Settings saved - restart required" << std::endl;
        saveSettings(mSettingsFilename);
    }
    else if (controlIdIn == mIdSaveSettings)
    {
        std::cout << "Saving settings to " << mSettingsFilename << std::endl;
        saveSettings(mSettingsFilename);
    }
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::reshape(int widthIn, int heightIn)
{
    // save these as we'll need them later
    mWindowWidth = widthIn;
    mWindowHeight = heightIn;

    // just a rough calculation
    mUrlEdit->set_w(mWindowWidth - 700);
    mStatusText->set_w(mWindowWidth - 100);

    // update viewport (the active window inside the chrome stuff)
    int viewportX, viewportY;
    int viewportHeight, viewportWidth;
    GLUI_Master.get_viewport_area(&viewportX, &viewportY, &viewportWidth, &viewportHeight);
    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);

    // need this when we come to display
    mViewportAspect = (float)(viewportWidth) / (float)(viewportHeight);

    // GLUI requires this
    if(glutGetWindow() != mAppWindow)
        glutSetWindow(mAppWindow);

    // trigger re-display
    glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::display()
{
    // let's start with a clean slate
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // scale all texture so that they fit the geometry exactly
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glScalef(mTextureScaleX, mTextureScaleY, 1.0f);

    // set up OpenGL view
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-mViewportAspect * 0.04f, mViewportAspect * 0.04f, -0.04f, 0.04f, 0.1f, 50.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.0f);
    glTranslatef(mViewPos[0], mViewPos[1], -mViewPos[2]);
    glMultMatrixf(mViewRotation);

    static GLfloat localRotX = 180.f;
    static GLfloat localRotY = 0.f;
    static GLfloat localRotZ = 0.f;

    if(fabs(localRotX - mRotX) > 0.1f)
        localRotX += (mRotX - localRotX) / 25.0f;

    if(fabs(localRotY - mRotY) > 0.1f)
        localRotY += (mRotY - localRotY) / 25.0f;

    if(fabs(localRotZ - mRotZ) > 0.1f)
        localRotZ += (mRotZ - localRotZ) / 25.0f;

    glRotatef(localRotX, 1.0f, 0.0f, 0.0f);
    glRotatef(localRotY, 0.0f, 1.0f, 0.0f);
    glRotatef(localRotZ, 0.0f, 0.0f, 1.0f);
    glScalef(1.0f, 1.0f, 1.0f);

    // red blue pattern
    glBindTexture(GL_TEXTURE_2D, mRedBlueTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    drawGeometry(mCurObjType, mIdUpdateTypeRB);

    // read colors and get red/blue value
    glReadPixels(mCurMouseX, mCurMouseY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, mPixelColorRB);

    // green texture mask
    glBindTexture(GL_TEXTURE_2D, mGreenTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glScalef(mTextureScaleX * 256.0f, mTextureScaleX * 256.0f, 1.0f);   // scale the scale by the scale :)
    drawGeometry(mCurObjType, mIdUpdateTypeG);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // read colors and get green value
    glReadPixels(mCurMouseX, mCurMouseY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, mPixelColorG);

    
    // draw the browser texture
    drawGeometry(mCurObjType, mIdUpdateTypeApp);

    // restore matrix after texture scale
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();

    glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::idle()
{
    mLLCEFLib->update();
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::windowPosToTexturePos(int winXIn, int winYIn, int& texXOut, int& texYOut)
{
    // force a redraw so that red/blue and green pick textures get re-picked
    display();

    // this is how we convert from the color in the textures draw in the first 2 passes to an
    // XY location in the coordinate space of the texture
    mCurMouseX = winXIn;
    mCurMouseY = mWindowHeight - winYIn;    // opposite Y coordinate systems..

    // red gives 0..255 on X and blue value gives 0.255 on Y
    // green divides each discrete values by 16 giving 256 * 16 (4096) resolution in each of X & Y
    texXOut = (mPixelColorRB[0] * 16 + (mPixelColorG[1] & 0x0f)) / (4096 / mTextureWidth);
    texYOut = (mPixelColorRB[2] * 16 + (mPixelColorG[1] >> 4)) / (4096 / mTextureHeight);

    // GLUI requires this
    if(glutGetWindow() != mAppWindow)
        glutSetWindow(mAppWindow);

    glutPostRedisplay();
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::keyboard(int keyIn, int xIn, int yIn)
{

}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::specialKeyboard(int keyIn, int xIn, int yIn)
{

}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::passiveMouse(int xIn, int yIn)
{
    // called when the mouse is moving and button isn't pressed
    int x, y;
    windowPosToTexturePos(xIn, yIn, x, y);

    mLLCEFLib->mouseMove(x, y);

    if(mVerboseLevel > 1)
    {
        std::cout << "LOG> (Passive) mouse move to " << x << ", " << y << std::endl;
    }
}

void uBrowser::mouseMove(int xIn, int yIn)
{
    // called when mouse is moving and button is pressed
    int x, y;
    windowPosToTexturePos(xIn, yIn, x, y);

    mLLCEFLib->mouseMove(x, y);

    if(mVerboseLevel > 1)
    {
        std::cout << "LOG> mouse move to " << x << ", " << y << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
void uBrowser::mouseButton(int buttonIn, int stateIn, int xIn, int yIn)
{
    int x, y;
    windowPosToTexturePos(xIn, yIn, x, y);

	EMouseButton btn;
	if (buttonIn == GLUT_LEFT_BUTTON) btn = MB_MOUSE_BUTTON_LEFT;
	if (buttonIn == GLUT_RIGHT_BUTTON) btn = MB_MOUSE_BUTTON_RIGHT;
	if (buttonIn == GLUT_MIDDLE_BUTTON) btn = MB_MOUSE_BUTTON_MIDDLE;

	EMouseEvent ev;
	if (stateIn == GLUT_DOWN) ev = ME_MOUSE_DOWN;
	if (stateIn == GLUT_UP) ev = ME_MOUSE_UP;

	mLLCEFLib->mouseButton(btn, ev, x, y);

	if (mVerboseLevel > 1) 
		std::cout << "LOG> Mouse button " << btn << " event " << ev << " at " <<  x << ", " << y << std::endl;
}

////////////////////////////////////////////////////////////////////////////////
//
static void gluiCallbackWrapper(int controlIdIn)
{
    // dispatch GLUI callbacks into my class cos I like it that way :)
    if(gInstance)
        gInstance->gluiCallback(controlIdIn);
}


uBrowser* theApp;

////////////////////////////////////////////////////////////////////////////////
//
void glutReshape(int widthIn, int heightIn)
{
    if(theApp)
        theApp->reshape(widthIn, heightIn);
};

////////////////////////////////////////////////////////////////////////////////
//
void glutDisplay()
{
    if(theApp)
        theApp->display();
};

////////////////////////////////////////////////////////////////////////////////
//
void glutIdle()
{
    if(theApp)
        theApp->idle();
};

////////////////////////////////////////////////////////////////////////////////
//
void glutKeyboard(unsigned char keyIn, int xIn, int yIn)
{
    if(theApp)
        theApp->keyboard(keyIn, xIn, yIn);
};

////////////////////////////////////////////////////////////////////////////////
//
void glutSpecialKeyboard(int keyIn, int xIn, int yIn)
{
    if(theApp)
        theApp->specialKeyboard(keyIn, xIn, yIn);
};

////////////////////////////////////////////////////////////////////////////////
//
void glutPassiveMouse(int xIn, int yIn)
{
    if(theApp)
        theApp->passiveMouse(xIn, yIn);
}

////////////////////////////////////////////////////////////////////////////////
//
void glutMouseMove(int xIn, int yIn)
{
    if(theApp)
        theApp->mouseMove(xIn, yIn);
}

////////////////////////////////////////////////////////////////////////////////
//
void glutMouseButton(int buttonIn, int stateIn, int xIn, int yIn)
{
    if(theApp)
        theApp->mouseButton(buttonIn, stateIn, xIn, yIn);
}

////////////////////////////////////////////////////////////////////////////////
//
int uBrowserApp()
{
    theApp = new uBrowser;

    if(theApp)
    {
        int argc = 1;
        glutInit(&argc, NULL);
        glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

        glutInitWindowPosition(80, 0);
        glutInitWindowSize(1500, 1250);

        int appWindow = glutCreateWindow("uBrowserDLL");

        glutDisplayFunc(glutDisplay);

        GLUI_Master.set_glutReshapeFunc(glutReshape);
        GLUI_Master.set_glutKeyboardFunc(glutKeyboard);
        GLUI_Master.set_glutMouseFunc(glutMouseButton);
        GLUI_Master.set_glutSpecialFunc(glutSpecialKeyboard);

        glutPassiveMotionFunc(glutPassiveMouse);
        glutMotionFunc(glutMouseMove);

        glutSetWindow(appWindow);
        
        theApp->init(appWindow);

        GLUI_Master.set_glutIdleFunc(glutIdle);

        glutMainLoop();
        delete theApp;
    };

    return 1;
}

////////////////////////////////////////////////////////////////////////////////
//
extern "C" int __declspec(dllexport) __cdecl startuBrowser()
{
    return uBrowserApp();
}

////////////////////////////////////////////////////////////////////////////////
//
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    return TRUE;
}