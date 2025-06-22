//#pragma once
//#include <map>
//#include <string>
//#include <memory>
//#include <mutex>
//#include "ModelComponent.h"
//#include "Material.h"
//
//class Graphics; // Deklaracja wyprzedzaj¹ca
//
//// Struktura przechowuj¹ca wszystkie dane modelu, które mo¿na wspó³dzieliæ
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
//    // Dostêp do instancji singletona
//    static ModelCache& GetInstance();
//
//    // G³ówna funkcja do pobierania danych modelu.
//    // Jeœli model jest w cache, zwraca go. Jeœli nie, wczytuje i zapisuje.
//    std::shared_ptr<CachedModelData> GetData(Graphics& gfx, const std::string& modelFile, bool isSkinned);
//
//    // Usuwamy mo¿liwoœæ kopiowania i przypisywania
//    ModelCache(const ModelCache&) = delete;
//    void operator=(const ModelCache&) = delete;
//
//private:
//    ModelCache() = default; // Prywatny konstruktor
//
//    // Funkcja wczytuj¹ca model z pliku - zostanie wywo³ana, gdy model nie istnieje w cache
//    std::shared_ptr<CachedModelData> LoadModel(Graphics& gfx, const std::string& modelFile, bool isSkinned);
//
//    std::map<std::string, std::shared_ptr<CachedModelData>> cache;
//    std::mutex mtx; // Mutex do zabezpieczenia dostêpu do mapy w œrodowisku wielow¹tkowym
//};