#include <Geode/Geode.hpp>

#include <Geode/modify/PlayerObject.hpp>

#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/sprite_nodes/CCSpriteFrame.h>

#include <Geode/binding/FMODAudioEngine.hpp>

#include <string>

using namespace geode::prelude;

int randInt(int min, int max) {
    return rand() % (max - min) + min;
}

$on_mod(Loaded) {
    srand(time(0));
}

class $modify(AltPlayerObject, PlayerObject){
	void playerDestroyed(bool p0) {
    	PlayerObject::playerDestroyed(p0);

		
		if (!Mod::get()->getSettingValue<bool>("practice_enabled")){
			if (PlayLayer::get()->m_isPracticeMode || PlayLayer::get()->m_isTestMode) return;
		}

		auto probability  = Mod::get()->getSettingValue<int64_t>("probability");
		if (randInt(1,probability) != 1){
			return;
		}
		
		auto anim = CCAnimation::create();
		anim->setDelayPerUnit(0.05);

		for (int i = 1; i <= 24; i++){
			std::string frameNum = "";
			if (i <= 9){
				frameNum = "00"+std::to_string(i);
			}else{
				frameNum = "0"+std::to_string(i);
			}

			std::string name = frameNum + ".png";
			std::string fullPath = geode::utils::string::pathToString(Mod::get()->getResourcesDir() / name);

			auto tex = CCTextureCache::sharedTextureCache()->addImage(fullPath.c_str(), false);
			if (!tex) continue;
			//tex->setAliasTexParameters();
			
			auto frame = CCSpriteFrame::createWithTexture(tex,
				{{0,0}, tex->getContentSize() }
			);

			anim->addSpriteFrame(frame);
		}

		auto center = CCDirector::get()->getWinSize() / 2;

		auto sprite = CCSprite::create();
		sprite->setTextureRect({ {0,0}, {0,0} });
		sprite->setPosition({ center.width, center.height - 10 });
		sprite->setScale(4.0f);

		CCDirector::get()->getRunningScene()->addChild(sprite, 999);

		auto animate = CCAnimate::create(anim);
		auto remove = CCCallFunc::create(sprite, callfunc_selector(CCNode::removeFromParent));
		
		// super scary sound
		auto fmodAE = FMODAudioEngine::sharedEngine();
		auto system = fmodAE->m_system;

		FMOD::Channel* channel;
		FMOD::Sound* sound;

		system->createSound(geode::utils::string::pathToString(Mod::get()->getResourcesDir() / "jumpscare.mp3").c_str(), FMOD_DEFAULT, nullptr, &sound);
		system->playSound(sound, nullptr, false, &channel);

		if (!Mod::get()->getSettingValue<bool>("full_volume")){
			channel->setVolume(fmodAE->getEffectsVolume());
		}
		//

		sprite->runAction(CCSequence::create(animate, remove, nullptr));
		
	}
};
