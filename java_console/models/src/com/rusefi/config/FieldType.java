package com.rusefi.config;

public enum FieldType {
    // Signed
    INT8(1),
    INT16(2),
    INT(4),

    // Unsigned
    UINT8(1),
    UINT16(2),

    BIT(/*bits are stored in 4 byte packs */4),
    FLOAT(4);

    public static final String INT_TYPE_STRING = "int";
    public static final String FLOAT_TYPE_STRING = "float";
    public static final String BYTE_TYPE_STRING = "byte";
    public static final String UBYTE_TYPE_STRING = "ubyte";
    public static final String SHORT_TYPE_STRING = "short";
    public static final String USHORT_TYPE_STRING = "ushort";

    private final int storageSize;

    FieldType(int storageSize) {
        this.storageSize = storageSize;
    }

    private String getTypeForCommand() {
        switch (this) {
            case FLOAT:
                return FLOAT_TYPE_STRING;
            case INT16:
                return SHORT_TYPE_STRING;
            case UINT16:
                return USHORT_TYPE_STRING;
            case INT8:
                return BYTE_TYPE_STRING;
            case UINT8:
                return UBYTE_TYPE_STRING;
            case INT:
            default:
                return INT_TYPE_STRING;
        }
    }

    public String getLoadCommand() {
        return "get_" + getTypeForCommand();
    }

    public String getStoreCommand() {
        return "set_" + getTypeForCommand();
    }

    public int getStorageSize() {
        return storageSize;
    }
}