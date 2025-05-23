#include <JSystem/JParticle/JPABaseShape.hpp>
#include <JSystem/JParticle/JPAMath.hpp>
#include <dolphin/gx/GXStruct.h>

GXBlendMode JPABaseShape::stBlendMode[] = {
	GX_BM_NONE,
	GX_BM_BLEND,
	GX_BM_LOGIC,
	GX_BM_NONE,
};

GXBlendFactor JPABaseShape::stBlendFactor[] = {
	GX_BL_ZERO,     GX_BL_ONE,         GX_BL_SRCCLR,   GX_BL_INVSRCCLR,
	GX_BL_SRCCLR,   GX_BL_INVSRCCLR,   GX_BL_SRCALPHA, GX_BL_INVSRCALPHA,
	GX_BL_DSTALPHA, GX_BL_INVDSTALPHA,
};

GXLogicOp JPABaseShape::stLogicOp[] = {
	GX_LO_CLEAR,  GX_LO_SET,    GX_LO_COPY,  GX_LO_INVCOPY,
	GX_LO_NOOP,   GX_LO_INV,    GX_LO_AND,   GX_LO_NAND,
	GX_LO_OR,     GX_LO_NOR,    GX_LO_XOR,   GX_LO_EQUIV,
	GX_LO_REVAND, GX_LO_INVAND, GX_LO_REVOR, GX_LO_INVOR,
};

GXCompare JPABaseShape::stCompare[] = {
	GX_NEVER,  GX_LESS,   GX_LEQUAL,  GX_EQUAL,
	GX_NEQUAL, GX_GEQUAL, GX_GREATER, GX_ALWAYS,
};

GXAlphaOp JPABaseShape::stAlphaOp[] = {
	GX_AOP_AND,
	GX_AOP_OR,
	GX_AOP_XOR,
	GX_AOP_XNOR,
};

struct JPAColorRegAnmKey {
	s16 unk0;
	GXColor unk2;
};

static GXColor* makeColorTable(JPAColorRegAnmKey* param_1, int param_2,
                               int param_3, JKRHeap* param_4)
{
	GXColor* result
	    = (GXColor*)JKRHeap::alloc((param_3 + 1) * sizeof(GXColor), 4, param_4);

	f32 accumR, accumG, accumB, accumA;
	f32 fR, fG, fB, fA;
	f32 thing;

	fR = (f32)param_1[0].unk2.r;
	fG = (f32)param_1[0].unk2.g;
	fB = (f32)param_1[0].unk2.b;
	fA = (f32)param_1[0].unk2.a;

	accumR = accumG = accumB = accumA = 0.0f;

	int nextKey = 0;
	for (int i = 0; i < param_3 + 1; ++i) {
		if (i == param_1[nextKey].unk0) {
			result[i] = param_1[nextKey].unk2;

			fR = (f32)param_1[nextKey].unk2.r;
			fG = (f32)param_1[nextKey].unk2.g;
			fB = (f32)param_1[nextKey].unk2.b;
			fA = (f32)param_1[nextKey].unk2.a;

			++nextKey;

			if (nextKey < param_2) {
				JPAColorRegAnmKey* key = &param_1[nextKey];

				thing = 1.0f / (key->unk0 - key[-1].unk0);

				accumR = thing * (key->unk2.r - fR);
				accumG = thing * (key->unk2.g - fG);
				accumB = thing * (key->unk2.b - fB);
				accumA = thing * (key->unk2.a - fA);
			} else {
				accumR = accumG = accumB = accumA = 0.0f;
			}
		} else {
			fR += accumR;
			fG += accumG;
			fB += accumB;
			fA += accumA;
			result[i].r = fR;
			result[i].g = fG;
			result[i].b = fB;
			result[i].a = fA;
		}
	}

	return result;
}

JPABaseShape::JPABaseShape(const u8* data, JKRHeap* heap)
{
	mBaseSizeY  = *(f32*)(data + 0x18);
	mBaseSizeX  = *(f32*)(data + 0x1c);
	mLoopOffset = *(s16*)(data + 0x20);
	mType       = data[0x24];
	mDirType    = data[0x25];
	mRotType    = data[0x26];

	u8 r5          = data[0x22];
	u8 r6          = data[0x23];
	mColLoopOffset = (r5 & 1) ? 0xffff : 0;
	mTexLoopOffset = (r6 & 1) ? 0xffff : 0;

	// TODO: something awful is happening here (no inlines involved?!)
	u8 r5_3 = true;
	u8 r5_2 = 1;
	if (!(r5 & 2) && mType != 5) {
		r5_3 = false;
	}

	if (!r5_3 && (mType != 6)) {
		r5_2 = 0;
	}

	u8 r6_2 = r5_2 ? 1 : 0;

	u8 r5_4;
	u8 r4;
	r5_4 = true;
	r4   = 1;
	if (!(r6 & 2) && mType != 5) {
		r5_4 = false;
	}

	if (!r5_4 && (mType != 6)) {
		r4 = 0;
	}

	u8 r0 = r4 ? 1 : 0;

	unk68 = r6_2 << 1 | r0;

	switch (data[0x30]) {
	case 0:
		unk48.unk0 = GX_CC_ZERO;
		unk48.unk4 = GX_CC_TEXC;
		unk48.unk8 = GX_CC_ONE;
		unk48.unkC = GX_CC_ZERO;
		break;
	case 1:
		unk48.unk0 = GX_CC_ZERO;
		unk48.unk4 = GX_CC_C0;
		unk48.unk8 = GX_CC_TEXC;
		unk48.unkC = GX_CC_ZERO;
		break;
	case 2:
		unk48.unk0 = GX_CC_C0;
		unk48.unk4 = GX_CC_ONE;
		unk48.unk8 = GX_CC_TEXC;
		unk48.unkC = GX_CC_ZERO;
		break;
	case 3:
		unk48.unk0 = GX_CC_C1;
		unk48.unk4 = GX_CC_C0;
		unk48.unk8 = GX_CC_TEXC;
		unk48.unkC = GX_CC_ZERO;
		break;
	case 4:
		unk48.unk0 = GX_CC_ZERO;
		unk48.unk4 = GX_CC_TEXC;
		unk48.unk8 = GX_CC_C0;
		unk48.unkC = GX_CC_C1;
		break;
	case 5:
		unk48.unk0 = GX_CC_ZERO;
		unk48.unk4 = GX_CC_ZERO;
		unk48.unk8 = GX_CC_ZERO;
		unk48.unkC = GX_CC_C0;
		break;
	}

	unk6C                 = data[0x31];
	mBlendMode1Index      = data[0x35];
	mSrcBlendFactor1Index = data[0x36];
	mDstBlendFactor1Index = data[0x37];
	mBlendOp1Index        = data[0x38];
	mAlphaCmpComp0Index   = data[0x39];
	mAlphaCmpRef0         = data[0x3a];
	mAlphaCmpOpIndex      = data[0x3b];
	mAlphaCmpComp1Index   = data[0x3c];
	mAlphaCmpRef1         = data[0x3d];
	mZCompLoc             = !(data[0x3e] >> 1 & 1);
	unk77                 = data[0x3f];
	mZCmpFunctionIndex    = data[0x40];
	unk79                 = data[0x41];
	unk7A                 = data[0x42];
	unk7B                 = data[0x43];
	mFlags                = data[0x44];
	unk7D                 = data[0x4d];
	mTextureAnmKeyNum     = data[0x4e];
	mTextureIndex         = data[0x4f];
	unk80                 = (data[0x4c] & 1) && mTextureAnmKeyNum != 0 ? 1 : 0;
	unk81                 = (data[0x4c] & 2) ? 0 : 1;
	mColorRegAnmMaxFrm    = *(s16*)(data + 0x5c);
	unk82                 = data[0x5e];
	unk83                 = data[0x60];
	unk84                 = data[0x61];
	unk85                 = data[0x62];
	unk86                 = data[99];
	mPrmColor             = *(GXColor*)(data + 100);
	mEnvColor             = *(GXColor*)(data + 0x68);
	mTexStaticTransX      = JPAConvertFixToFloat(*(s16*)(data + 0x80)) * 10.0f;
	mTexStaticScaleX      = JPAConvertFixToFloat(*(s16*)(data + 0x82)) * 10.0f;
	mTexStaticTransY      = JPAConvertFixToFloat(*(s16*)(data + 0x84)) * 10.0f;
	mTexStaticScaleY      = JPAConvertFixToFloat(*(s16*)(data + 0x86)) * 10.0f;
	mTilingX              = JPAConvertFixToFloat(*(s16*)(data + 0x88)) * 10.0f;
	mTilingY              = JPAConvertFixToFloat(*(s16*)(data + 0x8a)) * 10.0f;
	mTexScrollTransX      = JPAConvertFixToFloat(*(s16*)(data + 0x8c));
	mTexScrollScaleX      = JPAConvertFixToFloat(*(s16*)(data + 0x8e));

	mTexScrollTransY = JPAConvertFixToFloat(*(s16*)(data + 0x90)) * 0.1f;
	mTexScrollScaleY = JPAConvertFixToFloat(*(s16*)(data + 0x92)) * 0.1f;
	mTexScrollRotate = JPAConvertFixToFloat(*(s16*)(data + 0x94));
	unk87            = data[0x96];

	if (unk80 != 0) {
		mTextureIndices = (u8*)JKRHeap::alloc(mTextureAnmKeyNum, 4, heap);
		memcpy(mTextureIndices, data + *(s16*)(data + 0x12), mTextureAnmKeyNum);
	} else {
		mTextureIndices = nullptr;
	}

	if (unk83 & 2) {
		mPrmColors
		    = makeColorTable((JPAColorRegAnmKey*)(data + *(s16*)(data + 0x14)),
		                     unk85, mColorRegAnmMaxFrm, heap);
	} else {
		mPrmColors = nullptr;
	}

	if (unk84 & 2) {
		mEnvColors
		    = makeColorTable((JPAColorRegAnmKey*)(data + *(s16*)(data + 0x16)),
		                     unk86, mColorRegAnmMaxFrm, heap);
	} else {
		mEnvColors = nullptr;
	}
}
