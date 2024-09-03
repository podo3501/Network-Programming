#pragma once

enum class PacketType : int
{
	PT_Hello,
	PT_ReplicationData,
	PT_Disconnect,
	PT_Max,
};
