
namespace ErGui {
	struct ObjectConfig {
		int thumbnailObjectId;
		std::vector<int> objectIdVector;
	};

	//std::map<const char*, std::vector<int>> getObjectTabMap();
	std::map<std::string, std::vector<ObjectConfig>> getDefaultObjectCfg();
	
	inline std::map<std::string, std::vector<ObjectConfig>> objectCfg;
	inline std::vector<std::string> keyOrder = {"Blocks", "Slabs", "Outlines", "Slopes", "Hazards", "3D Objects", "Special", "Animated", "Pixel", "Collectables", "Particles", "Deco", "Pulsators", "Saws", "Triggers"};

}