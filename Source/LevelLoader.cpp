#include "LevelLoader.h"
#include "axmol/platform/FileUtils.h"
#include "rapidjson/document.h"

using namespace ax;

bool LevelLoader::loadFromFile(const std::string& filename, LevelData& outData)
{
    std::string jsonStr = FileUtils::getInstance()->getStringFromFile(filename);
    if (jsonStr.empty())
    {
        printf("ERROR: Cannot read file %s\n", filename.c_str());
        return false;
    }

    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());

    if (!doc.IsObject())
    {
        printf("ERROR: Invalid JSON format\n");
        return false;
    }

    if (!doc.HasMember("rows") || !doc.HasMember("cols"))
    {
        printf("ERROR: Missing rows or cols\n");
        return false;
    }

    outData.rows = doc["rows"].GetInt();
    outData.cols = doc["cols"].GetInt();

    printf("Loading level: %dx%d\n", outData.rows, outData.cols);

    // Load tiles
    if (doc.HasMember("tiles") && doc["tiles"].IsArray())
    {
        const rapidjson::Value& tiles = doc["tiles"];
        outData.tiles.resize(outData.rows);
        
        for (int r = 0; r < outData.rows && r < (int)tiles.Size(); r++)
        {
            if (tiles[r].IsArray())
            {
                for (int c = 0; c < outData.cols && c < (int)tiles[r].Size(); c++)
                {
                    outData.tiles[r].push_back(tiles[r][c].GetInt());
                }
            }
        }
    }

    // Load objects
    if (doc.HasMember("objects") && doc["objects"].IsArray())
    {
        const rapidjson::Value& objects = doc["objects"];
        outData.objects.resize(outData.rows);

        for (int r = 0; r < outData.rows && r < (int)objects.Size(); r++)
        {
            outData.objects[r].resize(outData.cols);
            
            if (objects[r].IsArray())
            {
                for (int c = 0; c < outData.cols && c < (int)objects[r].Size(); c++)
                {
                    outData.objects[r][c] = objects[r][c].GetString();
                    
                    if (!outData.objects[r][c].empty())
                    {
                        printf("Object at (%d,%d): %s\n", r, c, outData.objects[r][c].c_str());
                    }
                }
            }
        }
    }

    printf("Level loaded successfully\n");
    return true;
}