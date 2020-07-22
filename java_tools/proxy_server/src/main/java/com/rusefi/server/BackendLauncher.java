package com.rusefi.server;

import com.opensr5.Logger;
import com.rusefi.LocalApplicationProxy;
import com.rusefi.tools.online.HttpUtil;

import java.net.MalformedURLException;

public class BackendLauncher {
    /**
     * need this method to be not in Backend class for console to work without all backend classes
     */
    public static void start(String[] args) throws MalformedURLException {
        /* todo
        rusEFISSLContext.setupCertificates(new File("keystore.jks"), System.getProperty("RUSEFI_PROXY_PASSWORD"));
         */

        UserDetailsResolver userDetailsFunction = new JsonUserDetailsResolver();

        Backend backend = new Backend(userDetailsFunction, HttpUtil.HTTP_PORT, Logger.CONSOLE);
        backend.runApplicationConnector(LocalApplicationProxy.SERVER_PORT_FOR_APPLICATIONS, parameter -> {
        });
        backend.runControllerConnector(Backend.SERVER_PORT_FOR_CONTROLLERS, parameter -> {
        });

    }
}
