#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <windows.h>
#include <filesystem>
namespace fs = std::filesystem;

// 执行系统命令并返回退出码
int runCommand(const std::string& cmd) {
    std::cout << "执行: " << cmd << std::endl;
    return std::system(cmd.c_str());
}

int main(int argc, char* argv[]) {

    system("chcp 65001");
    constexpr int MAX_RETRY = 10;
    constexpr int BUILD_DIR = 10;  // 秒
    const std::string buildDir = "build";

    // 收集额外参数（从第2个参数开始，类似)
    std::string extraArgs;
    for (int i = 1; i < argc; ++i) {
        extraArgs += " ";
        extraArgs += argv[i];
    }

    for (int i = 1; i <= MAX_RETRY; ++i) {
        std::cout << "\n=== 第 " << i << " 次尝试 ===" << std::endl;

        // 步骤1: cmake 配置
        std::string configCmd = "cmake -G Ninja -B \"" + buildDir + "\" -S ." + extraArgs;
        if (runCommand(configCmd) != 0) {
            std::cerr << "❌ CMake 配置失败，等待重试..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(BUILD_DIR));
            continue;
        }

        // 步骤2: cmake 构建
        std::string buildCmd = "cmake --build \"" + buildDir + "\" --config Release";
        if (runCommand(buildCmd) != 0) {
            std::cerr << "❌ CMake 构建失败，等待重试..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(BUILD_DIR));
            continue;
        }

        // 成功
        std::cout << "✅ 构建完成" << std::endl;

        //Run
        fs::path exePath = fs::current_path() / buildDir / "bin" / "main.exe";

        if (!fs::exists(exePath)) {
            std::cerr << "Executable not found: " << exePath << std::endl;
            return 0;
        }
        runCommand(exePath.string());
        return 0;
    }

    std::cerr << "❌ 达到最大重试次数，构建失败" << std::endl;
    return 1;
}