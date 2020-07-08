package com.rusefi.ui.storage;

import com.rusefi.shared.FileUtil;

import java.beans.XMLDecoder;
import java.beans.XMLEncoder;
import java.io.*;
import java.util.HashMap;
import java.util.Map;

public class PersistentConfiguration {
    private static final PersistentConfiguration INSTANCE = new PersistentConfiguration();

    private static boolean hookRegistered;

    static {
        new File(FileUtil.RUSEFI_SETTINGS_FOLDER).mkdirs();
    }

    private static final String CONFIG_FILE_NAME = FileUtil.RUSEFI_SETTINGS_FOLDER + File.separator + "console_properties.xml";

    private Map<String, Object> config = new HashMap<>();
    private boolean isLoaded;

    public static PersistentConfiguration getConfig() {
        return INSTANCE;
    }

    private PersistentConfiguration() {
    }

    /**
     * Does not work under TS plugin looks like TS does a violent System.exit?
     */
    public static synchronized void registerShutdownHook() {
        if (hookRegistered) {
            hookRegistered = true;
            return;
        }
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            System.out.println("Shutdown hook...");
            getConfig().save();
            System.out.println("Shutdown hook!");
        }));
    }

    @SuppressWarnings("unchecked")
    public void load() {
        try {
            BufferedInputStream inputStream = new BufferedInputStream(new FileInputStream(CONFIG_FILE_NAME));
            /**
             * interesting things are happening while this is executed under TS as a plugin so we need non-default CL parameter
             */
            XMLDecoder e = new XMLDecoder(inputStream, null, null, PersistentConfiguration.class.getClassLoader());
            config = (Map<String, Object>) e.readObject();
            e.close();
            System.out.println("Console configuration from " + CONFIG_FILE_NAME);
        } catch (Throwable e) {
            System.out.println("Console configuration not found " + CONFIG_FILE_NAME + ", using defaults");
        }
        isLoaded = true;
    }

    /**
     * todo: maybe the shutdown hook is the only place where this method should be invoked?
     */
    public void save() {
        if (!isLoaded) {
            // settings not loaded, nothing to save
            return;
        }
        try {
            XMLEncoder e = new XMLEncoder(new BufferedOutputStream(new FileOutputStream(CONFIG_FILE_NAME)));
            e.writeObject(config);
            e.close();
            System.out.println("Saved settings to " + CONFIG_FILE_NAME);
        } catch (FileNotFoundException e) {
            System.out.println("Error saving " + CONFIG_FILE_NAME + e);
        }
    }

    public Node getRoot() {
        if (!isLoaded)
            load();
        return new Node("root", config);
    }
}