static constexpr LogField fields[] = {
{packedTime, GAUGE_NAME_TIME, "sec", 0},
	{engine->outputChannels.RPMValue, "RPM", "RPM", 0},
	{engine->outputChannels.rpmAcceleration, "dRPM", "RPM/s", 2},
	{engine->outputChannels.speedToRpmRatio, "Gearbox Ratio", "value", 2},
	{engine->outputChannels.vehicleSpeedKph, "Vehicle Speed", "kph", 1},
	{engine->outputChannels.internalMcuTemperature, "CPU Temperature", "deg C", 0},
	{engine->outputChannels.coolant, "CLT", "deg C", 1},
	{engine->outputChannels.intake, "IAT", "deg C", 1},
	{engine->outputChannels.auxTemp1, "auxTemp1", "deg C", 1},
	{engine->outputChannels.auxTemp2, "auxTemp2", "deg C", 1},
	{engine->outputChannels.TPSValue, "TPS", "%", 2},
	{engine->outputChannels.throttlePedalPosition, "Throttle pedal position", "%", 2},
	{engine->outputChannels.tpsADC, "tpsADC", "ADC", 0},
	{engine->outputChannels.rawMaf, "rawMaf", "V", 3},
	{engine->outputChannels.mafMeasured, "MAF", "kg/h", 1},
	{engine->outputChannels.MAPValue, "MAP", "kPa", 1},
	{engine->outputChannels.baroPressure, "baroPressure", "kPa", 1},
	{engine->outputChannels.lambdaValue, "Lambda", "", 3},
	{engine->outputChannels.knockRetard, "knock: retard", "deg", 1},
	{engine->outputChannels.idleCurrentPosition, "idleCurrentPosition", "%", 1},
	{engine->outputChannels.VBatt, "VBatt", "V", 2},
	{engine->outputChannels.oilPressure, "Oil Pressure", "kPa", 0},
	{engine->outputChannels.vvtPositionB1I, "VVT: bank 1 intake", "deg", 1},
	{engine->outputChannels.chargeAirMass, "air mass", "g", 3},
	{engine->outputChannels.crankingFuelMs, "crankingFuelMs", "ms", 2},
	{engine->outputChannels.baseFuel, "fuel: base mass", "mg", 3},
	{engine->outputChannels.fuelRunning, "fuel: running", "mg", 3},
	{engine->outputChannels.actualLastInjection, "fuel: Last injection", "ms", 3},
	{engine->outputChannels.injectorDutyCycle, "fuel: injector duty cycle", "%", 0},
	{engine->outputChannels.veValue, "fuel: VE", "ratio", 1},
	{engine->outputChannels.injectionOffset, "injectionOffset", "deg", 0},
	{engine->outputChannels.tCharge, "tCharge", "deg C", 1},
	{engine->outputChannels.injectorLagMs, "fuel: injector lag", "ms", 3},
	{engine->outputChannels.engineMakeCodeNameCrc16, "Engine CRC16", "crc16", 0},
	{engine->outputChannels.wallFuelAmount, "fuel: wall amount", "mg", 3},
	{engine->outputChannels.wallFuelCorrectionValue, "fuel: wall correction", "mg", 3},
	{engine->outputChannels.revolutionCounterSinceStart, "revolutionCounterSinceStart", "", 0},
	{engine->outputChannels.deltaTpsValue, "fuel: TPS change", "ratio", 0},
	{engine->outputChannels.canReadCounter, "canReadCounter", "", 0},
	{engine->outputChannels.tpsAccelFuel, "fuel: TPS acceleration add fuel ms", "ms", 3},
	{engine->outputChannels.ignitionAdvance, "timing", "deg", 1},
	{engine->outputChannels.sparkDwellValue, "dwell: coil charge time", "ms", 1},
	{engine->outputChannels.coilDutyCycle, "dwell: coil duty cycle", "%", 0},
	{engine->outputChannels.idleAirValvePosition, "Idle Air Valve", "%", 1},
	{engine->outputChannels.etbTarget, "ETB Target", "%", 2},
	{engine->outputChannels.etb1DutyCycle, "ETB Duty", "%", 1},
	{engine->outputChannels.fuelTankLevel, "Fuel level", "%", 0},
	{engine->outputChannels.totalFuelConsumption, "fuel: Total consumed", "grams", 1},
	{engine->outputChannels.fuelFlowRate, "fuel: Flow rate", "gram/s", 3},
	{engine->outputChannels.TPS2Value, "TPS2", "%", 2},
	{engine->outputChannels.tuneCrc16, "Tune CRC16", "crc16", 0},
	{engine->outputChannels.knockLevel, "knock: current level", "Volts", 2},
	{engine->outputChannels.seconds, "Uptime", "sec", 0},
	{engine->outputChannels.engineMode, "Engine Mode", "em", 0},
	{engine->outputChannels.firmwareVersion, "firmware", "version_f", 0},
	{engine->outputChannels.rawIdlePositionSensor, "rawIdlePositionSensor", "V", 3},
	{engine->outputChannels.rawWastegatePosition, "rawWastegatePosition", "V", 3},
	{engine->outputChannels.accelerationX, "Acceleration: X", "G", 2},
	{engine->outputChannels.accelerationY, "Acceleration: Y", "G", 2},
	{engine->outputChannels.tsConfigVersion, "tsConfigVersion", "", 0},
	{engine->outputChannels.calibrationValue, "calibrationValue", "", 0},
	{engine->outputChannels.calibrationMode, "calibrationMode", "", 0},
	{engine->outputChannels.idleTargetPosition, "idleTargetPosition", "", 0},
	{engine->outputChannels.detectedGear, "Detected Gear", "", 0},
	{engine->outputChannels.maxTriggerReentrant, "maxTriggerReentrant", "", 0},
	{engine->outputChannels.totalTriggerErrorCounter, "Trigger Error Counter", "counter", 0},
	{engine->outputChannels.orderingErrorCounter, "orderingErrorCounter", "", 0},
	{engine->outputChannels.warningCounter, "warning: counter", "count", 0},
	{engine->outputChannels.lastErrorCode, "warning: last", "error", 0},
	{engine->outputChannels.recentErrorCode[0], "recentErrorCode 1", "error", 0},
	{engine->outputChannels.recentErrorCode[1], "recentErrorCode 2", "error", 0},
	{engine->outputChannels.recentErrorCode[2], "recentErrorCode 3", "error", 0},
	{engine->outputChannels.recentErrorCode[3], "recentErrorCode 4", "error", 0},
	{engine->outputChannels.recentErrorCode[4], "recentErrorCode 5", "error", 0},
	{engine->outputChannels.recentErrorCode[5], "recentErrorCode 6", "error", 0},
	{engine->outputChannels.recentErrorCode[6], "recentErrorCode 7", "error", 0},
	{engine->outputChannels.recentErrorCode[7], "recentErrorCode 8", "error", 0},
	{engine->outputChannels.debugFloatField1, "debugFloatField1", "val", 3},
	{engine->outputChannels.debugFloatField2, "debugFloatField2", "val", 3},
	{engine->outputChannels.debugFloatField3, "debugFloatField3", "val", 3},
	{engine->outputChannels.debugFloatField4, "debugFloatField4", "val", 3},
	{engine->outputChannels.debugFloatField5, "debugFloatField5", "val", 3},
	{engine->outputChannels.debugFloatField6, "debugFloatField6", "val", 3},
	{engine->outputChannels.debugFloatField7, "debugFloatField7", "val", 3},
	{engine->outputChannels.debugIntField1, "debugIntField1", "val", 0},
	{engine->outputChannels.debugIntField2, "debugIntField2", "val", 0},
	{engine->outputChannels.debugIntField3, "debugIntField3", "val", 0},
	{engine->outputChannels.debugIntField4, "debugIntField4", "val", 0},
	{engine->outputChannels.debugIntField5, "debugIntField5", "val", 0},
	{engine->outputChannels.egt[0], "egt 1", "deg C", 0},
	{engine->outputChannels.egt[1], "egt 2", "deg C", 0},
	{engine->outputChannels.egt[2], "egt 3", "deg C", 0},
	{engine->outputChannels.egt[3], "egt 4", "deg C", 0},
	{engine->outputChannels.egt[4], "egt 5", "deg C", 0},
	{engine->outputChannels.egt[5], "egt 6", "deg C", 0},
	{engine->outputChannels.egt[6], "egt 7", "deg C", 0},
	{engine->outputChannels.egt[7], "egt 8", "deg C", 0},
	{engine->outputChannels.rawTps1Primary, "rawTps1Primary", "V", 3},
	{engine->outputChannels.rawPpsPrimary, "rawPpsPrimary", "V", 3},
	{engine->outputChannels.rawClt, "rawClt", "V", 3},
	{engine->outputChannels.rawIat, "rawIat", "V", 3},
	{engine->outputChannels.rawOilPressure, "rawOilPressure", "V", 3},
	{engine->outputChannels.fuelClosedLoopBinIdx, "fuelClosedLoopBinIdx", "", 0},
	{engine->outputChannels.tcuCurrentGear, "Current Gear", "gear", 0},
	{engine->outputChannels.rawPpsSecondary, "rawPpsSecondary", "V", 3},
	{engine->outputChannels.knock[0], "knock 1", "dBv", 0},
	{engine->outputChannels.knock[1], "knock 2", "dBv", 0},
	{engine->outputChannels.knock[2], "knock 3", "dBv", 0},
	{engine->outputChannels.knock[3], "knock 4", "dBv", 0},
	{engine->outputChannels.knock[4], "knock 5", "dBv", 0},
	{engine->outputChannels.knock[5], "knock 6", "dBv", 0},
	{engine->outputChannels.knock[6], "knock 7", "dBv", 0},
	{engine->outputChannels.knock[7], "knock 8", "dBv", 0},
	{engine->outputChannels.knock[8], "knock 9", "dBv", 0},
	{engine->outputChannels.knock[9], "knock 10", "dBv", 0},
	{engine->outputChannels.knock[10], "knock 11", "dBv", 0},
	{engine->outputChannels.knock[11], "knock 12", "dBv", 0},
	{engine->outputChannels.tcuDesiredGear, "Desired Gear", "gear", 0},
	{engine->outputChannels.flexPercent, "Flex Ethanol %", "%", 1},
	{engine->outputChannels.wastegatePositionSensor, "Wastegate position sensor", "%", 2},
	{engine->outputChannels.idlePositionSensor, "Idle position sensor", "%", 2},
	{engine->outputChannels.rawLowFuelPressure, "rawLowFuelPressure", "V", 3},
	{engine->outputChannels.rawHighFuelPressure, "rawHighFuelPressure", "V", 3},
	{engine->outputChannels.lowFuelPressure, "Fuel pressure (low)", "kpa", 0},
	{engine->outputChannels.highFuelPressure, "Fuel pressure (high)", "bar", 0},
	{engine->outputChannels.AFRValue, "Air/Fuel Ratio", "AFR", 2},
	{engine->outputChannels.VssAcceleration, "Vss Accel", "m/s2", 2},
	{engine->outputChannels.lambdaValue2, "Lambda 2", "", 3},
	{engine->outputChannels.AFRValue2, "Air/Fuel Ratio 2", "AFR", 2},
	{engine->outputChannels.vvtPositionB1E, "VVT: bank 1 exhaust", "deg", 1},
	{engine->outputChannels.vvtPositionB2I, "VVT: bank 2 intake", "deg", 1},
	{engine->outputChannels.vvtPositionB2E, "VVT: bank 2 exhaust", "deg", 1},
	{engine->outputChannels.fuelPidCorrection[0], "fuelPidCorrection 1", "%", 2},
	{engine->outputChannels.fuelPidCorrection[1], "fuelPidCorrection 2", "%", 2},
	{engine->outputChannels.rawTps1Secondary, "rawTps1Secondary", "V", 3},
	{engine->outputChannels.rawTps2Primary, "rawTps2Primary", "V", 3},
	{engine->outputChannels.rawTps2Secondary, "rawTps2Secondary", "V", 3},
	{engine->outputChannels.knockCount, "knock: count", "", 0},
	{engine->outputChannels.accelerationZ, "Acceleration: Z", "G", 0},
	{engine->outputChannels.accelerationRoll, "Acceleration: Roll", "G", 0},
	{engine->outputChannels.accelerationYaw, "Acceleration: Yaw", "G", 0},
	{engine->outputChannels.vvtTargets[0], "vvtTargets 1", "deg", 0},
	{engine->outputChannels.vvtTargets[1], "vvtTargets 2", "deg", 0},
	{engine->outputChannels.vvtTargets[2], "vvtTargets 3", "deg", 0},
	{engine->outputChannels.vvtTargets[3], "vvtTargets 4", "deg", 0},
	{engine->outputChannels.turboSpeed, "Turbocharger Speed", "hz", 0},
	{engine->outputChannels.tps1Split, "tps1Split", "%", 2},
	{engine->outputChannels.tps2Split, "tps2Split", "%", 2},
	{engine->outputChannels.tps12Split, "tps12Split", "%", 2},
	{engine->outputChannels.accPedalSplit, "accPedalSplit", "%", 2},
	{engine->outputChannels.sparkCutReason, "Spark Cut Code", "code", 0},
	{engine->outputChannels.fuelCutReason, "Fuel Cut Code", "code", 0},
	{engine->outputChannels.mafEstimate, "MAF estimate", "kg/h", 0},
	{engine->outputChannels.instantRpm, "instantRpm", "rpm", 0},
	{engine->outputChannels.systemEventReuse, "systemEventReuse", "counter", 0},
	{engine->outputChannels.rawMap, "rawMap", "V", 3},
	{engine->outputChannels.rawAfr, "rawAfr", "V", 3},
	{engine->outputChannels.tpsAccelFrom, "tpsAccelFrom", "%", 0},
	{engine->outputChannels.tpsAccelTo, "tpsAccelTo", "%", 0},
	{engine->outputChannels.calibrationValue2, "calibrationValue2", "", 0},
	{engine->outputChannels.luaInvocationCounter, "luaInvocationCounter", "count", 0},
	{engine->outputChannels.luaLastCycleDuration, "luaLastCycleDuration", "nt", 0},
	{engine->outputChannels.testBenchIter, "testBenchIter", "count", 0},
	{engine->outputChannels.tcu_currentRange, "Current Range", "", 0},
	{engine->outputChannels.tcRatio, "Torque Converter Ratio", "value", 0},
	{engine->outputChannels.lastShiftTime, "lastShiftTime", "", 0},
	{engine->outputChannels.vssEdgeCounter, "vssEdgeCounter", "", 0},
	{engine->outputChannels.issEdgeCounter, "issEdgeCounter", "", 0},
	{engine->outputChannels.auxLinear1, "Aux Linear #1", "", 2},
	{engine->outputChannels.auxLinear2, "Aux Linear #2", "", 2},
	{engine->outputChannels.boostControllerOutput, "Boost Output", "%", 1},
	{engine->outputChannels.boostControllerOpenLoopPart, "Boost Open Loop", "%", 1},
	{engine->outputChannels.fallbackMap, "fallbackMap", "kPa", 1},
	{engine->outputChannels.instantMAPValue, "Instant MAP", "kPa", 2},
	{engine->outputChannels.maxLockedDuration, "maxLockedDuration", "us", 0},
	{engine->outputChannels.canWriteOk, "canWriteOk", "", 3},
	{engine->outputChannels.canWriteNotOk, "canWriteNotOk", "", 3},
	{engine->outputChannels.triggerPrimaryFall, "triggerPrimaryFall", "", 3},
	{engine->outputChannels.triggerPrimaryRise, "triggerPrimaryRise", "", 3},
	{engine->outputChannels.triggerSecondaryFall, "triggerSecondaryFall", "", 3},
	{engine->outputChannels.triggerSecondaryRise, "triggerSecondaryRise", "", 3},
	{engine->outputChannels.triggerVvtFall, "triggerVvtFall", "", 3},
	{engine->outputChannels.triggerVvtRise, "triggerVvtRise", "", 3},
	{engine->outputChannels.starterState, "starterState", "", 3},
	{engine->outputChannels.starterRelayDisable, "starterRelayDisable", "", 3},
	{engine->outputChannels.multiSparkCounter, "multiSparkCounter", "", 3},
	{engine->outputChannels.extiOverflowCount, "extiOverflowCount", "", 0},
	{engine->outputChannels.alternatorStatus.pTerm, "alternatorStatus.pTerm", "", 2},
	{engine->outputChannels.alternatorStatus.iTerm, "alternatorStatus.iTerm", "", 2},
	{engine->outputChannels.alternatorStatus.dTerm, "alternatorStatus.dTerm", "", 2},
	{engine->outputChannels.alternatorStatus.output, "alternatorStatus.output", "", 2},
	{engine->outputChannels.alternatorStatus.error, "alternatorStatus.error", "", 2},
	{engine->outputChannels.alternatorStatus.resetCounter, "alternatorStatus.resetCounter", "", 0},
	{engine->outputChannels.idleStatus.pTerm, "idleStatus.pTerm", "", 2},
	{engine->outputChannels.idleStatus.iTerm, "idleStatus.iTerm", "", 2},
	{engine->outputChannels.idleStatus.dTerm, "idleStatus.dTerm", "", 2},
	{engine->outputChannels.idleStatus.output, "idleStatus.output", "", 2},
	{engine->outputChannels.idleStatus.error, "idleStatus.error", "", 2},
	{engine->outputChannels.idleStatus.resetCounter, "idleStatus.resetCounter", "", 0},
	{engine->outputChannels.etbStatus.pTerm, "etbStatus.pTerm", "", 2},
	{engine->outputChannels.etbStatus.iTerm, "etbStatus.iTerm", "", 2},
	{engine->outputChannels.etbStatus.dTerm, "etbStatus.dTerm", "", 2},
	{engine->outputChannels.etbStatus.output, "etbStatus.output", "", 2},
	{engine->outputChannels.etbStatus.error, "etbStatus.error", "", 2},
	{engine->outputChannels.etbStatus.resetCounter, "etbStatus.resetCounter", "", 0},
	{engine->outputChannels.boostStatus.pTerm, "boostStatus.pTerm", "", 2},
	{engine->outputChannels.boostStatus.iTerm, "boostStatus.iTerm", "", 2},
	{engine->outputChannels.boostStatus.dTerm, "boostStatus.dTerm", "", 2},
	{engine->outputChannels.boostStatus.output, "boostStatus.output", "", 2},
	{engine->outputChannels.boostStatus.error, "boostStatus.error", "", 2},
	{engine->outputChannels.boostStatus.resetCounter, "boostStatus.resetCounter", "", 0},
	{engine->outputChannels.auxSpeed1, "aux speed 1", "s", 0},
	{engine->outputChannels.auxSpeed2, "aux speed 2", "s", 0},
	{engine->outputChannels.ISSValue, "Input Shaft Speed", "RPM", 0},
	{engine->outputChannels.rawAnalogInput[0], "rawAnalogInput 1", "V", 3},
	{engine->outputChannels.rawAnalogInput[1], "rawAnalogInput 2", "V", 3},
	{engine->outputChannels.rawAnalogInput[2], "rawAnalogInput 3", "V", 3},
	{engine->outputChannels.rawAnalogInput[3], "rawAnalogInput 4", "V", 3},
	{engine->outputChannels.rawAnalogInput[4], "rawAnalogInput 5", "V", 3},
	{engine->outputChannels.rawAnalogInput[5], "rawAnalogInput 6", "V", 3},
	{engine->outputChannels.rawAnalogInput[6], "rawAnalogInput 7", "V", 3},
	{engine->outputChannels.rawAnalogInput[7], "rawAnalogInput 8", "V", 3},
	{engine->outputChannels.gppwmOutput[0], "GPPWM Output 1", "%", 2},
	{engine->outputChannels.gppwmOutput[1], "GPPWM Output 2", "%", 2},
	{engine->outputChannels.gppwmOutput[2], "GPPWM Output 3", "%", 2},
	{engine->outputChannels.gppwmOutput[3], "GPPWM Output 4", "%", 2},
	{engine->outputChannels.rawBattery, "rawBattery", "V", 3},
	{engine->outputChannels.ignBlendBias[0], "ignBlendBias 1", "%", 1},
	{engine->outputChannels.ignBlendBias[1], "ignBlendBias 2", "%", 1},
	{engine->outputChannels.ignBlendBias[2], "ignBlendBias 3", "%", 1},
	{engine->outputChannels.ignBlendBias[3], "ignBlendBias 4", "%", 1},
	{engine->outputChannels.ignBlendOutput[0], "ignBlendOutput 1", "deg", 2},
	{engine->outputChannels.ignBlendOutput[1], "ignBlendOutput 2", "deg", 2},
	{engine->outputChannels.ignBlendOutput[2], "ignBlendOutput 3", "deg", 2},
	{engine->outputChannels.ignBlendOutput[3], "ignBlendOutput 4", "deg", 2},
	{engine->outputChannels.veBlendBias[0], "veBlendBias 1", "%", 1},
	{engine->outputChannels.veBlendBias[1], "veBlendBias 2", "%", 1},
	{engine->outputChannels.veBlendBias[2], "veBlendBias 3", "%", 1},
	{engine->outputChannels.veBlendBias[3], "veBlendBias 4", "%", 1},
	{engine->outputChannels.veBlendOutput[0], "veBlendOutput 1", "%", 2},
	{engine->outputChannels.veBlendOutput[1], "veBlendOutput 2", "%", 2},
	{engine->outputChannels.veBlendOutput[2], "veBlendOutput 3", "%", 2},
	{engine->outputChannels.veBlendOutput[3], "veBlendOutput 4", "%", 2},
	{engine->outputChannels.outputRequestPeriod, "outputRequestPeriod", "", 0},
	{engine->outputChannels.mapFast, "mapFast", "", 0},
	{engine->outputChannels.luaGauges[0], "luaGauges 1", "value", 3},
	{engine->outputChannels.luaGauges[1], "luaGauges 2", "value", 3},
	{engine->outputChannels.rawMaf2, "rawMaf2", "V", 3},
	{engine->outputChannels.mafMeasured2, "MAF #2", "kg/h", 1},
	{engine->outputChannels.schedulingUsedCount, "schedulingUsedCount", "", 0},
	{engine->fuelComputer.afrTableYAxis, "afrTableYAxis", "%", 0},
	{engine->fuelComputer.targetLambda, "fuel: target lambda", "", 3},
	{engine->fuelComputer.targetAFR, "fuel: target AFR", "ratio", 2},
	{engine->fuelComputer.stoichiometricRatio, "fuel: stoich ratio", "ratio", 2},
	{engine->fuelComputer.sdTcharge_coff, "sdTcharge_coff", "", 0},
	{engine->fuelComputer.sdAirMassInOneCylinder, "SD cylinder mass", "", 0},
	{engine->engineState.lua.fuelAdd, "lua.fuelAdd", "", 0},
	{engine->engineState.lua.fuelMult, "lua.fuelMult", "", 0},
	{engine->engineState.sd.tCharge, "speed density", "", 0},
	{engine->engineState.sd.tChargeK, "sd.tChargeK", "", 0},
	{engine->engineState.sd.airFlow, "sd.airFlow", "", 0},
	{engine->engineState.engineCycleDurationMs, "engineCycleDurationMs", "", 0},
	{engine->engineState.minRpmKcurrentTPS, "minRpmKcurrentTPS", "", 0},
	{engine->engineState.currentTpsAdc, "currentTpsAdc", "", 0},
	{engine->engineState.tpsVoltageMCU, "tpsVoltageMCU", "", 0},
	{engine->engineState.tpsVoltageBoard, "tpsVoltageBoard", "", 0},
	{engine->engineState.currentBaroCorrectedVE, "currentBaroCorrectedVE", "", 0},
	{engine->engineState.injectorFlowCorrection, "injectorFlowCorrection", "", 0},
	{engine->engineState.baroCorrection, "fuel: Barometric pressure mult", "", 0},
	{engine->engineState.crankingFuel.baseFuel, "Fuel: base duration", "", 0},
	{engine->engineState.crankingFuel.coolantTemperatureCoefficient, "Cranking: CLT coefficient", "", 0},
	{engine->engineState.crankingFuel.tpsCoefficient, "crankingFuel.tpsCoefficient", "", 0},
	{engine->engineState.crankingFuel.durationCoefficient, "crankingFuel.durationCoefficient", "", 0},
	{engine->engineState.crankingFuel.fuel, "Fuel: Actual injection", "", 0},
	{engine->engineState.running.postCrankingFuelCorrection, "running.postCrankingFuelCorrection", "", 0},
	{engine->engineState.running.intakeTemperatureCoefficient, "fuel: IAT correction", "", 0},
	{engine->engineState.running.coolantTemperatureCoefficient, "fuel: CLT correction", "", 0},
	{engine->engineState.running.timeSinceCrankingInSecs, "running.timeSinceCrankingInSecs", "", 0},
	{engine->engineState.running.baseFuel, "running.baseFuel", "", 0},
	{engine->engineState.running.fuel, "Fuel: actual", "", 0},
	{engine->engineState.sparkDwell, "ignition dwell duration in ms", "", 0},
	{engine->engineState.dwellAngle, "ignition dwell duration", "", 0},
	{engine->engineState.cltTimingCorrection, "cltTimingCorrection", "deg", 2},
	{engine->engineState.timingIatCorrection, "timingIatCorrection", "deg", 2},
	{engine->engineState.timingPidCorrection, "timingPidCorrection", "deg", 2},
	{engine->engineState.hellenBoardId, "Detected Board ID", "id", 0},
	{engine->engineState.startStopStateToggleCounter, "startStopStateToggleCounter", "", 0},
	{engine->engineState.egtValue1, "egtValue1", "", 0},
	{engine->engineState.egtValue2, "egtValue2", "", 0},
	{engine->engineState.desiredRpmLimit, "User-defined RPM hard limit", "rpm", 0},
	{engine->engineState.fuelInjectionCounter, "fuelInjectionCounter", "", 0},
	{engine->engineState.sparkCounter, "sparkCounter", "", 0},
	{engine->engineState.fuelingLoad, "fuel: load", "", 0},
	{engine->engineState.ignitionLoad, "ign: load", "", 0},
	{engine->engineState.veTableYAxis, "veTableYAxis", "%", 0},
	{engine->triggerCentral.hwEventCounters[0], "Hardware events since boot 1", "", 0},
	{engine->triggerCentral.hwEventCounters[1], "Hardware events since boot 2", "", 0},
	{engine->triggerCentral.hwEventCounters[2], "Hardware events since boot 3", "", 0},
	{engine->triggerCentral.hwEventCounters[3], "Hardware events since boot 4", "", 0},
	{engine->triggerCentral.hwEventCounters[4], "Hardware events since boot 5", "", 0},
	{engine->triggerCentral.hwEventCounters[5], "Hardware events since boot 6", "", 0},
	{engine->triggerCentral.vvtCamCounter, "vvtCamCounter", "", 0},
	{engine->triggerCentral.mapVvt_MAP_AT_SPECIAL_POINT, "mapVvt_MAP_AT_SPECIAL_POINT", "", 3},
	{engine->triggerCentral.mapVvt_MAP_AT_DIFF, "mapVvt_MAP_AT_DIFF", "", 3},
	{engine->triggerCentral.mapVvt_MAP_AT_CYCLE_COUNT, "mapVvt_MAP_AT_CYCLE_COUNT", "", 3},
	{engine->triggerCentral.mapVvt_map_peak, "mapVvt_map_peak", "", 3},
	{engine->triggerCentral.currentEngineDecodedPhase, "Engine Phase", "deg", 0},
	{engine->triggerCentral.triggerToothAngleError, "triggerToothAngleError", "deg", 2},
	{engine->triggerCentral.triggerIgnoredToothCount, "triggerIgnoredToothCount", "", 0},
};
