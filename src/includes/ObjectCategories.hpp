
namespace ErGui {
	struct ObjectConfig {
		int thumbnailObjectId;
		std::vector<int> objectIdVector;
	};

	//std::map<const char*, std::vector<int>> getObjectTabMap();
	std::map<const char*, std::vector<ObjectConfig>> getDefaultObjectCfg();
	
	inline std::map<const char*, std::vector<ObjectConfig>> objectCfg;

}