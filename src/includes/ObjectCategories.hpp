
namespace ErGui {
	struct ObjectConfig {
		int thumbnailObjectId;
		std::vector<int> objectIdVector;
	};

	//std::map<const char*, std::vector<int>> getObjectTabMap();
	std::map<std::string, std::vector<ObjectConfig>> getDefaultObjectCfg();

	inline const char* CUSTOM_TAB_KEY = "Custom";
	inline const char* RECENT_TAB_KEY = "Recent";
	inline const char* FAVOURITES_TAB_KEY = "Favourites";
	
	inline std::map<std::string, std::vector<ObjectConfig>> objectCfg;
	inline const std::vector<std::string> keyOrder = {"Blocks", "Slabs", "Outlines", "Slopes", 
		"Hazards", "3D Objects", "Special", "Animated", "Pixel", "Collectables", "Particles", 
		"Deco", "Pulsators", "Saws", "Triggers", CUSTOM_TAB_KEY, RECENT_TAB_KEY, FAVOURITES_TAB_KEY};


}