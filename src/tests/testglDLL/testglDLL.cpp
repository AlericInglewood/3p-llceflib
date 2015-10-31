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
#include "boost/function.hpp"
#include "boost/bind.hpp"

#include "../../llCEFLib.h"

#ifdef WIN32
#include "GL/freeglut.h"
#elif __APPLE__
#include <GLUT/glut.h>
#endif

class LLMediaSimpleTest
{
    public:
		LLMediaSimpleTest() :
			mBrowserWidth(1024),
			mBrowserHeight(1024),
			mBrowserDepth(4),
			mAppTexture(0),
			mHomepageURL("https://callum-linden.s3.amazonaws.com/ceftests.html")
        {
            mLLCEFLib = new LLCEFLib();
        };

        ~LLMediaSimpleTest()
        {
        };

        bool init(int init_width, int init_height)
        {
			std::cout << "TestGL(DLL) starting - version:" << LLCEFLIB_VERSION << std::endl;
            mLLCEFLib->setOnPageChangedCallback(boost::bind(&LLMediaSimpleTest::onPageChangedCallback, this, _1, _2, _3));
            mLLCEFLib->setOnCustomSchemeURLCallback(boost::bind(&LLMediaSimpleTest::onCustomSchemeURLCallback, this, _1));
            mLLCEFLib->setOnConsoleMessageCallback(boost::bind(&LLMediaSimpleTest::onConsoleMessageCallback, this, _1, _2, _3));
            mLLCEFLib->setOnStatusMessageCallback(boost::bind(&LLMediaSimpleTest::onStatusMessageCallback, this, _1));
			mLLCEFLib->setOnTitleChangeCallback(boost::bind(&LLMediaSimpleTest::onTitleChangeCallback, this, _1));
			mLLCEFLib->setOnAddressChangeCallback(boost::bind(&LLMediaSimpleTest::onAddressChangeCallback, this, _1));
			mLLCEFLib->setOnLoadStartCallback(boost::bind(&LLMediaSimpleTest::onLoadStartCallback, this));
			mLLCEFLib->setOnLoadEndCallback(boost::bind(&LLMediaSimpleTest::onLoadEndCallback, this, _1));
			mLLCEFLib->setOnNavigateURLCallback(boost::bind(&LLMediaSimpleTest::onNavigateURLCallback, this, _1, _2));
			mLLCEFLib->setOnHTTPAuthCallback(boost::bind(&LLMediaSimpleTest::onHTTPAuthCallback, this, _1, _2, _3, _4));
			mLLCEFLib->setOnRequestExitCallback(boost::bind(&LLMediaSimpleTest::onRequestExitCallback, this));

			LLCEFLib::LLCEFLibSettings settings;
            settings.initial_width = mBrowserWidth;
            settings.initial_height = mBrowserHeight;
            settings.javascript_enabled = true;
            settings.cookies_enabled = true;
			settings.accept_language_list = "mi-wwow";
			settings.cookies_enabled = true;
			settings.cookie_store_path = "C:\\browser_cookies";
			settings.cache_enabled = true;
			settings.cache_path = "C:\\browser_cache";
			settings.user_agent_substring = "SecondLife";


            bool result = mLLCEFLib->init(settings);
            if(result)
            {
				// default is secondlife:// but let's test it...
                std::vector<std::string> custom_schemes;
                custom_schemes.push_back("secondlife://");
                custom_schemes.push_back("flasm://");
				mLLCEFLib->setCustomSchemes(custom_schemes);
                mLLCEFLib->navigate(mHomepageURL);
				mLLCEFLib->setFocus(true);
            }

            return result;
        }

        void onPageChangedCallback(unsigned char* pixels, int width, int height)
        {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
            glutPostRedisplay();
        }

        void onCustomSchemeURLCallback(std::string url)
        {
            std::cout << "Custom scheme URL triggered: " << url << std::endl;
        }

        void onConsoleMessageCallback(std::string message, std::string source, int line)
        {
            std::cout << "TestGL - console message " << message << " in file (" << source << ") at line " << line << std::endl;
        }

        void onStatusMessageCallback(std::string value)
        {
            std::cout << "TestGL - status changed to " << value << std::endl;
        }

		void onTitleChangeCallback(std::string title)
		{
			std::cout << "TestGL - title changed to " << title << std::endl;
		}

		void onAddressChangeCallback(std::string new_url)
		{
			std::cout << "TestGL - URL changed to " << new_url << std::endl;
		}

		void onLoadStartCallback()
		{
			std::cout << "TestGL - Load started" << std::endl;
		}

		void onRequestExitCallback()
		{
			std::cout << "TestGL - exit requested after cleanup" << std::endl;
#ifdef WIN32
            glutLeaveMainLoop();
#elif __APPLE__
            /// wrong wrong wrong exit(0);
#endif
		}

		void onLoadEndCallback(int httpStatusCode)
		{
			std::cout << "TestGL - Load ended with HTTP status code of " << httpStatusCode << std::endl;
		}

		void onNavigateURLCallback(std::string url, std::string target)
		{
			std::cout << "TestGL - navigate to " << url << " with a target of " << target << std::endl;
		}

		bool onHTTPAuthCallback(const std::string host, const std::string realm, std::string& username, std::string& password)
		{
			std::cout << "--------- HTTP AUTH TEST ---------" << std::endl;
			std::cout << "Host is " << host << std::endl;
			std::cout << "Realm is " << realm << std::endl;
			std::cout << "----------------------------------" << std::endl;

			//// Windows only testing code
			//int msgboxID = MessageBox(
			//	NULL,
			//	L"Shall I enter the password for you?",
			//	L"Enter Password ?",
			//	MB_ICONEXCLAMATION | MB_YESNO
			//	);

			//if (msgboxID == IDYES)
			//{
			//	username = "user";
			//	password = "passwd";
			//	return true; // username/password and "OKAY" entered in HTTP Auth dialog
			//}
			//else
			//{
			//	return false; // cancel pressed in HTTP Auth dialog
			//}

			// for testing this URL where the endpoint contains username and password
			// and password entered CORRECTLY
			// http://httpbin.org/basic-auth/user/passwd
			username = "user";
			password = "passwd";
			return true; // username/password and "OKAY" entered in HTTP Auth dialog

			// for testing CANCEL
			//return false; // cancel pressed in HTTP Auth dialog

			//// for testing this URL where the endpoint contains username and password
			//// and password entered INCORRECTLY
			//// http://httpbin.org/basic-auth/user/passwd
			//username = "foobar";
			//password = "flasmsasm";
			//return true; // username/password and "OKAY" entered in HTTP Auth dialog
		}

		void display()
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glColor3f(1.0f, 1.0f, 1.0f);
            glBegin(GL_QUADS);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2d(mBrowserWidth, 0);
            glTexCoord2f(0.0f, 1.0f);
            glVertex2d(0, 0);
            glTexCoord2f(0.0f, 0.0f);
            glVertex2d(0, mBrowserHeight);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2d(mBrowserWidth, mBrowserHeight);
            glEnd();

            glutSwapBuffers();
        }

        void idle()
        {
            mLLCEFLib->update();
        }

		void shutdown()
		{
			mLLCEFLib->shutdown();
		}

        void reshape(int width, int height)
        {
            if(height == 0) height = 1;

            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glViewport(0, 0, width, height);
            glOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            resizeTexture(width, height);
        };

        void mouseButton(int button, int state, int x, int y)
        {
			LLCEFLib::EMouseButton btn = LLCEFLib::MB_MOUSE_BUTTON_LEFT;
			if (button == GLUT_RIGHT_BUTTON) btn = LLCEFLib::MB_MOUSE_BUTTON_RIGHT;
			if (button == GLUT_MIDDLE_BUTTON) btn = LLCEFLib::MB_MOUSE_BUTTON_MIDDLE;

			LLCEFLib::EMouseEvent ev = LLCEFLib::ME_MOUSE_UP;
			if (state == GLUT_DOWN) ev = LLCEFLib::ME_MOUSE_DOWN;
			if (state == GLUT_UP) ev = LLCEFLib::ME_MOUSE_UP;

			mLLCEFLib->mouseButton(btn, ev, x, y);

			//if (state == GLUT_DOWN)
			//	mLLCEFLib->setFocus(true);
        };

        void mouseMove(int x, int y)
        {
            mLLCEFLib->mouseMove(x, y);
        }

        void resizeTexture(int width, int height)
        {
            std::cout << "reshaping - window size is now " << mBrowserWidth << " x " << mBrowserHeight << std::endl;

            mBrowserWidth = width;
            mBrowserHeight = height;

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mBrowserWidth, mBrowserHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

            mLLCEFLib->setSize(width, height);
        }

        void setupOpenGL()
        {
            glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
            glEnable(GL_COLOR_MATERIAL);
            glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
            glEnable(GL_TEXTURE_2D);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glEnable(GL_CULL_FACE);

            glEnable(GL_TEXTURE_2D);
            glDisable(GL_CULL_FACE);
            glDisable(GL_LIGHTING);

            glGenTextures(1, &mAppTexture);
            glBindTexture(GL_TEXTURE_2D, mAppTexture);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            resizeTexture(mBrowserWidth, mBrowserHeight);
        }

        void keyboard(unsigned char key)
        {
			if (key == 27)
			{
				mLLCEFLib->requestExit();
			}

            if(key == '`') mLLCEFLib->navigate(mHomepageURL);
            if(key == '-') mLLCEFLib->goBack();
			if (key == '=') mLLCEFLib->goForward();
			if (key == '\\') mLLCEFLib->editCopy();
			if (key == ']') mLLCEFLib->editPaste();
        }

        void keyboardUp(unsigned char key)
        {
            //mLLCEFLib->keyPress(key, false);
        }

        void mouseWheel(int button, int dir, int x, int y)
        {
#if WIN32
            int delta = 32;
            mLLCEFLib->mouseWheel(0, dir * delta);
#endif
        }

    private:
        int mBrowserWidth;
        int mBrowserHeight;
        int mBrowserDepth;
        std::string mHomepageURL;
        GLuint mAppTexture;
        LLCEFLib* mLLCEFLib;
};

LLMediaSimpleTest* gApplication;

void glutReshape(int width, int height)
{
    if(gApplication)
        gApplication->reshape(width, height);
};

void glutDisplay()
{
    if(gApplication)
        gApplication->display();
};

void glutIdle()
{
    if(gApplication)
        gApplication->idle();
};

void glutKeyboard(unsigned char key, int x, int y)
{
    if(gApplication)
        gApplication->keyboard(key);
};

void glutKeyboardUp(unsigned char key, int x, int y)
{
    if(gApplication)
        gApplication->keyboardUp(key);
};

void glutMouseMove(int x, int y)
{
    if(gApplication)
        gApplication->mouseMove(x, y);
}

void glutMouseButton(int button, int state, int x, int y)
{
    if(gApplication)
        gApplication->mouseButton(button, state, x, y);
}

void glutMouseWheel(int button, int dir, int x, int y)
{
    gApplication->mouseWheel(button, dir, x, y);
}

int testGL()
{
    gApplication = new LLMediaSimpleTest();

    // For some reason, calling glutInit after gApplication->init() resulted in a glut double init error
    // on OSX. 'Solved' by moving glutInit call before gApplication->init.

    int argc = 0;
    glutInit(&argc, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowPosition(750, 0);
    glutInitWindowSize(1024, 1024);

    glutCreateWindow("testGL");

	glutKeyboardFunc(glutKeyboard);
	glutKeyboardUpFunc(glutKeyboardUp);
    glutDisplayFunc(glutDisplay);
    glutIdleFunc(glutIdle);
    glutReshapeFunc(glutReshape);
    glutMouseFunc(glutMouseButton);
    glutPassiveMotionFunc(glutMouseMove);
    glutMotionFunc(glutMouseMove);

#ifdef WIN32
    // Not availabe in GLUT on Mac
    glutMouseWheelFunc(glutMouseWheel);
#endif

    bool result = gApplication->init(1024, 1024);
    if (result)
    {
        gApplication->setupOpenGL();

        glutMainLoop();
    }

	gApplication->shutdown();

    return 0;
}

// Some defines so we can share dynamic library functions over multiple platforms
#define EXT_C_FUNC      extern "C"

#if WIN32
#define DLL_FUNC_DECL   __declspec(dllexport) __cdecl
#elif __APPLE__
#define DLL_FUNC_DECL   __attribute__((visibility("default")))
#endif

EXT_C_FUNC int DLL_FUNC_DECL startTestGL()
{
    return testGL();
}
