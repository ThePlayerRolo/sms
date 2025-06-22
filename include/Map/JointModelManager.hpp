#ifndef MAP_JOINT_MODEL_MANAGER_HPP
#define MAP_JOINT_MODEL_MANAGER_HPP

#include <Map/JointModel.hpp>
#include <M3DUtil/MActorData.hpp>
#include <JSystem/JDrama/JDRViewObj.hpp>

class TJointModel;

class TJointModelManager : public JDrama::TViewObj {
public:
	TJointModelManager(const char*);

	virtual ~TJointModelManager() { }
	virtual void perform(u32, JDrama::TGraphics*);
	virtual TJointModel* newJointModel(int) const { return new TJointModel; }

	void initJointModel(const char*, const char**);

	// fabricated
	const char* getFolder() { return mModelsFolder; }
	int getJointModelNum() const { return mJointModelNum; }
	TJointModel* getJointModel(int i) { return mJointModels[i]; }
	const TJointModel* getJointModel(int i) const { return mJointModels[i]; }
	MActorAnmData* getMActorAnmData() { return &mActorAnimationData; }

public:
	/* 0x10 */ int mJointModelNum;
	/* 0x14 */ TJointModel** mJointModels;
	/* 0x18 */ u32 unk18;
	/* 0x1C */ const char* mModelsFolder;
	/* 0x20 */ MActorAnmData mActorAnimationData;
};

#endif
