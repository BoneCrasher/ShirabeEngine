package appleduckit.shirabe.engine.platformlayer.android.graphics;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceView;
import android.view.SurfaceHolder;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

/**
 * Created by dotti on 2/12/17.
 */

/**
 * The TemplatedSurfaceView is heavily based on the Android GLSurfaceView
 * implementation and "templates-away" the EGL specifics, replacing them
 * with policies/builders/factories and services, so that the actual low-level
 * API to be used for rendering can be configured from configuration-scripts or
 * hardcoded configuration entities.
 * <p>
 * It's main purpose is to allow a custom implementation of EGL and Vulkan entirely
 * in CPP without having to use the very verbose Java-EGL classes.
 * <p>
 * It is required, that this surface view blends properly into the abstract
 * rendering/resources model of the shirabe engine, so that as much code as possible
 * can be shared across platforms.
 * <p>
 * Consequently, the templated surface view will use specific factories/builders to
 * create the JNI-Adapter classes to the EGL or Vulkan environment being used
 * internally by the rendering and resources environment of the Shirabe-Engine.
 */
public class TemplatedSurfaceView
		extends SurfaceView {

	private final static String  LOG_TAG                 = "TemplatedSurfaceView";
	// Should attachment/detachment from window be logged to Log?
	private final static boolean LOG_ATTACH_DETACH       = false;
	// Log onPause/onResume-events?
	private final static boolean LOG_PAUSE_RESUME        = false;
	// Should the graphics API worker thread be logged?
	private final static boolean LOG_THREADS             = false;
	// Log surface-related events?
	private final static boolean LOG_SURFACE             = false;
	// Log renderer-related common events?
	private final static boolean LOG_RENDERER            = false;
	// Log renderer-related drawing events?
	private final static boolean LOG_RENDERER_DRAW_FRAME = false;
	// Log graphics API specific events?
	private final static boolean LOG_GRAPHICS_API        = false;

	/**
	 * The renderer only renders
	 * when the surface is created, or when {@link #requestRender} is called.
	 *
	 * @see #getRenderMode()
	 * @see #setRenderMode(int)
	 * @see #requestRender()
	 */
	public final static int RENDERMODE_WHEN_DIRTY = 0;

	/**
	 * The renderer is called
	 * continuously to re-render the scene.
	 *
	 * @see #getRenderMode()
	 * @see #setRenderMode(int)
	 */
	public final static int RENDERMODE_CONTINUOUSLY = 1;

	public final static int DEBUG_GRAPHICS_API__CHECK_ERROR   = 1;
	public final static int DEBUG_GRAPHICS_API__LOG_API_CALLS = 2;

	public interface IGraphicsAPI<TUnderlying> {
		TUnderlying getUnderlying();
	}

	/**
	 * Wrap a graphicsAPI in another, so that we can pretend or
	 * append extended functionality to the regular functionality,
	 * e.g. Logging, Tracing, Debugging.
	 */
	public interface IGraphicsAPIWrapper {
		IGraphicsAPI wrap(IGraphicsAPI graphicsAPI);
	}

	public interface IGraphicsAPIContext {

	}

	public interface IGraphicsAPIDisplay {

	}

	public interface IGraphicsAPIConfiguration {

	}

	public interface IGraphicsAPISurface {

	}

	public interface IGraphicsAPIContextFactory {
		IGraphicsAPIContext createContext(IGraphicsAPI apiInterface,
										  IGraphicsAPIDisplay display,
										  IGraphicsAPIConfiguration configuration);

		void destroyContext(IGraphicsAPI apiInterface,
							IGraphicsAPIDisplay display,
							IGraphicsAPIContext context);
	}

	public interface IGraphicsAPIWindowSurfaceFactory {
		IGraphicsAPISurface createWindowSurface(IGraphicsAPI apiInterface,
												IGraphicsAPIDisplay display,
												IGraphicsAPIConfiguration configuration,
												Object nativeWindow);

		void destroyWindowSurface(IGraphicsAPI apiInterface,
								  IGraphicsAPIDisplay display,
								  IGraphicsAPISurface surface);
	}

	public interface IGraphicsAPIConfigurationChooser {
		IGraphicsAPIConfiguration chooseConfig(IGraphicsAPI apiInterface,
											   IGraphicsAPIDisplay display);
	}

	public interface IRenderer {
		void onSurfaceCreated(IGraphicsAPI apiInterface,
							  IGraphicsAPIConfiguration apiConfiguration);

		void onSurfaceChanged(IGraphicsAPI apiInterface,
							  int width,
							  int height);

		void onDrawFrame(IGraphicsAPI apiInterface);
	}

	public TemplatedSurfaceView(Context context) {
		super(context);
		init();
	}

	public TemplatedSurfaceView(Context context,
								AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	@Override
	protected void finalize() throws Throwable {
		try {
			if (_graphicsAPIWorkerThread != null) {
				//
				// GLThread may still be running if this view was never
				// attached to a window.
				//
				_graphicsAPIWorkerThread.requestExitAndWait();
			}
		} finally {
			super.finalize();
		}
	}

	private void init() {
		//
		// Install a SurfaceHolder.Callback so we get notified when the
		// underlying surface is created and destroyed
		//
		// Local nested helper class to "Readably group the implementation methods".
		_surfaceCallbackImplementation = new SurfaceCallbackImpl();

		SurfaceHolder holder = getHolder();
		holder.addCallback(_surfaceCallbackImplementation);
	}

	@Override
	protected void onAttachedToWindow() {
		super.onAttachedToWindow();
		if (LOG_ATTACH_DETACH) {
			Log.d(LOG_TAG, "onAttachedToWindow -> reattach = " + _viewDetachedFromWindow);
		}

		//
		// Are we detached but there's still a renderer instance?
		// If so, continue rendering with the render mode of the
		// current worker thread, if any, otherwise render continuously.
		//
		// (!) Restarting the worker thread with the new render mode
		//     will be required.
		//
		if (_viewDetachedFromWindow && (_renderer != null)) {
			int renderMode = RENDERMODE_CONTINUOUSLY;
			// If there's already a render thread, don't ignore its state.
			if (_graphicsAPIWorkerThread != null) {
				renderMode = _graphicsAPIWorkerThread.getRenderMode();
			}

			_graphicsAPIWorkerThread = new GraphicsAPIWorkerThread(_thisWeakRef);
			_graphicsAPIWorkerThread.setRenderMode(renderMode);
			_graphicsAPIWorkerThread.start();
		}

		_viewDetachedFromWindow = false;
	}

	@Override
	protected void onDetachedFromWindow() {
		if (LOG_ATTACH_DETACH) {
			Log.d(LOG_TAG, "onDetachedToWindow");
		}

		if (_graphicsAPIWorkerThread != null)
			_graphicsAPIWorkerThread.requestExitAndWait();

		_viewDetachedFromWindow = true;

		super.onDetachedFromWindow();
	}

	public void onPause() {
		_graphicsAPIWorkerThread.onPause();
	}

	public void onResume() {
		_graphicsAPIWorkerThread.onResume();
	}

	private class SurfaceCallbackImpl
			implements SurfaceHolder.Callback2 {

		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			_graphicsAPIWorkerThread.surfaceCreated();
		}

		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
			_graphicsAPIWorkerThread.surfaceResized(width, height);
		}

		@Override
		public void surfaceDestroyed(SurfaceHolder holder) {
			_graphicsAPIWorkerThread.surfaceDestroyed();
		}

		@Override
		public void surfaceRedrawNeeded(SurfaceHolder holder) {
			if (_graphicsAPIWorkerThread != null) {
				_graphicsAPIWorkerThread.requestRenderAndWait();
			}
		}
	}

	public void setDebugFlags(int debugFlags) {
		_debugFlags = debugFlags;
	}

	public int getDebugFlags() {
		return _debugFlags;
	}

	public void setRenderer(IRenderer renderer) {
		checkGraphicsAPIWorkerThreadState();

		if (_graphicsAPIConfigurationChooser == null) {
			_graphicsAPIConfigurationChooser = new SimpleEGLConfigChooser(true);
		}

		if (_graphicsAPIContextFactory == null) {
			_graphicsAPIContextFactory = new DefaultEGLContextFactory();
		}

		if (_graphicsAPIWindowSurfaceFactory == null) {
			_grahpicsAPIWindowSurfaceFactory = new DefaultEGLWindowSurfaceFactory();
		}

		_renderer = renderer;
		_graphicsAPIWorkerThread = new GraphicsAPIWorkerThread(_thisWeakRef);
		_graphicsAPIWorkerThread.start();
	}

	public void setGraphicsAPIContextFactory(IGraphicsAPIContextFactory factory) {
		checkGraphicsAPIWorkerThreadState();

		_graphicsAPIContextFactory = factory;
	}

	public void setGraphicsAPIWindowSurfaceFactory(IGraphicsAPIWindowSurfaceFactory factory) {
		_graphicsAPIWindowSurfaceFactory = factory;
	}

	public void setGraphicsAPIConfigurationChooser(IGraphicsAPIConfigurationChooser chooser) {
		_graphicsAPIConfigurationChooser = chooser;
	}

	public void setGraphicsAPIConfigurationChooser(boolean depthBufferRequired) {
		// Do a Vulkan?-switch here and create accordingly...
		// setEGLConfigChooser(new SimpleEGLConfigChooser(needDepth));
	}

	public void setGraphicsAPIConfigurationChooser(
			int redSize, int greenSize, int blueSize,
			int alphaSize, int depthSize, int stencilSize) {
		// Do a Vulkan?-switch here and create accordingly...
		// setGraphicsAPIConfigurationChooser(new ComponentSizeChooser(redSize, greenSize,
		//		blueSize, alphaSize, depthSize, stencilSize));
	}

	public void setGraphicsAPIWrapper(IGraphicsAPIWrapper apiWrapper) {
		_graphicsAPIWrapper = apiWrapper;
	}

	public void setGraphicsAPIContextClientVersion(int version) {
		checkGraphicsAPIWorkerThreadState();
		_graphicsAPIContextClientVersion = version;
	}

	public void setPreserveGraphicsAPIContextOnPause(boolean preserveOnPause) {
		_preserveGraphicsAPIContextOnPause = preserveOnPause;
	}

	public boolean getPreserveGraphicsAPIContextOnPause() {
		return _preserveGraphicsAPIContextOnPause;
	}

	public void setRenderMode(int renderMode) {
		_graphicsAPIWorkerThread.setRenderMode(renderMode);
	}

	public int getRenderMode() {
		return _graphicsAPIWorkerThread.getRenderMode();
	}

	public void requestRender() {
		_graphicsAPIWorkerThread.requestRender();
	}

	public void queueEvent(Runnable r) {
		_graphicsAPIWorkerThread.queueEvent(r);
	}

	static class GraphicsAPIWorkerThread
			extends Thread {
		private static final String LOG_TAG = "GFXAPIWrkThread";

		GraphicsAPIWorkerThread(WeakReference<TemplatedSurfaceView> templatedSurfaceViewWeakRef) {
			super();

			_surfaceWidth = 0;
			_surfaceHeight = 0;

			_requestRender = true;
			_renderMode = RENDERMODE_CONTINUOUSLY;

			_wantRenderNotification = false;
			_templatedSurfaceViewWeakRef = templatedSurfaceViewWeakRef;
		}

		@Override
		public void run() {
			setName("GraphicsAPIWorkerThread [" + getId() + "]");

			if (LOG_THREADS) {
				Log.i(LOG_TAG, "Started graphics api worker thread [tid=" + getId() + "]");
			}

			try {
				guardedRun();
			} catch (InterruptedException e) {
				// Fall through to allow exiting normally.
			} finally {
				_graphicsAPIWorkerThreadStaticManager.threadIsExiting(this);
			}
		}

		private void guardedRun()
				throws InterruptedException {

		}

		public boolean isAbleToDraw() {
			return (_hasGraphicsAPIContext && _hasGraphicsAPIWindowSurface) && isReadyToDraw();
		}

		public boolean isReadyToDraw() {
			return (!_threadIsPaused)
					&& _hasSurface
					&& (!_surfaceIsBad)
					&& (_surfaceWidth > 0)
					&& (_surfaceHeight > 0)
					&& (_requestRender
					|| (_renderMode == RENDERMODE_CONTINUOUSLY));
		}

		public void setRenderMode(int renderMode) {
			if (!((RENDERMODE_WHEN_DIRTY <= renderMode)
					&& (renderMode <= RENDERMODE_CONTINUOUSLY))) {
				throw new IllegalArgumentException("renderMode");
			}

			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				_renderMode = renderMode;
				_graphicsAPIWorkerThreadStaticManager.notifyAll();
			}
		}

		public int getRenderMode() {
			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				return _renderMode;
			}
		}

		public void requestRender() {
			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				_requestRender = true;
				_graphicsAPIWorkerThreadStaticManager.notifyAll();
			}
		}

		public void requestRenderAndWait() {
			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				if (Thread.currentThread() == this) {
					// If we are already on the graphics API worker thread,
					// this means a client callback has caused reentrancy,
					// for example via updating the SurfaceView parameters.
					// We will return to the client rendering code, so here we don't need to
					// do anything.
					return;
				}

				_wantRenderNotification = true;
				_requestRender = true;
				_renderComplete = false;

				_graphicsAPIWorkerThreadStaticManager.notifyAll();

				while (!_threadHasExited
						&& !_threadIsPaused
						&& !_renderComplete
						&& isAbleToDraw()) {
					try {
						_graphicsAPIWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void surfaceCreated() {
			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				if (LOG_THREADS) {
					Log.i(LOG_TAG, "surfaceCreated [tid=" + getId() + "]");
				}

				_hasSurface = true;
				_threadFinishedCreatingGraphicsAPIWindowSurface = false;
				_graphicsAPIWorkerThreadStaticManager.notifyAll();

				while (_waitingForSurface
						&& !_threadFinishedCreatingGraphicsAPIWindowSurface
						&& !_threadHasExited) {
					try {
						_graphicsAPIWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void surfaceDestroyed() {
			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				if (LOG_THREADS) {
					Log.i(LOG_TAG, "surfaceDestroyed [tid=" + getId() + "]");
				}

				_hasSurface = false;
				_graphicsAPIWorkerThreadStaticManager.notifyAll();

				while ((!_waitingForSurface)
						&& (!_threadHasExited)) {
					try {
						_graphicsAPIWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void onPause() {
			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				if (LOG_PAUSE_RESUME) {
					Log.i(LOG_TAG, "onPause [tid=" + getId() + "]");
				}

				_isPauseRequested = true;
				_graphicsAPIWorkerThreadStaticManager.notifyAll();

				while ((!_threadHasExited)
						&& (!_threadIsPaused)) {
					if (LOG_PAUSE_RESUME) {
						Log.i(TemplatedSurfaceView.LOG_TAG, "onPause waiting for _isThreadPaused");
					}

					try {
						_graphicsAPIWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void onResume() {
			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				if (LOG_PAUSE_RESUME) {
					Log.i(LOG_TAG, "onResume [tid=" + getId() + "]");
				}

				_isPauseRequested = false;
				_requestRender = true;
				_renderComplete = false;
				_graphicsAPIWorkerThreadStaticManager.notifyAll();

				while ((!_threadHasExited)
						&& _threadIsPaused
						&& (!_renderComplete)) {
					if (LOG_PAUSE_RESUME) {
						Log.i(TemplatedSurfaceView.LOG_TAG, "onResume waiting for !_isThreadPaused");
					}

					try {
						_graphicsAPIWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void onWindowResize(int w, int h) {
			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				_surfaceWidth = w;
				_surfaceHeight = h;
				_sizeChanged = true;
				_requestRender = true;
				_renderComplete = false;

				if (Thread.currentThread() == this) {
					// If we are already on the graphics API worker thread,
					// this means a client callback has caused reentrancy,
					// for example via updating the SurfaceView parameters.
					// We will return to the client rendering code, so here we don't need to
					// do anything.
					return;
				}

				_graphicsAPIWorkerThreadStaticManager.notifyAll();

				// Wait for thread to resize and rerender.
				while ((!_threadHasExited)
						&& (!_threadIsPaused)
						&& (!_renderComplete)
						&& isAbleToDraw()) {
					if (LOG_SURFACE) {
						Log.i(TemplatedSurfaceView.LOG_TAG, "onWindowResize waiting for render complete from [tid=" + getId() + "]");
					}

					try {
						_graphicsAPIWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void requestExitAndWait() {
			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				_threadShouldExit = true;
				_graphicsAPIWorkerThreadStaticManager.notifyAll();

				while ((!_threadHasExited)) {
					try {
						_graphicsAPIWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void queueEvent(Runnable r) {
			if (r == null) {
				throw new IllegalArgumentException("Paramter r:Runnable must not be null.");
			}

			synchronized (_graphicsAPIWorkerThreadStaticManager) {
				_eventQueue.add(r);
				_graphicsAPIWorkerThreadStaticManager.notifyAll();
			}
		}

		public void requestReleaseGrahpicsAPIContextLocked() {
			_threadShouldReleaseGraphicsAPIContext = true;
			_graphicsAPIWorkerThreadStaticManager.notifyAll();
		}

		private void destroyGraphicsAPIWindowSurfaceLocked() {
			if (_hasGraphicsAPIWindowSurface) {
				_hasGraphicsAPIWindowSurface = false;
				// mEglHelper.destroySurface();
			}
		}

		private void destroyGraphicsAPIContextLocked() {
			if (_hasGraphicsAPIContext) {
				// mEglHelper.finish();
				_hasGraphicsAPIContext = false;
				_graphicsAPIWorkerThreadStaticManager.releaseGraphicsAPIContextLocked(this);
			}
		}


		int _surfaceWidth;
		int _surfaceHeight;
		int _renderMode;

		boolean _sizeChanged;

		boolean _requestRender;
		boolean _renderComplete;
		boolean _wantRenderNotification;

		boolean _hasGraphicsAPIContext;
		boolean _hasGraphicsAPIWindowSurface;
		boolean _threadShouldReleaseGraphicsAPIContext;
		boolean _threadFinishedCreatingGraphicsAPIWindowSurface;

		boolean _waitingForSurface;
		boolean _hasSurface;
		boolean _surfaceIsBad;

		boolean _isPauseRequested;
		boolean _threadIsPaused;
		boolean _threadShouldExit;
		boolean _threadHasExited;

		private ArrayList<Runnable> _eventQueue = new ArrayList<Runnable>();

		WeakReference<TemplatedSurfaceView> _templatedSurfaceViewWeakRef;
	}

	private static class GraphicsAPIWorkerThreadManager {
		private static String LOG_TAG = "GFXAPIWrkThreadMgr";

		public synchronized void threadIsExiting(GraphicsAPIWorkerThread thread) {
			if (LOG_THREADS) {
				Log.i(GraphicsAPIWorkerThread.LOG_TAG, "Exiting thread [tid=" + thread.getId() + "]");
			}

			thread._threadHasExited = true;
			notifyAll();
		}

		public void releaseGraphicsAPIContextLocked(GraphicsAPIWorkerThread thread) {
			notifyAll();
		}
	}

	private void checkGraphicsAPIWorkerThreadState() {
		if (_graphicsAPIWorkerThread != null) {
			throw new IllegalStateException("GraphicsAPI worker thread already instantiated and started.");
		}
	}

	private static final GraphicsAPIWorkerThreadManager _graphicsAPIWorkerThreadStaticManager = new GraphicsAPIWorkerThreadManager();
	GraphicsAPIWorkerThread _graphicsAPIWorkerThread;
	IRenderer               _renderer;

	SurfaceCallbackImpl _surfaceCallbackImplementation = null;

	IGraphicsAPIContextFactory       _graphicsAPIContextFactory       = null;
	IGraphicsAPIWindowSurfaceFactory _graphicsAPIWindowSurfaceFactory = null;
	IGraphicsAPIConfigurationChooser _graphicsAPIConfigurationChooser = null;
	IGraphicsAPIWrapper              _graphicsAPIWrapper              = null;

	WeakReference<TemplatedSurfaceView> _thisWeakRef = new WeakReference<TemplatedSurfaceView>(this);

	boolean _viewDetachedFromWindow;
	boolean _preserveGraphicsAPIContextOnPause;

	int _debugFlags;

	int _graphicsAPIContextClientVersion = 0;


}
