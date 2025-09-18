#pragma once
#include "ObjectCategories.hpp"
using namespace geode::prelude;

template <>
struct matjson::Serialize<ErGui::ObjectConfig> {
	static geode::Result<ErGui::ObjectConfig> fromJson(const matjson::Value& value) {
		GEODE_UNWRAP_INTO(int thumbnailObjectId, value["thumbnailObjectId"].asInt());
		GEODE_UNWRAP_INTO(std::vector<matjson::Value> arr, value["objectIdVector"].asArray());

		// Преобразование каждого элемента в int
		std::vector<int> vec;
		vec.reserve(arr.size());
		for (auto& item : arr) {
			GEODE_UNWRAP_INTO(int id, item.asInt());
			vec.push_back(id);
		}

		return geode::Ok(ErGui::ObjectConfig{
			thumbnailObjectId,
			std::move(vec)
			});
	}

	static matjson::Value toJson(const ErGui::ObjectConfig& obj) {
		// Собираем JSON-массив из вектора int
		std::vector<matjson::Value> arr;
		arr.reserve(obj.objectIdVector.size());
		for (int id : obj.objectIdVector) {
			arr.emplace_back(id);
		}
		// Value(std::vector<matjson::Value>) создаёт JSON-массив :contentReference[oaicite:0]{index=0}

		// Собираем объект
		return matjson::makeObject({
			{ "thumbnailObjectId", obj.thumbnailObjectId },
			{ "objectIdVector", matjson::Value(std::move(arr)) }
			}); // makeObject(...) для создания JSON-объекта :contentReference[oaicite:1]{index=1}
	}
};


$on_mod(Loaded) {
	auto cfgDir = Mod::get()->getSettingValue<std::filesystem::path>("object-list-config");

	std::ifstream cfgFile = std::ifstream(cfgDir);
	geode::Result parsed = matjson::parse(cfgFile);

	if (!parsed) {
		matjson::Value j;
		for (auto key : ErGui::keyOrder) {
			j[key] = ErGui::getDefaultObjectCfg()[key];
		}

		auto someError = parsed.unwrapErr();
		if (parsed.unwrapErr().message == "eof") {
			std::ofstream oCfgFile = std::ofstream(cfgDir);
			oCfgFile.write(j.dump().c_str(), j.dump().size());
			oCfgFile.close();
		}
		std::cout << "Failed to parse json: " << someError.message << " " << typeid(someError).name() << "\n";

		ErGui::objectCfg = ErGui::getDefaultObjectCfg();

	}
	else {
		ErGui::objectCfg.clear();
		// ErGui::keyOrder.clear();
		matjson::Value parsedObject = parsed.unwrap();
		std::map<std::string, std::vector<ErGui::ObjectConfig>> newData;

		for (const auto& [key, mapValue] : parsedObject) {
			// ErGui::keyOrder.push_back(key);
			auto myVectorOfObjCfg = mapValue.asArray().unwrap();
			std::vector<ErGui::ObjectConfig> objCfgVector;
			for (const auto& objCfgValue : myVectorOfObjCfg) {
				auto unwrappedObjCfg = objCfgValue.as<ErGui::ObjectConfig>().unwrap();
				objCfgVector.push_back(unwrappedObjCfg);
			}

			ErGui::objectCfg[key] = objCfgVector;
		}
	}

	cfgFile.close();
}