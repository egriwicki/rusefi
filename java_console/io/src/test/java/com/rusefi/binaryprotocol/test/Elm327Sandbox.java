package com.rusefi.binaryprotocol.test;

import com.rusefi.binaryprotocol.BinaryProtocol;
import com.rusefi.binaryprotocol.IncomingDataBuffer;
import com.rusefi.config.generated.Fields;
import com.rusefi.io.IoStream;
import com.rusefi.io.can.Elm327Connector;
import com.rusefi.io.serial.BaudRateHolder;
import com.rusefi.io.serial.SerialIoStream;

import java.io.IOException;

import static com.rusefi.binaryprotocol.IoHelper.checkResponseCode;
import static com.rusefi.io.can.Elm327Connector.ELM327_DEFAULT_BAUDRATE;

public class Elm327Sandbox {
    public static void main(String[] args) throws InterruptedException, IOException {
        BaudRateHolder.INSTANCE.baudRate = ELM327_DEFAULT_BAUDRATE;
        String serialPort = "COM7";
        Elm327Connector connector = new Elm327Connector(SerialIoStream.openPort(serialPort));
        connector.start("sandbox");

        IoStream tsStream = connector.getTsStream();

        IncomingDataBuffer dataBuffer = tsStream.getDataBuffer();
        System.out.println("Hello new connection " + dataBuffer.getPendingCount());

        runFcommand("First time", tsStream);
        Elm327Connector.whyDoWeNeedToSleepBetweenCommands();

        runFcommand("Second time", tsStream);
        Elm327Connector.whyDoWeNeedToSleepBetweenCommands();

        {
            String signature = BinaryProtocol.getSignature(tsStream);
            System.out.println("Got " + signature + " signature via CAN/ELM327");
        }

        Elm327Connector.whyDoWeNeedToSleepBetweenCommands();

        {
            String signature = BinaryProtocol.getSignature(tsStream);
            System.out.println("Let's do it again! Got " + signature + " signature via CAN/ELM327");
        }

        Elm327Connector.whyDoWeNeedToSleepBetweenCommands();

        {
            tsStream.sendPacket(new byte[]{Fields.TS_HELLO_COMMAND});
            byte[] response = dataBuffer.getPacket("[hello command]");
            if (!checkResponseCode(response, (byte) Fields.TS_RESPONSE_OK))
                return;
            String signature = new String(response, 1, response.length - 1);
            System.out.println(Fields.TS_HELLO_COMMAND + " returned " + signature);
        }

        Elm327Connector.whyDoWeNeedToSleepBetweenCommands();

        {
            tsStream.sendPacket(BinaryProtocol.createCrcCommand(1000));
            byte[] fResponse = new byte[3];
            dataBuffer.waitForBytes("CRC", System.currentTimeMillis(), fResponse.length);
            dataBuffer.getData(fResponse);
            System.out.println(" Got CRC response " + IoStream.printHexBinary(fResponse));
        }

    }

    private static void runFcommand(String prefix, IoStream tsStream) throws IOException {
        IncomingDataBuffer dataBuffer = tsStream.getDataBuffer();
        tsStream.sendPacket(new byte[]{Fields.TS_COMMAND_F});
        byte[] fResponse = new byte[3];
        dataBuffer.waitForBytes("hello", System.currentTimeMillis(), fResponse.length);
        dataBuffer.getData(fResponse);
        System.out.println(prefix + " Got F response " + IoStream.printHexBinary(fResponse));
    }

}
