#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msg.h"
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk_glue.h"
#include "PowerPC_EABI_Support/MetroTRK/trk.h"

DSError TRKMessageSend(TRK_Msg* msg)
{
	return TRKWriteUARTN(&msg->m_msg, msg->m_msgLength);
}
