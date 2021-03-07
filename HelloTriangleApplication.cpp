#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <cstring>
#include <cstdlib>
#include <optional>
#include <algorithm>

const uint32_t WIDTH = 800, HEIGHT = 600;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily, presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

class HelloTriangleApplication {

public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}
	
private:

	GLFWwindow* window;

	vk::UniqueInstance vkInstance;
	vk::UniqueSurfaceKHR vkSurface;

	vk::PhysicalDevice vkPhysicalDevice;
	vk::UniqueDevice vkDevice;

	vk::Queue graphicsQueue, presentQueue;
	 
	vk::UniqueSwapchainKHR swapChain;
	std::vector<vk::Image> swapChainImages;
	std::vector<vk::UniqueImageView> swapChainImageViews;
	vk::Format swapChainImageFormat;
	vk::Extent2D swapChainExtent;

	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "TestApplication", nullptr, nullptr);
	}

	void initVulkan() {
		createInstance();
		setupDebugMessenger();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}

	void cleanup() {
		glfwDestroyWindow(window);

		glfwTerminate();
	}

	void createInstance() {
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("requested validation layers, are not available");
		}

		auto appInfo = vk::ApplicationInfo{
			"TestApplication",
			VK_MAKE_VERSION(1, 0, 0),
			"No Engine",
			VK_MAKE_VERSION(1, 0, 0),
			VK_API_VERSION_1_0
		};

		auto glfwExtensions = getRequiredExtensions();

		auto createInfo = vk::InstanceCreateInfo(
			vk::InstanceCreateFlags(),
			&appInfo,
			0, nullptr,
			static_cast<uint32_t>(glfwExtensions.size()), glfwExtensions.data()
		);

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}

		try {
			vkInstance = vk::createInstanceUnique(createInfo, nullptr);
		}
		catch (vk::SystemError err) {
			throw std::runtime_error("failed to create instance!");
		}
	}

	void setupDebugMessenger() {
		if (!enableValidationLayers) return;

		auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
			vk::DebugUtilsMessengerCreateFlagsEXT(),
			vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
			vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
			debugCallback,
			nullptr
		);

		vk::DispatchLoaderDynamic instanceLoader(*vkInstance, vkGetInstanceProcAddr);
		vkInstance->createDebugUtilsMessengerEXTUnique(createInfo, nullptr, instanceLoader);
	}

	void createSurface() {
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(*vkInstance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
		vkSurface = vk::UniqueSurfaceKHR(surface, *vkInstance);
	}

	bool checkValidationLayerSupport() {
		auto availableLayers = vk::enumerateInstanceLayerProperties();

		for (const char* layername : validationLayers) {
			bool foundLayer = false;

			for (const auto& layerProperty : availableLayers) {
				if (strcmp(layername, layerProperty.layerName) == 0) {
					foundLayer = true;
					break;
				}
			}
			if (!foundLayer) {
				return false;
			}
		}

		return true;
	}

	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(vkPhysicalDevice);

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (auto queueFamily : uniqueQueueFamilies) {
			queueCreateInfos.push_back({ vk::DeviceQueueCreateFlags(), queueFamily, 1, &queuePriority });
		}

		auto deviceFeatures = vk::PhysicalDeviceFeatures();

		auto deviceCreateInfo = vk::DeviceCreateInfo(
			vk::DeviceCreateFlags(),
			static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(),
			0, nullptr, // Layers
			static_cast<uint32_t>(deviceExtensions.size()), deviceExtensions.data(),
			&deviceFeatures);

		if (enableValidationLayers) {
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		}

		try {
			vkDevice = vkPhysicalDevice.createDeviceUnique(deviceCreateInfo);
		}
		catch (vk::SystemError err) {
			throw std::runtime_error("failed to create logical device!");
		}

		graphicsQueue = vkDevice->getQueue(indices.graphicsFamily.value(), 0);
		presentQueue = vkDevice->getQueue(indices.presentFamily.value(), 0);
	}

	void createSwapChain() {
		auto swapChainSupport = querySwapChainSupport(vkPhysicalDevice);

		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		vk::Extent2D extent = chooseSwapExtend(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		auto createInfo = vk::SwapchainCreateInfoKHR(
			vk::SwapchainCreateFlagsKHR(),
			*vkSurface,
			imageCount,
			surfaceFormat.format,
			surfaceFormat.colorSpace,
			extent,
			1,
			vk::ImageUsageFlagBits::eColorAttachment
		);

		QueueFamilyIndices indicies = findQueueFamilies(vkPhysicalDevice);
		uint32_t queueFamilyIndicies[] = { indicies.graphicsFamily.value(), indicies.presentFamily.value() };

		if (indicies.graphicsFamily != indicies.presentFamily) {
			createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndicies;
		}
		else {
			createInfo.imageSharingMode = vk::SharingMode::eExclusive;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

		try {
			swapChain = vkDevice->createSwapchainKHRUnique(createInfo);
		}
		catch (vk::SystemError err) {
			throw std::runtime_error("failed to create swap chain!");
		}

		swapChainImages = vkDevice->getSwapchainImagesKHR(*swapChain);

		swapChainImageFormat = surfaceFormat.format;
	}

	void createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); ++i) {
			auto createInfo = vk::ImageViewCreateInfo(
				vk::ImageViewCreateFlags(),
				swapChainImages[i],
				vk::ImageViewType::e2D,
				swapChainImageFormat,
				{vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity, vk::ComponentSwizzle::eIdentity},
				{
					vk::ImageAspectFlagBits::eColor,
					0,	// baseMipLevel
					1,	// levelCount
					0,	// baseArrayLayer
					1	// layerCount
				}
			);
			try {
				swapChainImageViews.push_back(vkDevice->createImageViewUnique(createInfo));
			}
			catch (vk::SystemError err) {
				throw std::runtime_error("failed to create image views!");
			}
		}
	}

	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
		return extensions;
	}

	void pickPhysicalDevice() {
		auto devices = vkInstance->enumeratePhysicalDevices();

		if (devices.size() == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				vkPhysicalDevice = device;
				return;
			}
		}
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	bool isDeviceSuitable(const vk::PhysicalDevice& device) {
		QueueFamilyIndices indices = findQueueFamilies(device);

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.isComplete() && extensionsSupported && swapChainAdequate;
	}

	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) {
		QueueFamilyIndices indices;

		auto families = device.getQueueFamilyProperties();

		int i = 0;
		for (const auto& family : families) {
			if (family.queueCount > 0 && family.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
			}
			if (family.queueCount > 0 && device.getSurfaceSupportKHR(i, *vkSurface)) {
				indices.presentFamily = i;
			}
			if (indices.isComplete()) {
				break;
			}
			
			++i;
		}
		return indices;
	}

	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) {
		return {
			device.getSurfaceCapabilitiesKHR(*vkSurface),
			device.getSurfaceFormatsKHR(*vkSurface),
			device.getSurfacePresentModesKHR(*vkSurface)
		};
	}

	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			}
		}

		return vk::PresentModeKHR::eFifo;
	}

	vk::Extent2D chooseSwapExtend(const vk::SurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != INT32_MAX) {
			return capabilities.currentExtent;
		}
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		return {
			std::clamp(static_cast<uint32_t>(width), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp(static_cast<uint32_t>(height), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
	}

	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device) {
		auto availableExtensions = device.enumerateDeviceExtensionProperties();

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
};