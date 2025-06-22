//#pragma once
//#include <map>
//#include <string>
//#include <memory>
//#include <mutex>
//#include "ModelComponent.h"
//#include "Material.h"
//
//class Graphics; // Deklaracja wyprzedzaj�ca
//
//// Struktura przechowuj�ca wszystkie dane modelu, kt�re mo�na wsp�dzieli�
//struct CachedModelData
//{
//    std::shared_ptr<Model
// 
// 
// 
// 
// 
// Node> rootNode;
//    std::vector<std::shared_ptr<Mesh>> meshPtrs;
//    std::map<std::string, ModelComponent::BoneInfo> boneInfoMap;
//    int boneCounter = 0;
//    bool isSkinned = false;
//};
//
//class ModelCache
//{
//public:
//    // Dost�p do instancji singletona
//    static ModelCache& GetInstance();
//
//    // G��wna funkcja do pobierania danych modelu.
//    // Je�li model jest w cache, zwraca go. Je�li nie, wczytuje i zapisuje.
//    std::shared_ptr<CachedModelData> GetData(Graphics& gfx, const std::string& modelFile, bool isSkinned);
//
//    // Usuwamy mo�liwo�� kopiowania i przypisywania
//    ModelCache(const ModelCache&) = delete;
//    void operator=(const ModelCache&) = delete;
//
//private:
//    ModelCache() = default; // Prywatny konstruktor
//
//    // Funkcja wczytuj�ca model z pliku - zostanie wywo�ana, gdy model nie istnieje w cache
//    std::shared_ptr<CachedModelData> LoadModel(Graphics& gfx, const std::string& modelFile, bool isSkinned);
//
//    std::map<std::string, std::shared_ptr<CachedModelData>> cache;
//    std::mutex mtx; // Mutex do zabezpieczenia dost�pu do mapy w �rodowisku wielow�tkowym
//};