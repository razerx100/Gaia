#include <filesystem>
#include <fstream>
#include <regex>

std::string GenerateXMLTag(
    const std::string& tagName, const std::string& mode, const std::string& value
) noexcept {
    return std::string("<") + tagName + " Condition=\"'$(Configuration)|$(Platform)'=='" + mode
        + "|x64'\">" + value + "</" + tagName + ">";
}

void FindAndAdd(const std::string& projectDirectory) noexcept {
    std::string fileName = projectDirectory + "\\src\\shaders\\Shaders.vcxproj";

    std::string includeDirectory = std::filesystem::absolute(std::filesystem::current_path()).string()
        + "\\src\\shaders\\include";

    std::ifstream readFile(
        fileName.c_str(),
        std::ios_base::in
    );

    std::regex vsSearch(
        "<FXCompile.+VS[a-z]+\.hlsl\">",
        std::regex_constants::icase
    );
    std::regex psSearch(
        "<FXCompile.+PS[a-z]+\.hlsl\">",
        std::regex_constants::icase
    );
    std::regex rsSearch(
        "<FXCompile.+RS[a-z]+\.hlsl\">",
        std::regex_constants::icase
    );

    std::string fixedText;
    for (std::string line; std::getline(readFile, line);) {
        fixedText.append(line + "\n");

        if (std::regex_search(line, vsSearch))
            fixedText.append(
                GenerateXMLTag("ShaderType", "Debug", "Vertex") + "\n"
                + GenerateXMLTag("ShaderModel", "Debug", "5.1") + "\n"
                + GenerateXMLTag("ShaderType", "Release", "Vertex") + "\n"
                + GenerateXMLTag("ShaderModel", "Release", "5.1") + "\n"
                + GenerateXMLTag("AdditionalIncludeDirectories", "Debug", includeDirectory) + "\n"
                + GenerateXMLTag("AdditionalIncludeDirectories", "Release", includeDirectory) + "\n"
            );
        else if (std::regex_search(line, psSearch))
            fixedText.append(
                GenerateXMLTag("ShaderType", "Debug", "Pixel") + "\n"
                + GenerateXMLTag("ShaderModel", "Debug", "5.1") + "\n"
                + GenerateXMLTag("ShaderType", "Release", "Pixel") + "\n"
                + GenerateXMLTag("ShaderModel", "Release", "5.1") + "\n"
                + GenerateXMLTag("AdditionalIncludeDirectories", "Debug", includeDirectory) + "\n"
                + GenerateXMLTag("AdditionalIncludeDirectories", "Release", includeDirectory) + "\n"
            );
        else if (std::regex_search(line, rsSearch))
            fixedText.append(
                GenerateXMLTag("ShaderType", "Debug", "RootSignature") + "\n"
                + GenerateXMLTag("ShaderModel", "Debug", "rootsig_1.1") + "\n"
                + GenerateXMLTag("ShaderType", "Release", "RootSignature") + "\n"
                + GenerateXMLTag("ShaderModel", "Release", "rootsig_1.1") + "\n"
            );
    }
    readFile.close();

    std::ofstream writeFile(
        fileName.c_str(),
        std::ios_base::out | std::ios_base::trunc
    );

    writeFile << fixedText;
}

int main(int argc, char** argv) {
    if (argc > 1)
        FindAndAdd(argv[1]);

    return 0;
}