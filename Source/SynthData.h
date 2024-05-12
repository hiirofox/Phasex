//Synth Data
const float LimV = 12.0, LimK = 0.02;
typedef struct
{
	HilbertData hilbl;
	HilbertData hilbr;
	PhasexPhaserData phsl;
	PhasexPhaserData phsr;
	Filter1Data hpf1l;
	Filter1Data hpf1r;
	double lfot = 0, ctoft = 0;
	float fdbkl = 0, fdbkr = 0;

	double lforate2 = 0;
	float ctof2 = 0;

	int isInit = 0;

}PhasexData;