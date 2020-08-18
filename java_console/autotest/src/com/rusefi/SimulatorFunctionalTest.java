package com.rusefi;

import com.rusefi.io.LinkManager;

public class SimulatorFunctionalTest {
    public static void main(String[] args) {
        Thread.setDefaultUncaughtExceptionHandler((t, e) -> {
            e.printStackTrace();
            System.exit(-1);
        });
        boolean startSimulator = args.length == 1 && args[0].equalsIgnoreCase("start");

        long start = System.currentTimeMillis();
        FileLog.SIMULATOR_CONSOLE.start();
        FileLog.MAIN.start();

        boolean failed = false;
        try {
            LinkManager linkManager = new LinkManager();
            IoUtil.connectToSimulator(linkManager, startSimulator);
            new AutoTest(linkManager, linkManager.getCommandQueue()).mainTestBody();
        } catch (Throwable e) {
            e.printStackTrace();
            failed = true;
        } finally {
            SimulatorExecHelper.destroy();
        }
        if (failed)
            System.exit(-1);
        FileLog.MAIN.logLine("*******************************************************************************");
        FileLog.MAIN.logLine("************************************  Looks good! *****************************");
        FileLog.MAIN.logLine("*******************************************************************************");
        long time = (System.currentTimeMillis() - start) / 1000;
        FileLog.MAIN.logLine("Done in " + time + "secs");
        System.exit(0); // this is a safer method eliminating the issue of non-daemon threads
    }
}
