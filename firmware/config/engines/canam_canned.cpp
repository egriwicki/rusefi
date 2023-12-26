
static void cannedsparkDwellValues() {
	static const float hardCodedsparkDwellValues[8] = {4.0, 3.3, 3.2, 3.1, 3.05, 2.97, 2.88, 2.8};
	copyArray(config->sparkDwellValues, hardCodedsparkDwellValues);
}

static void canneddwellVoltageCorrValues() {
	static const float hardCodeddwellVoltageCorrValues[8] = {1.26, 1.2, 1.16, 1.1, 1.06, 1.0, 0.9, 0.8};
	copyArray(engineConfiguration->dwellVoltageCorrValues, hardCodeddwellVoltageCorrValues);
}

static void cannedidleAdvanceBins() {
	static const float hardCodedidleAdvanceBins[8] = {600.0, 800.0, 1000.0, 1200.0, 1400.0, 1600.0, 1800.0, 2000.0};
	copyArray(config->idleAdvanceBins, hardCodedidleAdvanceBins);
}

static void cannedidleAdvance() {
	static const float hardCodedidleAdvance[8] = {10.0, 10.571428, 11.142858, 11.714286, 12.285714, 12.857142, 13.428572, 14.0};
	copyArray(config->idleAdvance, hardCodedidleAdvance);
}

static void cannedignitionRpmBins() {
#if (IGN_RPM_COUNT == DEFAULT_IGN_RPM_COUNT) && (IGN_LOAD_COUNT == DEFAULT_IGN_LOAD_COUNT)
	static const float hardCodedignitionRpmBins[16] = {800.0, 1000.0, 1300.0, 1600.0, 2000.0, 2500.0, 3000.0, 3600.0, 4000.0, 4400.0, 4900.0, 5300.0, 5800.0, 6200.0, 7100.0, 8000.0};
	copyArray(config->ignitionRpmBins, hardCodedignitionRpmBins);
#endif
}

static void cannedignitionLoadBins() {
#if (IGN_RPM_COUNT == DEFAULT_IGN_RPM_COUNT) && (IGN_LOAD_COUNT == DEFAULT_IGN_LOAD_COUNT)
	static const float hardCodedignitionLoadBins[16] = {15.0, 27.0, 33.0, 45.0, 57.0, 69.0, 81.0, 93.0, 105.0, 129.0, 157.0, 186.0, 214.0, 243.0, 271.0, 300.0};
	copyArray(config->ignitionLoadBins, hardCodedignitionLoadBins);
#endif
}

static void cannedveRpmBins() {
#if (IGN_LOAD_COUNT == DEFAULT_IGN_LOAD_COUNT) && (IGN_RPM_COUNT == DEFAULT_IGN_RPM_COUNT)
	static const float hardCodedveRpmBins[16] = {800.0, 1000.0, 1300.0, 1600.0, 2000.0, 2500.0, 3000.0, 3600.0, 4000.0, 4400.0, 4900.0, 5300.0, 5800.0, 6200.0, 7100.0, 8000.0};
	copyArray(config->veRpmBins, hardCodedveRpmBins);
#endif
}

static void cannedveLoadBins() {
#if (IGN_LOAD_COUNT == DEFAULT_IGN_LOAD_COUNT) && (IGN_RPM_COUNT == DEFAULT_IGN_RPM_COUNT)
	static const float hardCodedveLoadBins[16] = {15.0, 27.0, 33.0, 45.0, 57.0, 69.0, 81.0, 93.0, 105.0, 129.0, 157.0, 186.0, 214.0, 243.0, 271.0, 300.0};
	copyArray(config->veLoadBins, hardCodedveLoadBins);
#endif
}

static void cannedboostRpmBins() {
#if (BOOST_LOAD_COUNT == 8) && (BOOST_RPM_COUNT == 8)
	static const float hardCodedboostRpmBins[8] = {0.0, 1100.0, 2300.0, 3400.0, 4600.0, 5700.0, 6900.0, 8000.0};
	copyArray(config->boostRpmBins, hardCodedboostRpmBins);
#endif
}

static void cannedpedalToTpsRpmBins() {
	static const float hardCodedpedalToTpsRpmBins[8] = {0.0, 1100.0, 2300.0, 3400.0, 4600.0, 5700.0, 6900.0, 8000.0};
	copyArray(config->pedalToTpsRpmBins, hardCodedpedalToTpsRpmBins);
}

static void cannedcltIdleCorr() {
	static const float hardCodedcltIdleCorr[16] = {1.1, 1.1, 0.9333, 0.9333, 0.9333, 0.9333, 0.9333, 0.9333, 0.9333, 0.8333, 0.7667, 0.65, 0.6, 0.6, 0.6, 0.6};
	copyArray(config->cltIdleCorr, hardCodedcltIdleCorr);
}

static void cannedcltCrankingCorr() {
	static const float hardCodedcltCrankingCorr[8] = {1.0, 1.0, 1.0, 0.7, 0.7, 0.7, 0.7, 0.7};
	copyArray(config->cltCrankingCorr, hardCodedcltCrankingCorr);
}
