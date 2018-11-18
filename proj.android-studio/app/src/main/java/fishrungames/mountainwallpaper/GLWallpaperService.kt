/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package fishrungames.mountainwallpaper

import java.io.Writer
import java.util.ArrayList

import javax.microedition.khronos.egl.EGL10
import javax.microedition.khronos.egl.EGL11
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.egl.EGLContext
import javax.microedition.khronos.egl.EGLDisplay
import javax.microedition.khronos.egl.EGLSurface
import javax.microedition.khronos.opengles.GL
import javax.microedition.khronos.opengles.GL10

import android.opengl.GLSurfaceView
import android.service.wallpaper.WallpaperService
import android.util.Log
import android.view.SurfaceHolder

// Original code provided by Robert Green
// http://www.rbgrn.net/content/354-glsurfaceview-adapted-3d-live-wallpapers

open class GLWallpaperService : WallpaperService() {

    companion object {
        private val TAG = "GLWallpaperService"
        val RENDERMODE_WHEN_DIRTY = 0;
        val RENDERMODE_CONTINUOUSLY = 1;
    }

    override fun onCreateEngine(): WallpaperService.Engine {
        return GLEngine()
    }

    open inner class GLEngine : WallpaperService.Engine() {

        private var mGLThread: GLThread? = null
        private var mEGLConfigChooser: GLSurfaceView.EGLConfigChooser? = null
        private var mEGLContextFactory: GLSurfaceView.EGLContextFactory? = null
        private var mEGLWindowSurfaceFactory: GLSurfaceView.EGLWindowSurfaceFactory? = null
        private var mEGLContextClientVersion: Int = 0
        var renderMode: Int
            get() = mGLThread!!.renderMode
            set(renderMode) {
                mGLThread!!.renderMode = renderMode
            }

        override fun onVisibilityChanged(visible: Boolean) {
            if (visible) {
                onResume()
            } else {
                onPause()
            }
            super.onVisibilityChanged(visible)
        }

        override fun onDestroy() {
            super.onDestroy()
            // Log.d(TAG, "GLEngine.onDestroy()");
            mGLThread!!.requestExitAndWait()
        }

        override fun onSurfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
            // Log.d(TAG, "onSurfaceChanged()");
            mGLThread!!.onWindowResize(width, height)
            super.onSurfaceChanged(holder, format, width, height)
        }

        override fun onSurfaceCreated(holder: SurfaceHolder) {
            Log.d(TAG, "onSurfaceCreated()")
            mGLThread!!.surfaceCreated(holder)
            super.onSurfaceCreated(holder)
        }

        override fun onSurfaceDestroyed(holder: SurfaceHolder) {
            Log.d(TAG, "onSurfaceDestroyed()")
            mGLThread!!.surfaceDestroyed()
            super.onSurfaceDestroyed(holder)
        }

        fun setRenderer(renderer: GLSurfaceView.Renderer) {
            checkRenderThreadState()

            var mEGLConfigChooser = this.mEGLConfigChooser;
            var mEGLContextFactory = this.mEGLContextFactory;
            var mEGLWindowSurfaceFactory = this.mEGLWindowSurfaceFactory;

            if (mEGLConfigChooser == null) {
                mEGLConfigChooser = BaseConfigChooser.SimpleEGLConfigChooser(true, mEGLContextClientVersion)
            }
            if (mEGLContextFactory == null) {
                mEGLContextFactory = DefaultContextFactory(mEGLContextClientVersion)
            }
            if (mEGLWindowSurfaceFactory == null) {
                mEGLWindowSurfaceFactory = DefaultWindowSurfaceFactory()
            }
            mGLThread = GLThread(renderer, mEGLConfigChooser, mEGLContextFactory, mEGLWindowSurfaceFactory)
            mGLThread!!.start()
        }

        fun setEGLContextFactory(factory: GLSurfaceView.EGLContextFactory) {
            checkRenderThreadState()
            mEGLContextFactory = factory
        }

        fun setEGLWindowSurfaceFactory(factory: GLSurfaceView.EGLWindowSurfaceFactory) {
            checkRenderThreadState()
            mEGLWindowSurfaceFactory = factory
        }

        fun setEGLConfigChooser(configChooser: GLSurfaceView.EGLConfigChooser) {
            checkRenderThreadState()
            mEGLConfigChooser = configChooser
        }

        fun setEGLConfigChooser(needDepth: Boolean) {
            setEGLConfigChooser(BaseConfigChooser.SimpleEGLConfigChooser(needDepth, mEGLContextClientVersion))
        }

        fun setEGLConfigChooser(redSize: Int, greenSize: Int, blueSize: Int, alphaSize: Int, depthSize: Int,
                                stencilSize: Int) {
            setEGLConfigChooser(
                    BaseConfigChooser.ComponentSizeChooser(redSize, greenSize, blueSize, alphaSize,
                            depthSize,
                            stencilSize, mEGLContextClientVersion))
        }

        fun setEGLContextClientVersion(version: Int) {
            checkRenderThreadState()
            mEGLContextClientVersion = version
        }

        open fun requestRender() {
            mGLThread!!.requestRender()
        }

        fun onPause() {
            mGLThread!!.onPause()
        }

        fun onResume() {
            mGLThread!!.onResume()
        }

        fun queueEvent(r: Runnable) {
            mGLThread!!.queueEvent(r)
        }

        private fun checkRenderThreadState() {
            if (mGLThread != null) {
                throw IllegalStateException("setRenderer has already been called for this instance.")
            }
        }
    }
}

internal class LogWriter : Writer() {
    private val mBuilder = StringBuilder()

    override fun close() {
        flushBuilder()
    }

    override fun flush() {
        flushBuilder()
    }

    override fun write(buf: CharArray, offset: Int, count: Int) {
        for (i in 0 until count) {
            val c = buf[offset + i]
            if (c == '\n') {
                flushBuilder()
            } else {
                mBuilder.append(c)
            }
        }
    }

    private fun flushBuilder() {
        if (mBuilder.length > 0) {
            Log.v("GLSurfaceView", mBuilder.toString())
            mBuilder.delete(0, mBuilder.length)
        }
    }
}

// ----------------------------------------------------------------------

internal class DefaultContextFactory(val eglContextClientVersion : Int) : GLSurfaceView.EGLContextFactory {

    private val EGL_CONTEXT_CLIENT_VERSION = 0x3098

    override fun createContext(egl: EGL10, display: EGLDisplay, config: EGLConfig): EGLContext {
        val attrib_list = intArrayOf(EGL_CONTEXT_CLIENT_VERSION, eglContextClientVersion, EGL10.EGL_NONE)
        return egl.eglCreateContext(display, config, EGL10.EGL_NO_CONTEXT,
                if (eglContextClientVersion != 0) attrib_list else null)
    }

    override fun destroyContext(egl: EGL10, display: EGLDisplay, context: EGLContext) {
        egl.eglDestroyContext(display, context)
    }
}

internal class DefaultWindowSurfaceFactory : GLSurfaceView.EGLWindowSurfaceFactory {

    override fun createWindowSurface(egl: EGL10, display: EGLDisplay, config: EGLConfig, nativeWindow: Any): EGLSurface {
        // this is a bit of a hack to work around Droid init problems - if you don't have this, it'll get hung up on orientation changes
        var eglSurface: EGLSurface? = null
        while (eglSurface == null) {
            try {
                eglSurface = egl.eglCreateWindowSurface(display,
                        config, nativeWindow, null)
            } catch (t: Throwable) {
            } finally {
                if (eglSurface == null) {
                    try {
                        Thread.sleep(10)
                    } catch (t: InterruptedException) {
                    }

                }
            }
        }
        return eglSurface
    }

    override fun destroySurface(egl: EGL10, display: EGLDisplay, surface: EGLSurface) {
        egl.eglDestroySurface(display, surface)
    }
}

internal class EglHelper(private val mEGLConfigChooser: GLSurfaceView.EGLConfigChooser, private val mEGLContextFactory: GLSurfaceView.EGLContextFactory,
                         private val mEGLWindowSurfaceFactory: GLSurfaceView.EGLWindowSurfaceFactory) {

    private var mEgl: EGL10? = null
    private var mEglDisplay: EGLDisplay? = null
    private var mEglSurface: EGLSurface? = null
    private var mEglContext: EGLContext? = null
    var mEglConfig: EGLConfig? = null

    /**
     * Initialize EGL for a given configuration spec.
     *
     * @param configSpec
     */
    fun start() {
        // Log.d("EglHelper" + instanceId, "start()");
        if (mEgl == null) {
            // Log.d("EglHelper" + instanceId, "getting new EGL");
            /*
                         * Get an EGL instance
                         */
            mEgl = EGLContext.getEGL() as EGL10
        } else {
            // Log.d("EglHelper" + instanceId, "reusing EGL");
        }

        if (mEglDisplay == null) {
            // Log.d("EglHelper" + instanceId, "getting new display");
            /*
                         * Get to the default display.
                         */
            mEglDisplay = mEgl!!.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY)
        } else {
            // Log.d("EglHelper" + instanceId, "reusing display");
        }

        if (mEglConfig == null) {
            // Log.d("EglHelper" + instanceId, "getting new config");
            /*
                         * We can now initialize EGL for that display
                         */
            val version = IntArray(2)
            mEgl!!.eglInitialize(mEglDisplay, version)
            mEglConfig = mEGLConfigChooser.chooseConfig(mEgl, mEglDisplay)
        } else {
            // Log.d("EglHelper" + instanceId, "reusing config");
        }

        if (mEglContext == null) {
            // Log.d("EglHelper" + instanceId, "creating new context");
            /*
                         * Create an OpenGL ES context. This must be done only once, an OpenGL context is a somewhat heavy object.
                         */
            mEglContext = mEGLContextFactory.createContext(mEgl, mEglDisplay, mEglConfig)
            if (mEglContext == null || mEglContext === EGL10.EGL_NO_CONTEXT) {
                throw RuntimeException("createContext failed")
            }
        } else {
            // Log.d("EglHelper" + instanceId, "reusing context");
        }

        mEglSurface = null
    }

    /*
         * React to the creation of a new surface by creating and returning an OpenGL interface that renders to that
         * surface.
         */
    fun createSurface(holder: SurfaceHolder): GL {
        /*
                 * The window size has changed, so we need to create a new surface.
                 */
        if (mEglSurface != null && mEglSurface !== EGL10.EGL_NO_SURFACE) {

            /*
                         * Unbind and destroy the old EGL surface, if there is one.
                         */
            mEgl!!.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT)
            mEGLWindowSurfaceFactory.destroySurface(mEgl, mEglDisplay, mEglSurface)
        }

        /*
                 * Create an EGL surface we can render into.
                 */
        mEglSurface = mEGLWindowSurfaceFactory.createWindowSurface(mEgl, mEglDisplay, mEglConfig, holder)

        if (mEglSurface == null || mEglSurface === EGL10.EGL_NO_SURFACE) {
            throw RuntimeException("createWindowSurface failed")
        }

        /*
                 * Before we can issue GL commands, we need to make sure the context is current and bound to a surface.
                 */
        if (!mEgl!!.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
            throw RuntimeException("eglMakeCurrent failed.")
        }

        var gl = mEglContext!!.gl

        /*
                 * if ((mDebugFlags & (DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS))!= 0) { int configFlags = 0; Writer log =
                 * null; if ((mDebugFlags & DEBUG_CHECK_GL_ERROR) != 0) { configFlags |= GLDebugHelper.CONFIG_CHECK_GL_ERROR; }
                 * if ((mDebugFlags & DEBUG_LOG_GL_CALLS) != 0) { log = new LogWriter(); } gl = GLDebugHelper.wrap(gl,
                 * configFlags, log); }
                 */

        return gl
    }

    /**
     * Display the current render surface.
     *
     * @return false if the context has been lost.
     */
    fun swap(): Boolean {
        mEgl!!.eglSwapBuffers(mEglDisplay, mEglSurface)

        /*
                 * Always check for EGL_CONTEXT_LOST, which means the context and all associated data were lost (For instance
                 * because the device went to sleep). We need to sleep until we get a new surface.
                 */
        return mEgl!!.eglGetError() != EGL11.EGL_CONTEXT_LOST
    }

    fun destroySurface() {
        if (mEglSurface != null && mEglSurface !== EGL10.EGL_NO_SURFACE) {
            mEgl!!.eglMakeCurrent(mEglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT)
            mEGLWindowSurfaceFactory.destroySurface(mEgl, mEglDisplay, mEglSurface)
            mEglSurface = null
        }
    }

    fun finish() {
        if (mEglContext != null) {
            mEGLContextFactory.destroyContext(mEgl, mEglDisplay, mEglContext)
            mEglContext = null
        }
        if (mEglDisplay != null) {
            mEgl!!.eglTerminate(mEglDisplay)
            mEglDisplay = null
        }
    }
}

internal class GLThread
(
        // End of member variables protected by the sGLThreadManager monitor.

    private val mRenderer: GLSurfaceView.Renderer, private val mEGLConfigChooser: GLSurfaceView.EGLConfigChooser, private val mEGLContextFactory: GLSurfaceView.EGLContextFactory,
    private val mEGLWindowSurfaceFactory: GLSurfaceView.EGLWindowSurfaceFactory) : Thread() {

    private val sGLThreadManager = GLThreadManager()
    private var mEglOwner: GLThread? = null

    lateinit var mHolder: SurfaceHolder
    private var mSizeChanged = true

    // Once the thread is started, all accesses to the following member
    // variables are protected by the sGLThreadManager monitor
    var mDone: Boolean = false
    private var mPaused: Boolean = false
    private var mHasSurface: Boolean = false
    private var mWaitingForSurface: Boolean = false
    private var mHaveEgl: Boolean = false
    private var mWidth: Int = 0
    private var mHeight: Int = 0
    private var mRenderMode: Int = 0
    private var mRequestRender: Boolean = false
    private var mEventsWaiting: Boolean = false
    private val mEventQueue = ArrayList<Runnable>()
    private var mEglHelper: EglHelper? = null

    private val isDone: Boolean
        get() = synchronized(sGLThreadManager.lockObject) {
            return mDone
        }

    var renderMode: Int
        get() = synchronized(sGLThreadManager.lockObject) {
            return mRenderMode
        }
        set(renderMode) {
            if (!(GLWallpaperService.RENDERMODE_WHEN_DIRTY <= renderMode && renderMode <= GLWallpaperService.RENDERMODE_CONTINUOUSLY)) {
                throw IllegalArgumentException("renderMode")
            }
            synchronized(sGLThreadManager.lockObject) {
                mRenderMode = renderMode
                if (renderMode == GLWallpaperService.RENDERMODE_CONTINUOUSLY) {
                    sGLThreadManager.lockObject.notifyAll()
                }
            }
        }

    private val event: Runnable?
        get() {
            synchronized(this) {
                if (mEventQueue.size > 0) {
                    return mEventQueue.removeAt(0)
                }

            }
            return null
        }

    init {
        mDone = false
        mWidth = 0
        mHeight = 0
        mRequestRender = true
        mRenderMode = GLWallpaperService.RENDERMODE_CONTINUOUSLY
    }

    override fun run() {
        name = "GLThread $id"
        if (LOG_THREADS) {
            Log.i("GLThread", "starting tid=$id")
        }

        try {
            guardedRun()
        } catch (e: InterruptedException) {
            // fall thru and exit normally
        } finally {
            sGLThreadManager.threadExiting(this)
        }
    }

    /*
         * This private method should only be called inside a synchronized(sGLThreadManager.lockObject) block.
         */
    private fun stopEglLocked() {
        if (mHaveEgl) {
            mHaveEgl = false
            mEglHelper!!.destroySurface()
            sGLThreadManager.releaseEglSurface(this)
        }
    }

    @Throws(InterruptedException::class)
    private fun guardedRun() {
        mEglHelper = EglHelper(mEGLConfigChooser, mEGLContextFactory, mEGLWindowSurfaceFactory)
        try {
            var gl: GL10? = null
            var tellRendererSurfaceCreated = true
            var tellRendererSurfaceChanged = true

            /*
                         * This is our main activity thread's loop, we go until asked to quit.
                         */
            while (!isDone) {
                /*
                                 * Update the asynchronous state (window size)
                                 */
                var w = 0
                var h = 0
                var changed = false
                var needStart = false
                var eventsWaiting = false

                synchronized(sGLThreadManager.lockObject) {
                    while (true) {
                        // Manage acquiring and releasing the SurfaceView
                        // surface and the EGL surface.
                        if (mPaused) {
                            stopEglLocked()
                        }
                        if (!mHasSurface) {
                            if (!mWaitingForSurface) {
                                stopEglLocked()
                                mWaitingForSurface = true
                                sGLThreadManager.lockObject.notifyAll()
                            }
                        } else {
                            if (!mHaveEgl) {
                                if (sGLThreadManager.tryAcquireEglSurface(this)) {
                                    mHaveEgl = true
                                    mEglHelper!!.start()
                                    mRequestRender = true
                                    needStart = true
                                }
                            }
                        }

                        // Check if we need to wait. If not, update any state
                        // that needs to be updated, copy any state that
                        // needs to be copied, and use "break" to exit the
                        // wait loop.

                        if (mDone) {
                            return
                        }

                        if (mEventsWaiting) {
                            eventsWaiting = true
                            mEventsWaiting = false
                            break
                        }

                        if (!mPaused && mHasSurface && mHaveEgl && mWidth > 0 && mHeight > 0
                                && (mRequestRender || mRenderMode == GLWallpaperService.RENDERMODE_CONTINUOUSLY)) {
                            changed = mSizeChanged
                            w = mWidth
                            h = mHeight
                            mSizeChanged = false
                            mRequestRender = false
                            if (mHasSurface && mWaitingForSurface) {
                                changed = true
                                mWaitingForSurface = false
                                sGLThreadManager.lockObject.notifyAll()
                            }
                            break
                        }

                        // By design, this is the only place where we wait().

                        if (LOG_THREADS) {
                            Log.i("GLThread", "waiting tid=$id")
                        }
                        sGLThreadManager.lockObject.wait()
                    }
                } // end of synchronized(sGLThreadManager.lockObject)

                /*
                                 * Handle queued events
                                 */
                if (eventsWaiting) {
                    var r: Runnable? = event
                    while (r != null) {
                        r!!.run()
                        if (isDone) {
                            return
                        }
                    }
                    // Go back and see if we need to wait to render.
                    continue
                }

                if (needStart) {
                    tellRendererSurfaceCreated = true
                    changed = true
                }
                if (changed) {
                    gl = mEglHelper!!.createSurface(mHolder) as GL10
                    tellRendererSurfaceChanged = true
                }
                if (tellRendererSurfaceCreated) {
                    mRenderer.onSurfaceCreated(gl, mEglHelper!!.mEglConfig)
                    tellRendererSurfaceCreated = false
                }
                if (tellRendererSurfaceChanged) {
                    mRenderer.onSurfaceChanged(gl, w, h)
                    tellRendererSurfaceChanged = false
                }
                if (w > 0 && h > 0) {
                    /* draw a frame here */
                    mRenderer.onDrawFrame(gl)

                    /*
                                         * Once we're done with GL, we need to call swapBuffers() to instruct the system to display the
                                         * rendered frame
                                         */
                    mEglHelper!!.swap()
                    Thread.sleep(10)
                }
            }
        } finally {
            /*
                         * clean-up everything...
                         */
            synchronized(sGLThreadManager.lockObject) {
                stopEglLocked()
                mEglHelper!!.finish()
            }
        }
    }

    fun requestRender() {
        synchronized(sGLThreadManager.lockObject) {
            mRequestRender = true
            sGLThreadManager.lockObject.notifyAll()
        }
    }

    fun surfaceCreated(holder: SurfaceHolder) {
        mHolder = holder
        synchronized(sGLThreadManager.lockObject) {
            if (LOG_THREADS) {
                Log.i("GLThread", "surfaceCreated tid=$id")
            }
            mHasSurface = true
            sGLThreadManager.lockObject.notifyAll()
        }
    }

    fun surfaceDestroyed() {
        synchronized(sGLThreadManager.lockObject) {
            if (LOG_THREADS) {
                Log.i("GLThread", "surfaceDestroyed tid=$id")
            }
            mHasSurface = false
            sGLThreadManager.lockObject.notifyAll()
            while (!mWaitingForSurface && isAlive && !mDone) {
                try {
                    sGLThreadManager.lockObject.wait()
                } catch (e: InterruptedException) {
                    Thread.currentThread().interrupt()
                }

            }
        }
    }

    fun onPause() {
        synchronized(sGLThreadManager.lockObject) {
            mPaused = true
            sGLThreadManager.lockObject.notifyAll()
        }
    }

    fun onResume() {
        synchronized(sGLThreadManager.lockObject) {
            mPaused = false
            mRequestRender = true
            sGLThreadManager.lockObject.notifyAll()
        }
    }

    fun onWindowResize(w: Int, h: Int) {
        synchronized(sGLThreadManager.lockObject) {
            mWidth = w
            mHeight = h
            mSizeChanged = true
            sGLThreadManager.lockObject.notifyAll()
        }
    }

    fun requestExitAndWait() {
        // don't call this from GLThread thread or it is a guaranteed
        // deadlock!
        synchronized(sGLThreadManager.lockObject) {
            mDone = true
            sGLThreadManager.lockObject.notifyAll()
        }
        try {
            join()
        } catch (ex: InterruptedException) {
            Thread.currentThread().interrupt()
        }

    }

    /**
     * Queue an "event" to be run on the GL rendering thread.
     *
     * @param r
     * the runnable to be run on the GL rendering thread.
     */
    fun queueEvent(r: Runnable) {
        synchronized(this) {
            mEventQueue.add(r)
            synchronized(sGLThreadManager.lockObject) {
                mEventsWaiting = true
                sGLThreadManager.lockObject.notifyAll()
            }
        }
    }

    private inner class GLThreadManager {

        val lockObject = Object()

        @Synchronized
        fun threadExiting(thread: GLThread) {
            synchronized(lockObject)
            {
                if (LOG_THREADS) {
                    Log.i("GLThread", "exiting tid=" + thread.id)
                }
                thread.mDone = true
                if (mEglOwner === thread) {
                    mEglOwner = null
                }
                lockObject.notifyAll()
            }
        }

        /*
                 * Tries once to acquire the right to use an EGL surface. Does not block.
                 *
                 * @return true if the right to use an EGL surface was acquired.
                 */
        @Synchronized
        fun tryAcquireEglSurface(thread: GLThread): Boolean {
            if (mEglOwner === thread || mEglOwner == null) {
                mEglOwner = thread
                lockObject.notifyAll()
                return true
            }
            return false
        }

        @Synchronized
        fun releaseEglSurface(thread: GLThread) {
            if (mEglOwner === thread) {
                mEglOwner = null
            }
            lockObject.notifyAll()
        }
    }

    companion object {
        private val LOG_THREADS = false
        val DEBUG_CHECK_GL_ERROR = 1
        val DEBUG_LOG_GL_CALLS = 2
    }
}

