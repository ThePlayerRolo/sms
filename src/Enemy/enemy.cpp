#include <Enemy/Enemy.hpp>
#include <Enemy/Graph.hpp>
#include <Enemy/EnemyManager.hpp>
#include <Enemy/Conductor.hpp>
#include <Map/Map.hpp>
#include <Map/MapData.hpp>
#include <Player/MarioAccess.hpp>
#include <M3DUtil/MActor.hpp>
#include <MarioUtil/MathUtil.hpp>
#include <Strategic/Spine.hpp>
#include <JSystem/J3D/J3DGraphAnimator/J3DAnimation.hpp>
#include <JSystem/J3D/J3DGraphAnimator/J3DModel.hpp>
#include <JSystem/JDrama/JDRNameRefGen.hpp>
#include <JSystem/JMath.hpp>

TSpineEnemy::TSpineEnemy(const char* name)
    : TLiveActor(name)
    , unkF4(0)
    , unkF8(0.0f, 0.0f, 0.0f)
    , unk104(0)
    , unk108(0.0f, 0.0f, 0.0f)
    , unk114(4)
    , unk128(0)
    , unk12C(0.0f)
    , unk130(0)
    , unk134(0.0f)
    , unk138(0)
    , unk13C(1)
    , unk140(1.0f)
    , unk144(5.0f)
    , unk148(1.0f)
    , unk14C(1.0f)
{
	unk8C  = new TSpineBase<TLiveActor>(this);
	unk124 = new TGraphTracer;
}

TSpineEnemy::~TSpineEnemy() { }

void TSpineEnemy::init(TLiveManager* param_1)
{
	TLiveActor::init(param_1);
	TSpineEnemyParams* params = getSaveParam();
	if (params) {
		unkBC  = params->mSLBodyRadius.get();
		unk14C = params->mSLWallRadius.get();
		unkC0  = params->mSLHeadHeight.get();
		unkB8  = unk148 * unkBC;
	}
}

void TSpineEnemy::reset()
{
	unk124->mPrevIdx = -1;
	goToShortestNextGraphNode();
	offLiveFlag(0x1);
	offLiveFlag(0x8);
	offLiveFlag(0x10);
	offLiveFlag(0x4);
	onLiveFlag(0x800);
	unk8C->reset();
	unk8C->pushDefault();
}

void TSpineEnemy::load(JSUMemoryInputStream& stream)
{
	JDrama::TActor::load(stream);

	char buffer[256];
	stream.readString(buffer, 256);
	TLiveManager* mgr = JDrama::TNameRefGen::search<TLiveManager>(buffer);

	stream.readString(buffer, 256);
	unk124->unk0 = gpConductor->getGraphByName(buffer);

	unkC4 = TMap::getIllegalCheckData();

	init(mgr);
}

void TSpineEnemy::calcEnemyRootMatrix()
{
	J3DModel* pJVar13 = getModel();
	MtxPtr mtx        = pJVar13->getBaseTRMtx();

	pJVar13->unk14 = mScaling;

	if (unk130 == 0) {
		MsMtxSetRotRPH(mtx, mRotation.x, mRotation.y, mRotation.z);
	} else {
		if (unk130 >= 2 && !(checkLiveFlag(0x80) ? TRUE : FALSE) && unk138) {
			JGeometry::TVec3<f32> v1(0.0f, 1.0f, 0.0f);

			JGeometry::TVec3<f32> v2 = unk138->mNormal;

			v1.cross(v2, v1);
			v1.normalize();

			JGeometry::TVec3<f32> v3;
			v3.cross(v1, v2);
			v3.normalize();

			mtx[0][0] = v1.x;
			mtx[1][0] = v1.y;
			mtx[2][0] = v1.z;
			mtx[0][1] = v2.x;
			mtx[1][1] = v2.y;
			mtx[2][1] = v2.z;
			mtx[0][2] = v3.x;
			mtx[1][2] = v3.y;
			mtx[2][2] = v3.z;
			mtx[0][3] = 0.0;
			mtx[1][3] = 0.0;
			mtx[2][3] = 0.0;
		} else {
			if (unk130 >= 1 && !unkC4->checkFlag(0x10)) {
				JGeometry::TVec3<f32> v1(
				    JMASSin(mRotation.y * (65536.0f / 360.0f)), 0.0f,
				    JMASCos(mRotation.y * (65536.0f / 360.0f)));

				JGeometry::TVec3<f32> v2 = unkC4->mNormal;
				v1.cross(v2, v1);
				v1.normalize();

				JGeometry::TVec3<f32> v3;
				v3.cross(v1, v2);
				v3.normalize();

				mtx[0][0] = v1.x;
				mtx[1][0] = v1.y;
				mtx[2][0] = v1.z;
				mtx[0][1] = v2.x;
				mtx[1][1] = v2.y;
				mtx[2][1] = v2.z;
				mtx[0][2] = v3.x;
				mtx[1][2] = v3.y;
				mtx[2][2] = v3.z;
				mtx[0][3] = 0.0;
				mtx[1][3] = 0.0;
				mtx[2][3] = 0.0;
			} else {
				MsMtxSetRotRPH(mtx, mRotation.x, mRotation.y, mRotation.z);
			}
		}
	}

	mtx[0][3] = mPosition.x;
	mtx[1][3] = mPosition.y;
	mtx[2][3] = mPosition.z;
}

void TSpineEnemy::calcRootMatrix()
{
	if (unk68 && unk68->unk6C == this) {
		MtxPtr src = getTakingMtx();
		if (src) {
			getModel()->setBaseTRMtx(src);
			mPosition.set(src[0][3], src[1][3], src[2][3]);
			return;
		}
	}

	calcEnemyRootMatrix();
}

TSpineEnemyParams* TSpineEnemy::getSaveParam() const
{
	return ((TEnemyManager*)unk70)->unk38;
}

void TSpineEnemy::resetToPosition(const JGeometry::TVec3<f32>& position)
{
	mPosition = position;
	offLiveFlag(0x8);
	offLiveFlag(0x1);
	reset();
	unk13C = getSaveParam() ? getSaveParam()->mSLHitPointMax.get() : 1;
	offHitFlag(0x1);
	unkAC = JGeometry::TVec3<f32>(0.0f, 5.0f, 0.0f);
	onLiveFlag(0x8000);
	onLiveFlag(0x80);
	control();
}

void TSpineEnemy::resetSRTV(const JGeometry::TVec3<f32>& param_1,
                            const JGeometry::TVec3<f32>& param_2,
                            const JGeometry::TVec3<f32>& param_3,
                            const JGeometry::TVec3<f32>& param_4)
{
	mPosition = param_1;
	offLiveFlag(0x8);
	offLiveFlag(0x1);
	reset();
	mRotation = param_2;
	mScaling  = param_3;
	unk13C    = getSaveParam() ? getSaveParam()->mSLHitPointMax.get() : 1;
	offHitFlag(0x1);
	unkAC = param_4;
	onLiveFlag(0x8000);
	onLiveFlag(0x80);
	control();
}

f32 TSpineEnemy::calcMinimumTurnRadius(f32 param_1, f32 param_2) const
{
	f32 result;
	if (param_2 >= 90.0f) {
		result = 0.0f;
	} else {
		f32 thing = JMASin(param_2);
		if (thing == 0.0f)
			result = 100000.0f;
		else
			result = param_1 * JMASin(-(param_2 * 0.5f - 90.0f)) / thing;
	}
	return result;
}

void TSpineEnemy::calcTurnSpeedToReach(f32 param_1, f32 param_2) const { }

void TSpineEnemy::updateSquareToMario()
{
	// assert?
	(void)gpMarioPos;
	unk134 = PSVECSquareDistance(&mPosition, gpMarioPos);
}

BOOL TSpineEnemy::receiveMessage(THitActor* param_1, u32 param_2)
{
	if (param_2 == 4 && unk68 == nullptr) {
		unk68 = (TTakeActor*)param_1;
		return true;
	}

	if (param_2 <= 2) {
		kill();
		return true;
	}

	return false;
}

bool TSpineEnemy::isInSight(const JGeometry::TVec3<f32>& param_1, f32 param_2,
                            f32 param_3, f32 param_4) const
{
	return MsIsInSight(mPosition, mRotation.y, param_1, param_2, param_3,
	                   param_4);
}

void TSpineEnemy::setGoalPathFromGraph()
{
	JGeometry::TVec3<f32> point = unk124->getCurrent().getPoint();

	unkF4  = 0;
	unkF8  = point;
	unk104 = 0;
	unk108 = point;
	unk114.clear();
}

void TSpineEnemy::goToInitialVisibleNode(f32, f32) { }

void TSpineEnemy::goToInitialGraphNodeCheckY(f32) { }

#pragma dont_inline on
void TSpineEnemy::goToShortestNextGraphNode() { }
#pragma dont_inline off

void TSpineEnemy::jumpToNextGraphNode() { }

void TSpineEnemy::goToRandomNextGraphNode() { }

void TSpineEnemy::goToRandomEscapeGraphNode() { }

void TSpineEnemy::goToExclusiveNextGraphNode() { }

void TSpineEnemy::goToDirectedNextGraphNode(const JGeometry::TVec3<f32>&) { }

void TSpineEnemy::goToDirLimitedNextGraphNode(f32) { }

void TSpineEnemy::updateStayCount(f32) { }

void TSpineEnemy::turnToCurPathNode(f32) { }

void TSpineEnemy::walkToCurPathNode(f32, f32, f32) { }

void TSpineEnemy::zigzagToCurPathNode(f32, f32, f32, f32) { }

void TSpineEnemy::doShortCut() { }

void TSpineEnemy::searchNearestBrother() const { }

f32 TSpineEnemy::getCurAnmFrameNo(int param_1) const
{
	TEnemyManager* mgr = (TEnemyManager*)unk70;

	int iVar1             = getUnk74()->getCurAnmIdx(param_1);
	TSharedMActorSet* set = mgr->getSharedMActorSet(iVar1);
	if (set == nullptr) {
		return getUnk74()->getFrameCtrl(param_1)->getCurrentFrame();
	} else {
		return set->getMActor(getUnk7C())
		    ->getFrameCtrl(param_1)
		    ->getCurrentFrame();
	}
}

BOOL TSpineEnemy::checkCurAnmEnd(int param_1) const
{
	TEnemyManager* mgr = (TEnemyManager*)unk70;

	int iVar1             = getUnk74()->getCurAnmIdx(param_1);
	TSharedMActorSet* set = mgr->getSharedMActorSet(iVar1);
	if (set == nullptr) {
		return getUnk74()->curAnmEndsNext(param_1, nullptr);
	} else {
		return set->getMActor(getUnk7C())->curAnmEndsNext(param_1, nullptr);
	}
}

void TSpineEnemy::perform(u32 param_1, JDrama::TGraphics* param_2)
{
	TEnemyManager* mgr = (TEnemyManager*)unk70;

	if (unk70 != nullptr) {
		if ((param_1 & 2) && !checkLiveFlag(0x6) && TEnemyManager::mIsCopyAnmMtx
		    && mgr->unk4C >= 0) {
			if (checkLiveFlag(0x201))
				return;

			if (mgr->copyAnmMtx(this))
				return;
		}
	} else {
		if ((param_1 & 2) && !checkLiveFlag(0x6) && checkLiveFlag(0x201)) {
			return;
		}
	}

	TLiveActor::perform(param_1, param_2);
}
