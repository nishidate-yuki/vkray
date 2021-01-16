

#include "../vkray.hpp"

class Application
{
public:

    void run()
    {
        window = std::make_unique<vkr::Window>("vkray", 800, 600);
        instance = std::make_unique<vkr::Instance>(*window, true);
        device = std::make_unique<vkr::Device>(*instance);
        swapChain = std::make_unique<vkr::SwapChain>(*device);

        outputImage = swapChain->createOutputImage();

        // Create BLAS
        std::vector<vkr::Vertex> vertices{
            { {1.0f, 1.0f, 0.0f} },
            { {-1.0f, 1.0f, 0.0f} },
            { {0.0f, -1.0f, 0.0f} } };
        std::vector<uint32_t> indices = { 0, 1, 2 };
        blas = std::make_unique<vkr::BottomLevelAccelerationStructure>(*device, vertices, indices);

        // Create TLAS
        vkr::AccelerationStructureInstance instance{ 0, glm::mat4(1), 0 };
        tlas = std::make_unique<vkr::TopLevelAccelerationStructure>(*device, *blas, instance);

        // Create Descriptor Set Layout
        descSets = std::make_unique<vkr::DescriptorSets>(*device, 1);
        descSets->addBindging(0, 0, vk::DescriptorType::eAccelerationStructureKHR, 1, vk::ShaderStageFlagBits::eRaygenKHR);
        descSets->addBindging(0, 1, vk::DescriptorType::eStorageImage, 1, vk::ShaderStageFlagBits::eRaygenKHR);

        // Load shaders
        shaderManager = std::make_unique<vkr::ShaderManager>(*device);
        shaderManager->addShader("samples/shaders/raygen.rgen.spv", vk::ShaderStageFlagBits::eRaygenKHR, "main", vk::RayTracingShaderGroupTypeKHR::eGeneral);
        shaderManager->addShader("samples/shaders/miss.rmiss.spv", vk::ShaderStageFlagBits::eMissKHR, "main", vk::RayTracingShaderGroupTypeKHR::eGeneral);
        shaderManager->addShader("samples/shaders/closesthit.rchit.spv", vk::ShaderStageFlagBits::eClosestHitKHR, "main", vk::RayTracingShaderGroupTypeKHR::eTrianglesHitGroup);

        window->run(); // TODO: 制御取る
    }

private:

    std::unique_ptr<vkr::Window> window;
    std::unique_ptr<vkr::Instance> instance;
    std::unique_ptr<vkr::Device> device;
    std::unique_ptr<vkr::SwapChain> swapChain;

    std::unique_ptr<vkr::Image> outputImage;
    std::unique_ptr<vkr::BottomLevelAccelerationStructure> blas;
    std::unique_ptr<vkr::TopLevelAccelerationStructure> tlas;

    std::unique_ptr<vkr::DescriptorSets> descSets;
    std::unique_ptr<vkr::ShaderManager> shaderManager;

};

int main()
{
    Application app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
