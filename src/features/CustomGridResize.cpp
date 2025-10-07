#pragma once
#include <Geode/modify/ObjectToolbox.hpp>
using namespace geode::prelude;

class $modify(ObjectToolbox) {
	float gridNodeSizeForKey(int id) {
		auto size = Mod::get()->template getSavedValue<float>("grid-size", 30.f);
		if (size < 1 || roundf(size) == 30) {
			return ObjectToolbox::gridNodeSizeForKey(id);
		}
		return size;
	}
};