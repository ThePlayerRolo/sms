#ifndef STRATEGIC_MIRROR_ACTOR_HPP
#define STRATEGIC_MIRROR_ACTOR_HPP

#include <JSystem/JDrama/JDRViewObj.hpp>

class J3DModel;

class TMirrorActor : public JDrama::TViewObj {
public:
	void isInMirror() const;
	void checkIsInMirror();
	void perform(u32, JDrama::TGraphics*);
	static void entryMirrorDrawBufferAlways(J3DModel*);
	void init(J3DModel*, u16);
	TMirrorActor(const char*);

public:
	/* 0x10 */ J3DModel* unk10;
	/* 0x14 */ J3DModel* unk14;
	/* 0x18 */ u8 unk18;
	/* 0x1A */ u16 unk1A;
};

#endif
