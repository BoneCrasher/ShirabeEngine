package appleduckit.shirabe.engine.platformlayer.android.graphics.gpu_api;

import android.annotation.TargetApi;
import android.os.Build;
import android.util.Log;

import JDK1_8_Helper.LambdaFunctionalCompatibility;

/**
 * Created by Shirabe on 17/02/2017.
 */

public class GpuApiInterface {

	enum SupportedGpuApi {
		// GLES_V10, -> NO more used acc. to Android Dashboards?
		GLES_V20,
		GLES_V30,
		GLES_V31,
		Vulkan
	}

	public class GLESConfiguration {

	}

	public class VulkanConfiguration {

	}

	public class GpuApiConfiguration {
		class GpuApiConfigurationException
				extends Exception {
			public GpuApiConfigurationException() {
				super(String.format("%s Failed to create/handle configuration instance.", TAG));
			}

			public GpuApiConfigurationException(String message) {
				super(String.format("%s %s", TAG, message));
			}

			public GpuApiConfigurationException(String message, Throwable cause) {
				super(String.format("%s %s", TAG, message), cause);
			}

			public GpuApiConfigurationException(Throwable cause) {
				super(String.format("%s Failed to create/handle configuration instance.", TAG),
					  cause);
			}

			@TargetApi(24)
			protected GpuApiConfigurationException(String message,
												   Throwable cause,
												   boolean enableSuppression,
												   boolean writableStackTrace) {
				super(String.format("%s %s", TAG, message), cause, enableSuppression, writableStackTrace);
			}

			private static final String TAG = "[GpuApiConfigurationException]";
		}

		GpuApiConfiguration() throws GpuApiConfigurationException {
			if (!determineDefaultPlatformSupportedConfiguration())
				throw new GpuApiConfigurationException("Failed to determine default platform supported configuration.");
		}

		private boolean determineDefaultPlatformSupportedConfiguration() {
			// Add a huuuuuuge collection of checks here.

			LambdaFunctionalCompatibility.SingleTypePredicate<Integer> sdkAtLeastFn = (api, chk) -> api >= chk;

			//
			// Determine basic platform parameters.
			//
			int android_sdk_int = Build.VERSION.SDK_INT;

			//
			// Check for features by platform.
			//
			boolean egl_supported       = false;
			boolean gles_supported      = false;
			boolean gles_supported__v20 = false;
			boolean gles_supported__v30 = false;
			boolean gles_supported__v31 = false;

			boolean vulkan_supported = false;

			if (sdkAtLeastFn.test(android_sdk_int, Build.VERSION_CODES.N)) { // 24+
				egl_supported = true;
				gles_supported = true;
				gles_supported__v20 = true;
				gles_supported__v30 = true;
				gles_supported__v31 = true;
				vulkan_supported = true;
			} else if (sdkAtLeastFn.test(android_sdk_int, Build.VERSION_CODES.LOLLIPOP)) { // 21+
				egl_supported = true;
				gles_supported = true;
				gles_supported__v20 = true;
				gles_supported__v30 = true;
				gles_supported__v31 = true;
				vulkan_supported = false;
			} else if (sdkAtLeastFn.test(android_sdk_int, Build.VERSION_CODES.JELLY_BEAN_MR2)) { // 18+
				egl_supported = true;
				gles_supported = true;
				gles_supported__v20 = true;
				gles_supported__v30 = true;
				gles_supported__v31 = false;
				vulkan_supported = false;
			} else if (sdkAtLeastFn.test(android_sdk_int, Build.VERSION_CODES.GINGERBREAD)) { // 9+
				egl_supported = true;
				gles_supported = true;
				gles_supported__v20 = true;
				gles_supported__v30 = false;
				gles_supported__v31 = false;
				vulkan_supported = false;
			} else if (sdkAtLeastFn.test(android_sdk_int, Build.VERSION_CODES.ECLAIR)) { // 5+
				egl_supported = false;
				gles_supported = true;
				gles_supported__v20 = true;
				gles_supported__v30 = false;
				gles_supported__v31 = false;
				vulkan_supported = false;
			} else {
				// Fully unsupported!
				egl_supported = false;
				gles_supported = false;
				gles_supported__v20 = false;
				gles_supported__v30 = false;
				gles_supported__v31 = false;
				vulkan_supported = false;
			}

			//
			// Evaluate all data gathered to configuration.
			//
			boolean foundSupportedGpuApi = true;

			if (vulkan_supported) {
				UseApi = SupportedGpuApi.Vulkan;
			} else if (gles_supported) {
				if (gles_supported__v31)
					UseApi = SupportedGpuApi.GLES_V31;
				else if (gles_supported__v30)
					UseApi = SupportedGpuApi.GLES_V30;
				else if (gles_supported__v20)
					UseApi = SupportedGpuApi.GLES_V20;
				else {
					foundSupportedGpuApi = false;
				}
			} else {
				foundSupportedGpuApi = false;
			}

			//
			// Check if we were able to determine a gpuApi
			//
			if (foundSupportedGpuApi) {
				Log.e(TAG, "Unable to determine a supported GpuApi.");
				return false;
			}

			return true;
		}

		private static final String TAG = "GpuApiConfiguration";
		public SupportedGpuApi     UseApi;
		public GLESConfiguration   GLESConfiguration;
		public VulkanConfiguration VulkanConfiguration;
	}

	public native boolean initGpuAPI();

	// Package privte... Consequently below public methods.
	static {
		System.loadLibrary("ShirabeAndroidPlatformLayer_GpuAPI");
	}
}
