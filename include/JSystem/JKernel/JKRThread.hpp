#ifndef JKR_THREAD_H
#define JKR_THREAD_H

#include <types.h>
#include <dolphin/os/OSMessage.h>
#include <JSystem/JSupport/JSUList.hpp>
#include <JSystem/JKernel/JKRHeap.hpp>
#include <JSystem/JKernel/JKRDisposer.hpp>

struct JKRThread;
class JUTConsole;

struct JKRThreadName_ {
	s32 id;
	char* name;
};

typedef void (*JKRThreadSwitch_PreCallback)(OSThread* current, OSThread* next);
typedef void (*JKRThreadSwitch_PostCallback)(OSThread* current, OSThread* next);

class JKRThreadSwitch {
public:
	JKRThreadSwitch(JKRHeap*);
	virtual void draw(JKRThreadName_* param_1, JUTConsole* param_2);
	virtual void draw(JKRThreadName_* param_1);
	virtual ~JKRThreadSwitch();

	static JKRThreadSwitch* createManager(JKRHeap* heap);

	JKRThread* enter(JKRThread* param_1, int param_2);
	static void callback(OSThread* param_1, OSThread* param_2);

	static u32 getTotalCount() { return sTotalCount; }

private:
	static JKRThreadSwitch* sManager;
	static u32 sTotalCount;
	static u64 sTotalStart;
	static JKRThreadSwitch_PreCallback mUserPreCallback;
	static JKRThreadSwitch_PostCallback mUserPostCallback;

private:
	JKRHeap* mHeap;              // _04
	bool mSetNextHeap;           // _08
	u8 _09[3];                   // _09, padding?
	u32 _0C;                     // _0C
	u32 _10;                     // _10
	u8 _14[4];                   // _14 - unknown/padding
	s64 _18;                     // _18
	JUTConsole* mConsole;        // _20
	JKRThreadName_* mThreadName; // _24
};

class JKRThread : public JKRDisposer {
public:
	JKRThread(u32 stackSize, int msgCount, int threadPrio);
	JKRThread(OSThread* osThread, int msgCount);

	virtual ~JKRThread();
	virtual void* run() { return nullptr; }

	static void* start(void* param);
	static JSUList<JKRThread>* getList() { return &JKRThread::sThreadList; }

	OSThread* getThreadRecord() const { return mThreadRecord; }
	void* getStack() const { return mStackMemory; }

	void resume() { OSResumeThread(mThreadRecord); }
	void jamMessageBlock(OSMessage msg)
	{
		OSJamMessage(&mMesgQueue, msg, OS_MESSAGE_BLOCK);
	}
	void sendMessage(OSMessage msg)
	{
		OSSendMessage(&mMesgQueue, msg, OS_MESSAGE_NOBLOCK);
	}

	OSMessage waitMessage(int* received)
	{
		OSMessage mesg;
		BOOL retrieved
		    = OSReceiveMessage(&mMesgQueue, &mesg, OS_MESSAGE_NOBLOCK);
		if (received != nullptr) {
			*received = retrieved;
		}
		return mesg;
	}

	OSMessage waitMeessageBlock()
	{
		OSMessage mesg;
		OSReceiveMessage(&mMesgQueue, &mesg, OS_MESSAGE_BLOCK);
		return mesg;
	}

	static JSUList<JKRThread> sThreadList;

protected:
	/* 0x18 */ JSULink<JKRThread> mLink;
	/* 0x28 */ JKRHeap* mHeap;
	/* 0x2C */ OSThread* mThreadRecord;
	/* 0x30 */ OSMessageQueue mMesgQueue;
	/* 0x50 */ OSMessage* mMesgBuffer;
	/* 0x54 */ int mMesgCount;
	/* 0x58 */ void* mStackMemory;
	/* 0x5C */ u32 mStackSize;
};

#endif
