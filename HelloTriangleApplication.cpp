#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <optional>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;

	bool isComplete() {
		return graphicsFamily.has_value();
	}
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
	vk::PhysicalDevice vkDevice;

	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(WIDTH, HEIGHT, "TestApplication", nullptr, nullptr);
	}

	void initVulkan() {
		createInstance();
		setupDebugMessenger();
		pickPhysicalDevice();
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

		vkInstance = vk::createInstanceUnique(createInfo, nullptr);
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
				vkDevice = device;
				return;
			}
		}
		throw std::runtime_error("failed to find a suitable GPU!");
	}

	bool isDeviceSuitable(const vk::PhysicalDevice& device) {
		QueueFamilyIndices indices = findQueueFamilies(device);
		return indices.isComplete();
	}

	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) {
		QueueFamilyIndices indices;

		auto families = device.getQueueFamilyProperties();

		int i = 0;
		for (const auto& family : families) {
			if (family.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
			}
			if (indices.isComplete()) {
				break;
			}
			++i;
		}

		return indices;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
};