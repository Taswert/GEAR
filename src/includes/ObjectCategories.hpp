
namespace ErGui {
	struct ObjectConfig {
		int thumbnailObjectId;
		std::vector<int> objectIdVector;
		bool isOpen = false;
	};

	std::map<const char*, std::vector<int>> getObjectTabMap();
	std::map<const char*, std::vector<ObjectConfig>> getObjectCfg();
}