package appleduckit.shirabe.engine.platformlayer.android.graphics;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.lang.ref.WeakReference;
import java.util.ArrayList;

import appleduckit.shirabe.engine.platformlayer.android.graphics.gpu_api.GpuApiInterface;

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

	public interface IGpuApi<TUnderlying> {
		TUnderlying getUnderlying();
	}

	/**
	 * Wrap a gpuApi in another, so that we can pretend or
	 * append extended functionality to the regular functionality,
	 * e.g. Logging, Tracing, Debugging.
	 */
	public interface IGpuApiWrapper {
		IGpuApi wrap(IGpuApi gpuApi);
	}

	public interface IGpuApiContext {

	}

	public interface IGpuApiDisplay {

	}

	public interface IGpuApiConfiguration {

	}

	public interface IGpuApiSurface {

	}

	public interface IGpuApiContextFactory {
		IGpuApiContext createContext(IGpuApi apiInterface,
									 IGpuApiDisplay display,
									 IGpuApiConfiguration configuration);

		void destroyContext(IGpuApi apiInterface,
							IGpuApiDisplay display,
							IGpuApiContext context);
	}

	public interface IGpuApiWindowSurfaceFactory {
		IGpuApiSurface createWindowSurface(IGpuApi apiInterface,
										   IGpuApiDisplay display,
										   IGpuApiConfiguration configuration,
										   Object nativeWindow);

		void destroyWindowSurface(IGpuApi apiInterface,
								  IGpuApiDisplay display,
								  IGpuApiSurface surface);
	}

	public interface IGpuApiConfigurationChooser {
		IGpuApiConfiguration chooseConfig(IGpuApi apiInterface,
										  IGpuApiDisplay display);
	}

	public interface IRenderer {
		void onSurfaceCreated(IGpuApi apiInterface,
							  IGpuApiConfiguration apiConfiguration);

		void onSurfaceChanged(IGpuApi apiInterface,
							  int width,
							  int height);

		void onDrawFrame(IGpuApi apiInterface);
	}

	static class GpuApiWorkerThread
			extends Thread {
		GpuApiWorkerThread(WeakReference<TemplatedSurfaceView> templatedSurfaceViewWeakRef) {
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
			setName("GpuApiWorkerThread [" + getId() + "]");

			if (LOG_THREADS) {
				Log.i(LOG_TAG, "Started graphics api worker thread [tid=" + getId() + "]");
			}

			try {
				guardedRun();
			} catch (InterruptedException e) {
				// Fall through to allow exiting normally.
			} finally {
				_gpuApiWorkerThreadStaticManager.threadIsExiting(this);
			}
		}

		private void guardedRun()
				throws InterruptedException {

		}

		public int getRenderMode() {
			synchronized (_gpuApiWorkerThreadStaticManager) {
				return _renderMode;
			}
		}

		public void setRenderMode(int renderMode) {
			if (!((RENDERMODE_WHEN_DIRTY <= renderMode)
						  && (renderMode <= RENDERMODE_CONTINUOUSLY))) {
				throw new IllegalArgumentException("renderMode");
			}

			synchronized (_gpuApiWorkerThreadStaticManager) {
				_renderMode = renderMode;
				_gpuApiWorkerThreadStaticManager.notifyAll();
			}
		}

		public void requestRender() {
			synchronized (_gpuApiWorkerThreadStaticManager) {
				_requestRender = true;
				_gpuApiWorkerThreadStaticManager.notifyAll();
			}
		}

		public void requestRenderAndWait() {
			synchronized (_gpuApiWorkerThreadStaticManager) {
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

				_gpuApiWorkerThreadStaticManager.notifyAll();

				while (!_threadHasExited
							   && !_threadIsPaused
							   && !_renderComplete
							   && isAbleToDraw()) {
					try {
						_gpuApiWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public boolean isAbleToDraw() {
			return (_hasGpuApiContext && _hasGpuApiWindowSurface) && isReadyToDraw();
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

		public void surfaceCreated() {
			synchronized (_gpuApiWorkerThreadStaticManager) {
				if (LOG_THREADS) {
					Log.i(LOG_TAG, "surfaceCreated [tid=" + getId() + "]");
				}

				_hasSurface = true;
				_threadFinishedCreatingGpuApiWindowSurface = false;
				_gpuApiWorkerThreadStaticManager.notifyAll();

				while (_waitingForSurface
							   && !_threadFinishedCreatingGpuApiWindowSurface
							   && !_threadHasExited) {
					try {
						_gpuApiWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void surfaceDestroyed() {
			synchronized (_gpuApiWorkerThreadStaticManager) {
				if (LOG_THREADS) {
					Log.i(LOG_TAG, "surfaceDestroyed [tid=" + getId() + "]");
				}

				_hasSurface = false;
				_gpuApiWorkerThreadStaticManager.notifyAll();

				while ((!_waitingForSurface)
							   && (!_threadHasExited)) {
					try {
						_gpuApiWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void onPause() {
			synchronized (_gpuApiWorkerThreadStaticManager) {
				if (LOG_PAUSE_RESUME) {
					Log.i(LOG_TAG, "onPause [tid=" + getId() + "]");
				}

				_isPauseRequested = true;
				_gpuApiWorkerThreadStaticManager.notifyAll();

				while ((!_threadHasExited)
							   && (!_threadIsPaused)) {
					if (LOG_PAUSE_RESUME) {
						Log.i(TemplatedSurfaceView.LOG_TAG, "onPause waiting for _isThreadPaused");
					}

					try {
						_gpuApiWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void onResume() {
			synchronized (_gpuApiWorkerThreadStaticManager) {
				if (LOG_PAUSE_RESUME) {
					Log.i(LOG_TAG, "onResume [tid=" + getId() + "]");
				}

				_isPauseRequested = false;
				_requestRender = true;
				_renderComplete = false;
				_gpuApiWorkerThreadStaticManager.notifyAll();

				while ((!_threadHasExited)
							   && _threadIsPaused
							   && (!_renderComplete)) {
					if (LOG_PAUSE_RESUME) {
						Log.i(TemplatedSurfaceView.LOG_TAG, "onResume waiting for !_isThreadPaused");
					}

					try {
						_gpuApiWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void onWindowResize(int w, int h) {
			synchronized (_gpuApiWorkerThreadStaticManager) {
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

				_gpuApiWorkerThreadStaticManager.notifyAll();

				// Wait for thread to resize and rerender.
				while ((!_threadHasExited)
							   && (!_threadIsPaused)
							   && (!_renderComplete)
							   && isAbleToDraw()) {
					if (LOG_SURFACE) {
						Log.i(TemplatedSurfaceView.LOG_TAG, "onWindowResize waiting for render complete from [tid=" + getId() + "]");
					}

					try {
						_gpuApiWorkerThreadStaticManager.wait();
					} catch (InterruptedException e) {
						Thread.currentThread().interrupt();
					}
				}
			}
		}

		public void requestExitAndWait() {
			synchronized (_gpuApiWorkerThreadStaticManager) {
				_threadShouldExit = true;
				_gpuApiWorkerThreadStaticManager.notifyAll();

				while ((!_threadHasExited)) {
					try {
						_gpuApiWorkerThreadStaticManager.wait();
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

			synchronized (_gpuApiWorkerThreadStaticManager) {
				_eventQueue.add(r);
				_gpuApiWorkerThreadStaticManager.notifyAll();
			}
		}

		public void requestReleaseGrahpicsAPIContextLocked() {
			_threadShouldReleaseGpuApiContext = true;
			_gpuApiWorkerThreadStaticManager.notifyAll();
		}

		private void destroyGpuApiWindowSurfaceLocked() {
			if (_hasGpuApiWindowSurface) {
				_hasGpuApiWindowSurface = false;
				// mEglHelper.destroySurface();
			}
		}

		private void destroyGpuApiContextLocked() {
			if (_hasGpuApiContext) {
				// mEglHelper.finish();
				_hasGpuApiContext = false;
				_gpuApiWorkerThreadStaticManager.releaseGpuApiContextLocked(this);
			}
		}

		private static final String LOG_TAG = "GpuApiWrkThread";
		int                                 _surfaceWidth;
		int                                 _surfaceHeight;
		int                                 _renderMode;
		boolean                             _sizeChanged;
		boolean                             _requestRender;
		boolean                             _renderComplete;
		boolean                             _wantRenderNotification;
		boolean                             _hasGpuApiContext;
		boolean                             _hasGpuApiWindowSurface;
		boolean                             _threadShouldReleaseGpuApiContext;
		boolean                             _threadFinishedCreatingGpuApiWindowSurface;
		boolean                             _waitingForSurface;
		boolean                             _hasSurface;
		boolean                             _surfaceIsBad;
		boolean                             _isPauseRequested;
		boolean                             _threadIsPaused;
		boolean                             _threadShouldExit;
		boolean                             _threadHasExited;
		WeakReference<TemplatedSurfaceView> _templatedSurfaceViewWeakRef;
		private ArrayList<Runnable> _eventQueue = new ArrayList<Runnable>();
	}

	private static class GpuApiWorkerThreadManager {
		public synchronized void threadIsExiting(GpuApiWorkerThread thread) {
			if (LOG_THREADS) {
				Log.i(GpuApiWorkerThread.LOG_TAG, "Exiting thread [tid=" + thread.getId() + "]");
			}

			thread._threadHasExited = true;
			notifyAll();
		}

		public void releaseGpuApiContextLocked(GpuApiWorkerThread thread) {
			notifyAll();
		}

		private static String LOG_TAG = "GpuApiWrkThreadMgr";
	}

	private class SurfaceCallbackImpl
			implements SurfaceHolder.Callback2 {

		@Override
		public void surfaceCreated(SurfaceHolder holder) {
			_gpuApiWorkerThread.surfaceCreated();
		}

		@Override
		public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
			// _gpuApiWorkerThread.surfaceResized(width, height);
		}

		@Override
		public void surfaceDestroyed(SurfaceHolder holder) {
			_gpuApiWorkerThread.surfaceDestroyed();
		}

		@Override
		public void surfaceRedrawNeeded(SurfaceHolder holder) {
			if (_gpuApiWorkerThread != null) {
				_gpuApiWorkerThread.requestRenderAndWait();
			}
		}
	}

	public TemplatedSurfaceView(Context context) {
		super(context);
		initializeAndroidViewDependencies();
	}

	private void initializeAndroidViewDependencies() {
		//
		// Install a SurfaceHolder.Callback so we get notified when the
		// underlying surface is created and destroyed
		//
		// Local nested helper class to "Readably group the implementation methods".
		_surfaceCallbackImplementation = new SurfaceCallbackImpl();

		SurfaceHolder holder = getHolder();
		holder.addCallback(_surfaceCallbackImplementation);
	}

	public TemplatedSurfaceView(Context context,
								AttributeSet attrs) {
		super(context, attrs);
		initializeAndroidViewDependencies();
	}

	public void onPause() {
		_gpuApiWorkerThread.onPause();
	}

	public void onResume() {
		_gpuApiWorkerThread.onResume();
	}

	public int getDebugFlags() {
		return _debugFlags;
	}

	public void setDebugFlags(int debugFlags) {
		_debugFlags = debugFlags;
	}

	public void setRenderer(IRenderer renderer) {
		checkGpuApiWorkerThreadState();

		if (_gpuApiConfigurationChooser == null) {
			//_gpuApiConfigurationChooser = new SimpleEGLConfigChooser(true);
		}

		if (_gpuApiContextFactory == null) {
			//_gpuApiContextFactory = new DefaultEGLContextFactory();
		}

		if (_gpuApiWindowSurfaceFactory == null) {
			//_grahpicsAPIWindowSurfaceFactory = new DefaultEGLWindowSurfaceFactory();
		}

		_renderer = renderer;
		_gpuApiWorkerThread = new GpuApiWorkerThread(_thisWeakRef);
		_gpuApiWorkerThread.start();
	}

	private void checkGpuApiWorkerThreadState() {
		if (_gpuApiWorkerThread != null) {
			throw new IllegalStateException("GpuApi worker thread already instantiated and started.");
		}
	}

	public void setGpuApiContextFactory(IGpuApiContextFactory factory) {
		checkGpuApiWorkerThreadState();

		_gpuApiContextFactory = factory;
	}

	public void setGpuApiWindowSurfaceFactory(IGpuApiWindowSurfaceFactory factory) {
		_gpuApiWindowSurfaceFactory = factory;
	}

	public void setGpuApiConfigurationChooser(IGpuApiConfigurationChooser chooser) {
		_gpuApiConfigurationChooser = chooser;
	}

	public void setGpuApiConfigurationChooser(boolean depthBufferRequired) {
		// Do a Vulkan?-switch here and create accordingly...
		// setEGLConfigChooser(new SimpleEGLConfigChooser(needDepth));
	}

	public void setGpuApiConfigurationChooser(
													 int redSize, int greenSize, int blueSize,
													 int alphaSize, int depthSize, int stencilSize) {
		// Do a Vulkan?-switch here and create accordingly...
		// setGpuApiConfigurationChooser(new ComponentSizeChooser(redSize, greenSize,
		//		blueSize, alphaSize, depthSize, stencilSize));
	}

	public void setGpuApiWrapper(IGpuApiWrapper apiWrapper) {
		_gpuApiWrapper = apiWrapper;
	}

	public void setGpuApiContextClientVersion(int version) {
		checkGpuApiWorkerThreadState();
		_gpuApiContextClientVersion = version;
	}

	public boolean getPreserveGpuApiContextOnPause() {
		return _preserveGpuApiContextOnPause;
	}

	public void setPreserveGpuApiContextOnPause(boolean preserveOnPause) {
		_preserveGpuApiContextOnPause = preserveOnPause;
	}

	public int getRenderMode() {
		return _gpuApiWorkerThread.getRenderMode();
	}

	public void setRenderMode(int renderMode) {
		_gpuApiWorkerThread.setRenderMode(renderMode);
	}

	public void requestRender() {
		_gpuApiWorkerThread.requestRender();
	}

	public void queueEvent(Runnable r) {
		_gpuApiWorkerThread.queueEvent(r);
	}

	@Override
	protected void finalize() throws Throwable {
		try {
			if (_gpuApiWorkerThread != null) {
				//
				// GLThread may still be running if this view was never
				// attached to a window.
				//
				_gpuApiWorkerThread.requestExitAndWait();
			}
		} finally {
			super.finalize();
		}
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
			if (_gpuApiWorkerThread != null) {
				renderMode = _gpuApiWorkerThread.getRenderMode();
			}

			_gpuApiWorkerThread = new GpuApiWorkerThread(_thisWeakRef);
			_gpuApiWorkerThread.setRenderMode(renderMode);
			_gpuApiWorkerThread.start();
		}

		_viewDetachedFromWindow = false;
	}

	@Override
	protected void onDetachedFromWindow() {
		if (LOG_ATTACH_DETACH) {
			Log.d(LOG_TAG, "onDetachedToWindow");
		}

		if (_gpuApiWorkerThread != null)
			_gpuApiWorkerThread.requestExitAndWait();

		_viewDetachedFromWindow = true;

		super.onDetachedFromWindow();
	}

	private void initializeGpuApiFunctionality(GpuApiInterface.GpuApiConfiguration config) {
		_gpuApiConfiguration = config;
	}

	/**
	 * The renderer only renders
	 * when the surface is created, or when {@link #requestRender} is called.
	 *
	 * @see #getRenderMode()
	 * @see #setRenderMode(int)
	 * @see #requestRender()
	 */
	public final static  int                       RENDERMODE_WHEN_DIRTY             = 0;
	/**
	 * The renderer is called
	 * continuously to re-render the scene.
	 *
	 * @see #getRenderMode()
	 * @see #setRenderMode(int)
	 */
	public final static  int                       RENDERMODE_CONTINUOUSLY           = 1;
	public final static  int                       DEBUG_GRAPHICS_API__CHECK_ERROR   = 1;
	public final static  int                       DEBUG_GRAPHICS_API__LOG_API_CALLS = 2;
	private final static String                    LOG_TAG                           = "TemplatedSurfaceView";
	// Should attachment/detachment from window be logged to Log?
	private final static boolean                   LOG_ATTACH_DETACH                 = false;
	// Log onPause/onResume-events?
	private final static boolean                   LOG_PAUSE_RESUME                  = false;
	// Should the graphics API worker thread be logged?
	private final static boolean                   LOG_THREADS                       = false;
	// Log surface-related events?
	private final static boolean                   LOG_SURFACE                       = false;
	// Log renderer-related common events?
	private final static boolean                   LOG_RENDERER                      = false;
	// Log renderer-related drawing events?
	private final static boolean                   LOG_RENDERER_DRAW_FRAME           = false;
	// Log graphics API specific events?
	private final static boolean                   LOG_GRAPHICS_API                  = false;
	private static final GpuApiWorkerThreadManager _gpuApiWorkerThreadStaticManager  = new GpuApiWorkerThreadManager();
	GpuApiInterface.GpuApiConfiguration _gpuApiConfiguration           = null;
	int                                 _gpuApiContextClientVersion    = 0;
	boolean                             _preserveGpuApiContextOnPause  = true;
	IGpuApiContextFactory               _gpuApiContextFactory          = null;
	IGpuApiWindowSurfaceFactory         _gpuApiWindowSurfaceFactory    = null;
	IGpuApiConfigurationChooser         _gpuApiConfigurationChooser    = null;
	IGpuApiWrapper                      _gpuApiWrapper                 = null;
	// Android View
	SurfaceCallbackImpl                 _surfaceCallbackImplementation = null;
	boolean            _viewDetachedFromWindow;
	GpuApiWorkerThread _gpuApiWorkerThread;
	IRenderer          _renderer;
	WeakReference<TemplatedSurfaceView> _thisWeakRef = new WeakReference<TemplatedSurfaceView>(this);
	int _debugFlags;


}
