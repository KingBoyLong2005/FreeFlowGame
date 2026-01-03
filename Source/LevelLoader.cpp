#include "LevelLoader.h"
#include "axmol/platform/FileUtils.h"
#include "rapidjson/document.h"

using namespace ax;

bool LevelLoader::loadFromFile(const std::string& filename, LevelData& outData)
{
    printf("\n=== LevelLoader::loadFromFile ===\n");
    printf("Attempting to load: %s\n", filename.c_str());
    
    std::string jsonStr = FileUtils::getInstance()->getStringFromFile(filename);
    if (jsonStr.empty())
    {
        printf("❌ ERROR: Cannot read file %s or file is empty\n", filename.c_str());
        printf("Full path: %s\n", FileUtils::getInstance()->fullPathForFilename(filename).c_str());
        return false;
    }

    printf("✓ File read successfully, size: %zu bytes\n", jsonStr.size());
    printf("First 100 chars: %s\n", jsonStr.substr(0, 100).c_str());

    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());

    if (doc.HasParseError())
    {
        printf("❌ ERROR: JSON parse error at offset %zu: %d\n", 
               doc.GetErrorOffset(), doc.GetParseError());
        return false;
    }

    if (!doc.IsObject())
    {
        printf("❌ ERROR: JSON root is not an object\n");
        return false;
    }

    if (!doc.HasMember("rows") || !doc.HasMember("cols"))
    {
        printf("❌ ERROR: Missing 'rows' or 'cols' in JSON\n");
        return false;
    }

    outData.rows = doc["rows"].GetInt();
    outData.cols = doc["cols"].GetInt();

    printf("✓ Level dimensions: %dx%d\n", outData.rows, outData.cols);

    // Load tiles (optional)
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
        printf("✓ Tiles loaded: %zu rows\n", outData.tiles.size());
    }

    // Load objects (CRITICAL)
    if (!doc.HasMember("objects"))
    {
        printf("❌ ERROR: No 'objects' field in JSON\n");
        return false;
    }

    const rapidjson::Value& objects = doc["objects"];
    if (!objects.IsArray())
    {
        printf("❌ ERROR: 'objects' is not an array\n");
        return false;
    }

    printf("Objects array size: %u\n", objects.Size());
    
    outData.objects.resize(outData.rows);

    for (int r = 0; r < outData.rows && r < (int)objects.Size(); r++)
    {
        outData.objects[r].resize(outData.cols, "");
        
        if (objects[r].IsArray())
        {
            for (int c = 0; c < outData.cols && c < (int)objects[r].Size(); c++)
            {
                if (objects[r][c].IsString())
                {
                    outData.objects[r][c] = objects[r][c].GetString();
                    
                    if (!outData.objects[r][c].empty())
                    {
                        printf("  Object at (%d,%d): '%s'\n", r, c, outData.objects[r][c].c_str());
                    }
                }
            }
        }
    }

    printf("✓ Objects loaded: %zu rows\n", outData.objects.size());
    printf("=== LevelLoader::loadFromFile COMPLETE ===\n\n");
    return true;
}