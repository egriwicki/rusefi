package com.rusefi.io.serial;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;
import com.opensr5.Logger;
import com.opensr5.io.DataListener;
import com.rusefi.io.IoStream;

import java.io.IOException;

/**
 * https://github.com/Fazecast/jSerialComm looks to be alive as of 2019
 * <p>
 * (c) Andrey Belomutskiy
 * 06/03/2019
 */
public class SerialIoStreamJSerialComm implements IoStream {
    private boolean isClosed;
    private SerialPort sp;

    SerialIoStreamJSerialComm(SerialPort sp) {
        this.sp = sp;
    }

    @Override
    public void setInputListener(DataListener listener) {
        sp.addDataListener(new SerialPortDataListener() {
            @Override
            public int getListeningEvents() { return SerialPort.LISTENING_EVENT_DATA_AVAILABLE; }
            @Override
            public void serialEvent(SerialPortEvent event)
            {
                if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE)
                    return;
                byte[] newData = new byte[sp.bytesAvailable()];
                int numRead = sp.readBytes(newData, newData.length);
                byte[] data = new byte[numRead];
                System.arraycopy(newData, 0, data, 0, numRead);
                listener.onDataArrived(data);
                //System.out.println("Read " + numRead + " bytes.");
            }
        });

    }

    @Override
    public boolean isClosed() {
        return isClosed;
    }

    @Override
    public void close() {
        isClosed = true;
    }

    @Override
    public void purge() {

    }

    @Override
    public void write(byte[] bytes) {
        sp.writeBytes(bytes, bytes.length);
    }

    public static IoStream open(String port, int baudRate, Logger logger) {
        logger.info("[SerialIoStreamJSerialComm]");
        SerialPort sp = SerialPort.getCommPort(port);
        sp.setBaudRate(baudRate);
        sp.openPort();


        return new SerialIoStreamJSerialComm(sp);
    }
}
