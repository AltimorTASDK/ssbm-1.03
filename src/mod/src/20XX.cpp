extern "C" char SaveFileName[25];

static const auto result = SaveFileName[0] == 'X' && SaveFileName[1] == 'X';

extern "C" bool is_20XX()
{
	// XXperSmashBros
	return result;
}