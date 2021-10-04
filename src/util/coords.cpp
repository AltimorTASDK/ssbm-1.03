#include "util/coords.h"
#include "hsd/pad.h"

static int clamp_pad_index(int index)
{
	return (HSD_PadLibData.qnum + (index % HSD_PadLibData.qnum)) % HSD_PadLibData.qnum;
}

static const PADStatus &get_input(int index, int port)
{
	return HSD_PadLibData.queue[index].stat[port];
}

bool check_ucf_xsmash(const Player *player)
{
	const auto port = player->port;
	const auto &prev_input = get_input(clamp_pad_index(HSD_PadLibData.qread - 3), port);
	const auto &current_input = get_input(clamp_pad_index(HSD_PadLibData.qread - 1), port);
	return std::abs(current_input.stick.x - prev_input.stick.x) > 75;
}